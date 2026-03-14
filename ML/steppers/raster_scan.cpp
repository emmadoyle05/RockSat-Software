#include <pigpio.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <iomanip>
#include <csignal>
#include <chrono>

// g++ -o raster_scan raster_scan.cpp -lpigpio -lrt -std=c++17

// TMC2209 Stepper Driver communication class
// Used since the driver allows for up to 4 stepper drivers to communicate
// over the same UART line.
//
// ALTITUDE STEPPER: MS1 - HIGH; MS2 - LOW 
// AZIMUTH STEPPER: MS1 - LOW; MS2 - LOW
class TMC2209 {
private:
	static int uart_fd;
	uint8_t slave_addr;
	
	// UART communication requires a specific CRC 8 based on the polynomial x^8+x^2+x^1+x^0 (0x07)
	// Calculates from Least Significant Bit (LSB) to Most Significant Bit (MSB)
	uint8_t calculate_CRC(const uint8_t* datagram, size_t len) {
		uint8_t crc = 0;
		
		for (size_t i = 0; i < len; i++) {
			uint8_t current_byte = datagram[i];
			
			for (int j=0; j < 8; j++) {
				if ((crc >> 7) ^ (current_byte & 0x01)) {
					crc = (crc << 1) ^ 0x07;
				} else {
					crc = (crc << 1);
				}
				current_byte >>= 1;
			}
		}
		return crc;
	}

public:
	static bool init_shared_UART(const char* device = "/dev/ttyS0") {
		uart_fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
		if (uart_fd < 0) {
			std::cerr << "Failed to open UART device " << device << "\n";
			return false;
		}
		
		// Setting up UART stuff with termios; configuration
		struct termios options;
		tcgetattr(uart_fd, &options);
		cfsetispeed(&options, B115200);
		cfsetospeed(&options, B115200);
		options.c_cflag |= (CLOCAL | CREAD | CS8);
		options.c_lflag &= ~(ICANON | ECHO | ISIG);
		tcsetattr(uart_fd, TCSANOW, &options);
		return true;
	}
	
	TMC2209(uint8_t addr) : slave_addr(addr) {}
	
	void write_register(uint8_t reg, uint32_t value) {
		uint8_t frame[8];
		// SYNC
		frame[0] = 0x05;
		// Slave address
		frame[1] = slave_addr;
		// Register + write bit
		frame[2] = reg | 0x80;
		// Data
		frame[3] = (value >> 24) & 0xFF;
		frame[4] = (value >> 16) & 0xFF;
		frame[5] = (value >> 8) & 0xFF;
		frame[6] = value & 0xFF;
		frame[7] = calculate_CRC(frame, 7);
		
		// Writes to/from pins 14,15 on the Pi
		write(uart_fd, frame, 8);
		usleep(5000);
	}
	
	void configure_stepper (int mA_rms = 600, int microsteps = 16) {
		// GCCONF: UART mode, internal Rsense
		write_register(0x00, 0x0000C408);
		
		// IHOLD_IRUN: 600mA RMS (setting through code instead of potentiometer)
		// IHOLDDELAY = 8; IRUN=20; IHOLD = 8
		uint32_t ihold_irun = (8 << 16) | (20 << 8) | 8;
		write_register(0x10, ihold_irun);
		
		// CHOPCONF: 1/16 microstepping
		// 1/16 -> mres = 4
		int mres = 4;
		uint32_t chopconf = (0x10000) | (mres << 24) | 0x000F0000;
		write_register(0x6C, chopconf); 
		
		// PWMCONF: StealthChop enabled (silent)
		write_register(0x70, 0x000114D4);  
	}
}; // End TMC2209 class

// Still need to set up the UART so initializing to not set up
int TMC2209::uart_fd = -1;

// Placeholder class for the VL53L8CX stuff
// Needs to be before steppers since Stepper classes both use this.
void capture_lidar_data(int az_steps, int alt_steps) {
	// In order for the 8x8 rate (~15Hz), we need ~67ms for 8x8 frame
	const int SENSOR_PAUSE_US = 67000;
	const double steps_per_deg = 3200.0 / 360.0;
	double az_deg = az_steps / steps_per_deg; 
	double alt_deg = alt_steps / steps_per_deg;
	
	std::cout << "[DATA] Azimuth: " << az_deg << " deg | Altitude: " << alt_deg << " deg" << std::endl;
	
	usleep(SENSOR_PAUSE_US);
}

