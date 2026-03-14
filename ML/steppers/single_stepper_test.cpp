#include <pigpio.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <iostream>

// Runs on the Pi; requires the installation of pigpio

// g++ -o stepper single_stepper_test.cpp -lpigpio -lrt
// sudo ./stepper

// Initial test for getting NEMA 17 stepper working with TMC2209 driver; will be refined.
// Will add class for stepper motors

int main(){
	if (gpioInitialise() < 0) {
		return 1;
	}
	
	// Set pinmode for 
	int EN = 21, STEP = 16, DIR = 20;
	
	gpioSetMode(EN, PI_OUTPUT);
	gpioSetMode(STEP, PI_OUTPUT);
	gpioSetMode(DIR, PI_OUTPUT);
	
	// Might need to use EN later; works with it unplugged, though
	// gpioWrite(EN, 1);
	
	const char* uart_device = "/dev/ttyS0";
	int uart_fd = open(uart_device, O_RDWR | O_NOCTTY | O_SYNC);
	if (uart_fd < 0) {
		std::cout << "UART failed." << std::endl;
		gpioTerminate();
		return 1;
	}
	
	struct termios options;
	
	tcgetattr(uart_fd, &options);
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	tcsetattr(uart_fd, TCSANOW, &options);
	
	
	// Write GCONF: enable UART, internal RSENSE
	uint8_t gconf[] = {0x80, 0x00, 0x01, 0x00, 0xF8, 0x1C, 0x00, 0x00};
	write(uart_fd, gconf, 8);
	usleep(10000);
	
	// IHOLD_IRUN: 600mA run current (IHOLD=10, IRUN=24 -> 600mA)
	uint8_t ih_ir[] = {0x80, 0x6C, 0xF9, 0x00, 0x30, 0x1F, 0x00, 0x00};
	//serialPuts(uart_fd, (char*)ih_ir);
	write(uart_fd, ih_ir, 8);
	usleep(10000);
	
	//CHOPCONF: 1/16 microstep TOFF=3
	uint8_t chop[] = {0x80, 0x6c, 0xF9, 0x00, 0x30, 0x1F, 0x00, 0x00};
	//serialPuts(uart_fd, (char*)chop);
	write(uart_fd, chop, 8);
	usleep(10000);
	
	gpioWrite(EN, 0);
	usleep(100000);
	
	// NOTE TO SELF: 90 deg = 800 microsteps (1/16); 180 deg = 1600
	auto move = [&](int steps) {
		// DIR pin tells the stepper whether you want to move clockwise (1) or counterclockwise (0)
		gpioWrite(DIR, steps > 0 ? 1 : 0);
		steps = steps > 0 ? steps : -steps;
		
		for (int i = 0; i < steps; i++) {
			gpioWrite(STEP, 1); 
			usleep(1500);
			gpioWrite(STEP, 0); 
			usleep(1500);
		}
		
		usleep(1000000);
	};
	
	// NOTE TO SELF: +90 deg
	move(800);
	// NOTE TO SELF: -90 deg
	move(-800);
	// NOTE TO SELF: +180 deg
	move(1600);
	// NOTE TO SELF: -180 deg
	move(-1600);

	// Might need to use EN later; works with it unplugged, though
	// gpioWrite(EN, 0);
	close(uart_fd);
	gpioTerminate();
	return 0; 
}
