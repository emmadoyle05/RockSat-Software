#include "Stepper.hpp"
#include "ScanLogger.hpp"
#include <lgpio.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>
#include <sys/select.h>
#include <sys/time.h>

Stepper* g_azimuth = nullptr;
Stepper* g_altitude = nullptr;
int g_gpio_handle = -1;
ScanLogger* g_logger = nullptr;
const int STEP_US = 1000;

void signal_handler(int sig) {
    std::cout << "\n\n!!!!!!!!! EMERGENCY STOP !!!!!!!!!!!" << std::endl;
    std::cout << "Homing..." << std::endl;
    
    if (g_azimuth) {
        g_azimuth->home(STEP_US);
    }
    if (g_altitude) {
        g_altitude->home(STEP_US);
    }

    std::cout << "Shutting down" << std::endl;

    if (g_gpio_handle > 0) {
        lgGpiochipClose(g_gpio_handle);
    }
    if (g_logger) {
        g_logger->print_summary();
    }
    exit(0);
}

int main() {
    // Handles programming terminating unexpctedly
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Bus used by lgpio
    int h = lgGpiochipOpen(0);
	if (h < 0) {
        std::cerr << "GPIO failed" << std::endl;
        return 1;
    }

    g_gpio_handle = h;

	if (!TMC2209::init_shared_UART("/dev/ttyAMA0")) {
		std::cerr << "UART failed; shutting down" << std::endl;
        lgGpiochipClose(h);
		return 1;
	}
	
    const double AZ_DEGREES = 45.0;
    // STEP=27, DIR=17, ID=0, STEPS=400/45
	Stepper azimuth(h, 27, 17, 0, 400.0/45, AZ_DEGREES);
    azimuth.configure();
    g_azimuth = &azimuth;


    const double ALT_DEGREES = 25.0;
    // STEP=11, DIR=22, STEPS=222/25
    Stepper altitude(h, 11, 22, 1, 222.0/25, ALT_DEGREES);
    altitude.configure();
    g_altitude = &altitude;
    
	
    ScanLogger logger;
    logger.log("Wide Angle LiDAR Laser Operating In Space beginning scan!");
    logger.log("Azimuth set to +/-" + std::to_string(AZ_DEGREES) + " deg");
    logger.log("Altitude set to +/-" + std::to_string(ALT_DEGREES) + " deg");
    g_logger = &logger;
	const int STEP_US = 1000;

	std::cout << "Begining Raster Scan!" << std::endl;
    std::cout << "(Press ENTER to stop.)"  << std::endl;

	int curr_scans = 0;

    // Setting up the system to check if ENTER pressed
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    fd_set readfds;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    // Scan continues indefinitely until ENTER is pressed
    // Check is at end so the raster scan isn't interrupted
	while (true) {		
		auto scan_start = logger.start_scan();

        // Azimuth moves to left
        azimuth.move_degrees(-AZ_DEGREES, STEP_US);
        altitude.raster_scan(STEP_US);

        // Azimuth returns to center
        azimuth.move_degrees(AZ_DEGREES, STEP_US);
        altitude.raster_scan(STEP_US);

        // Azimuth moves to right
        azimuth.move_degrees(AZ_DEGREES, STEP_US);
        altitude.raster_scan(STEP_US);

        // Azimuth moves back to the center
        azimuth.move_degrees(-AZ_DEGREES, STEP_US);
        altitude.raster_scan(STEP_US);

		// End timing and log duration
		logger.log_scan(scan_start);

		std::cout << "==============> RASTER " << ++curr_scans << " COMPLETE" << std::endl;

        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        int activity = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

        if (activity > 0 && FD_ISSET(STDIN_FILENO, &readfds)) {
            char ch;
            read(STDIN_FILENO, &ch, 1);
            if (ch=='\n' || ch == '\r'){
                std::cout << "\nEnter pressed - stopping" << std::endl;
                break;
            }
        }
	}

    std::cout << "\nShutting down..." << std::endl;

    azimuth.home(STEP_US);
    altitude.home(STEP_US);
	
	logger.print_summary();

    // Restores terminal; done waiting for enter
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	lgGpiochipClose(h);
	return 0;
}