// Class for the steppers; uses the TMC2209 driver class from above
class Stepper {
public:
	int step_pin, dir_pin;
	TMC2209 tmc;
	
	Stepper(int step, int dir, uint8_t tmc_addr) : step_pin(step), dir_pin(dir), tmc(tmc_addr) {
		gpioSetMode(step_pin, PI_OUTPUT);
		gpioSetMode(dir_pin, PI_OUTPUT);
		gpioWrite(step_pin, 0);
	}
	
	void configure() {
		// Steppers always use a 600mA, 1/16 microstep TMC2209 set up
		tmc.configure_stepper(600, 16);
	}
	
	// Move a single step in a given direction
	void move(int dir, int pulse_us = 1000) {
		gpioWrite(dir_pin, dir > 0 ? 1 : 0);
		gpioWrite(step_pin, 1);
		usleep(pulse_us);
		gpioWrite(step_pin, 0);
		usleep(pulse_us);
	}

	// Move n steps in a given direction
	void move_steps(int steps, int pulse_us = 1000) {
		// Use the sign of steps to move clockwise (1) or counterclockwise (-1)
		int dir = (steps >= 0) ? 1 : -1;
		int count = std::abs(steps);

		for (int i = 0; i < count; ++i){
			move(dir, pulse_us);
		}
	}
}; // End Stepper class

class Azimuth : public Stepper {
private:
	// 45 deg at 1/16
	static const int STEPS_45 = 400;
public:
	using Stepper::Stepper;

	void step_forward(int n = 1, int pulse_us = 1000) {
		move_steps(+n, pulse_us); 
	}

	void step_backward(int n = 1, int pulse_us = 1000) {
		move_steps(-n, pulse_us);
	}

	void next_chunk(int pulse_us = 1000) {
		std::cout << " AZ: +45 chunk ";
		step_forward(STEPS_45, pulse_us);
	}

	// Home from end position; at the end of 3 chunks (135)
	void home(int pulse_us = 1000) {
		std::cout << " AZ: Homing (135 deg back)" << std::endl;
		step_backward(3 * STEPS_45, pulse_us);
	}
}; // End Azimuth class

class Altitude : public Stepper {
private:
	static const int STEPS_90 = 800;
public:
	using Stepper::Stepper;

	void step_up(int n = 1, int pulse_us = 1000) {
		move_steps(+n, pulse_us);
	}

	void step_down(int n=1, int pulse_us = 1000) {
		move_steps(-n, pulse_us);
	}

	void full_raster_sweep(int pulse_us = 1000) {
		std::cout << " ALT: 0 to 90 sweep";
		step_up(STEPS_90, pulse_us);
		capture_lidar_data(0, STEPS_90);

		std::cout << " ALT: 90 to 0 sweep";
		step_down(STEPS_90, pulse_us);
		capture_lidar_data(0, 0);
	}
}; // End Altitude class

class ScanLogger {
private:
	std::ofstream csv_file;
	std::vector<double> scan_times;
	int scan_count = 0;
	std::chrono::high_resolution_clock::time_point program_start;	
public:
	ScanLogger() : program_start(std::chrono::high_resolution_clock::now()){
		// Making it so that a new csv is created each time the program runs
		// Names the new CSV after the time run: easy fix
		auto now = std::chrono::system_clock::now();
		auto time_t = std::chrono::system_clock::to_time_t(now);
		auto tm = *std::localtime(&time_t);
		
		std::ostringstream filename;
		
		filename << "stepper_test_history__" << std::put_time(&tm, "%Y-%m-%d__%H-%M") << ".csv";
		
		csv_file.open(filename.str());
		csv_file << "scan_id,timestamp_ms,duration_s\n";
		csv_file.flush();
		
		std::cout << "Test data logging to: " << filename.str() << std::endl; 
	}

	~ScanLogger() {
		csv_file.close();
	}

	std::chrono::high_resolution_clock::time_point start_scan() {
		return std::chrono::high_resolution_clock::now();
	}

