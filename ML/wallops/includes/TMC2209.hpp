#ifndef TMC2209_HPP
#define TMC2209_HPP

#include <lgpio.h>
#include <stdint.h>
#include <termios.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

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
	static bool init_shared_UART(const char* device = "/dev/ttyAMA0") {
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
};

// Still need to set up the UART so initializing to not set up
int TMC2209::uart_fd = -1;

#endif