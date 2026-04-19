#ifndef STEPPER_HPP
#define STEPPER_HPP

#include "TMC2209.hpp"
#include <lgpio.h>
#include <cmath>
#include <iostream>

// Azimuth and Altitude now both use Stepper class
class Stepper {
public:
    int gpio_handle;
	int step_pin, dir_pin;
	TMC2209 tmc;
    int steps_per_unit;
    double default_degrees;
    double current_position_deg;
	

	Stepper(int h, int step, int dir, uint8_t tmc_addr, double steps_per_degree, double default_movement=45.0) : gpio_handle(h), step_pin(step), dir_pin(dir), tmc(tmc_addr), steps_per_unit(steps_per_degree), default_degrees(default_movement), current_position_deg(0.0) {
        lgGpioClaimOutput(gpio_handle, 0, step_pin, 0);
        lgGpioClaimOutput(gpio_handle, 0, dir_pin, 0);
        lgGpioWrite(gpio_handle, step_pin, 0);
	}
	
	void configure() {
		// Steppers always use a 600mA, 1/16 microstep TMC2209 set up
		tmc.configure_stepper(600, 16);
	}
	
	// Move a single step in a given direction
	void move(int dir, int pulse_us = 1000) {
		lgGpioWrite(gpio_handle, dir_pin, dir > 0 ? 1 : 0);
        lgGpioWrite(gpio_handle, step_pin, 1);
        lguSleep(0.000001 * pulse_us);
        lgGpioWrite(gpio_handle, step_pin, 0);
        lguSleep(0.000001 * pulse_us); 
	}

	// Move n steps in a given direction
	void move_steps(int steps, int pulse_us = 1000) {
		// Use the sign of steps to move clockwise (1) or counterclockwise (-1)
		int dir = (steps >= 0) ? 1 : -1;
		int count = std::abs(steps);

		for (int i = 0; i < count; ++i){
			move(dir, pulse_us);
		}
        current_position_deg += (dir * 1.0 * count) / steps_per_unit;
	}

    void move_degrees(double degrees, int pulse_us = 1000) {
        double steps = degrees * steps_per_unit;

        std::cout << "Moving: - " << current_position_deg << " to " << default_degrees << std::endl;

        move_steps(static_cast<int>(steps), pulse_us);
    }

    void raster_scan(int pulse_us = 1000) {

        move_degrees(-default_degrees, pulse_us);

        lguSleep(0.1);

        move_degrees(+default_degrees, pulse_us);

        lguSleep(0.1);

        move_degrees(+default_degrees, pulse_us);

        lguSleep(0.1);

        move_degrees(-default_degrees, pulse_us);
    }

    void home(int pulse_us = 10000) {
        std::cout << "Returning to home from " << current_position_deg << " to 0 " << std::endl;
        move_degrees(-current_position_deg, pulse_us);
        current_position_deg = 0.0;
    }

    double get_position() {
        return current_position_deg;
    }
};

#endif