	void log_scan(std::chrono::high_resolution_clock::time_point scan_start_time) {
		auto scan_end = std::chrono::high_resolution_clock::now();
		auto program_elapsed_s = std::chrono::duration_cast<std::chrono::seconds>(scan_end - program_start).count();
		
		double scan_duration_s = std::chrono::duration<double>(scan_end - scan_start_time).count();

		scan_times.push_back(scan_duration_s);
		scan_count++;

		csv_file << scan_count << "," << program_elapsed_s << "," << scan_duration_s << "\n";
		csv_file.flush();

		std::cout << "[LOG] Scan #" << scan_count << " | Duration: " << scan_duration_s << " ms | Total time: " << program_elapsed_s/1000.0 << "s\n";
	}

	void print_summary() {
		if (scan_times.empty()) {
			std::cout << "No scans completed." << std::endl;
			return;
		}

		double mean_time = 0;
		double min_time = scan_times[0];
		double max_time = scan_times[0];

		for (double t : scan_times) {
			mean_time += t;
	
			if (t < min_time) {
				min_time = t;
			}

			if (t > max_time) {
				max_time = t;
			}
		}
		mean_time /= scan_times.size();

		auto total_runtime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - program_start).count();

		std::cout << "\n\n[RASTER TEST SUMMARY]" << std::endl;
		std::cout << "Total scans: " << scan_count << std::endl;
		std::cout << "Min scan time: " << min_time << "s" << std::endl;
		std::cout << "Mean scan time: " << mean_time << "s" << std::endl;
		std::cout << "Max scan time: " << max_time  << "s" << std::endl;
		std::cout << "Total runtime: " << total_runtime << "s" << std::endl;
		std::cout << "Scans per min: " << (scan_count * 60.0 / total_runtime) << std::endl;
	}
};

// Needs to be global since a ton of functions reference it
// Needs to be here since signalHandler needs to hook on to it.
ScanLogger logger;

// NOTE: If the gimbal only moves 1 step per 15 ms then it'd take 5 hours to do a full raster scan
// Because the VL53L8CX have a 45 by 45 field of view it's better to (1) move in 45 deg chunks (400 steps); (2) 4-5 movements instead 
int main() {
	// TEMPORARY FIX: 
	// The code needs to hook onto the pigpio daemon, and I'm tired of having
	// to turn the daemon off every time I reboot the Pi to run the code.
	// Later, when we deploy and make the daemon, this should become easier.
	
	
	// Ultimately the daemon needs to manage everything
	// Since we don't have the daemon, when the pi turns on, the pigpiod daemon makes /var/run/pigpio.pid
	// so when I run this code, it's unable to access the pid since the pigpiod daemon locks it
	
	// So until we make the daemon: We kill the pigpiod daemon in cold blood
	// DAEMON slayers.
	system("sudo killall pigpiod");
	usleep(100000);

	if (gpioInitialise() < 0 || !TMC2209::init_shared_UART("/dev/ttyS0")) {
		std::cerr << "Pigpio or UART failed; shutting down." << std::endl;
		return 1;
	}
	
	// Set Azimuth Stepper to 0 (STEP = 16; DIR = 20, ID=0 (LOW, LOW))
	Azimuth azimuth(16, 20, 0);
	azimuth.configure();
	// Set Altitude Stepper to 0 (STEP = 19, DIR = 26, ID=1 (HIGH, LOW))
	Altitude altitude(19, 26, 1);
	altitude.configure();
	
	const int STEP_US = 1000;

	std::cout << "Beginning chunked LiDAR scan..." << std::endl;

	// Just have it do 20 full scans for test purposes
	int curr_scans = 0;
	// TODO: figure out if needed to multithread with cameras or how to run together
	while (curr_scans < 20) {
		std::cout << "STARTING RASTER CYCLE" << std::endl;
		
		auto scan_start = logger.start_scan();

		for (int chunk = 0; chunk < 3; ++chunk) {
			altitude.full_raster_sweep(STEP_US);

			if (chunk < 2) {
				azimuth.next_chunk(STEP_US);
			}
		}

		azimuth.home(STEP_US);

		// End timing and log duration
		logger.log_scan(scan_start);

		std::cout << "==============> RASTER " << ++curr_scans << " COMPLETE" << std::endl;
	}
	
	logger.print_summary();
	
	gpioTerminate();
	return 0;
}