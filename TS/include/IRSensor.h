// ==== Modified for COC RSX ====
// ==== Original Code Author Credit ====
// MLX90641_processing.ino file for the MLX90641.h library, version 1.0.4
// Description: Outputs the ambient temperature + pixels all in one line
// to the Serial Monitor. Works in conjunction with the following sketch:
// https://github.com/dndubins/MLX90641/blob/main/extras/MLX90641_heatmap.pde
// which generates a colour heat map for the MLX90641, using Processing
// (available at https://processing.org/)
// Author: D. Dubins
// Lots of help from: ChatGPT 3.0, Perplexity.AI
// Date: 17-Dec-25
// Notes: the MLX90641 operating voltage is 3-3.6V (typical: 3.3V).
// Use a logic shifter, or connect to an MCU that operates at 3.3V (e.g. NodeMCU).
// After the device powers up and sends data, a thermal stabilization time is required
// before the device can reach the specified accuracy (up to 3 min) - 12.2.2
// Wiring: ("/\" is notch in device case, pins facing you)
//
//       _____/\______
//     /              \
//    /  4:SCL  1:SDA  \
//   |                  |
//   |                  |
//    \  3:GND  2:3.3V /
//     \______________/
//
// ESP32 - MLX90641:
// --------------------------------------
// SDA - D21 (GPIO21) - SDA
// SCL - D22 (GPIO22) - SCL
// GND -  GND
// 3.3V - VDD
//
// MLX90641 refresh rates (Control register 0x800D bits 10:7):
// -----------------------------------------------------------
// Bit    Freq      Sec/frame          POR Delay (ms)  Sample Every (ms)
// 0x00 = 0.5 Hz    2 sec              4080 ms         2400 ms
// 0x01 = 1 Hz      1 sec/frame        2080 ms         1200 ms
// 0x02 = 2 Hz      0.5 sec/frame      1080 ms         600 ms (default)
// 0x03 = 4 Hz      0.25 sec/frame     580 ms          300 ms
// 0x04 = 8 Hz      0.125 sec/frame    330 ms          150 ms
// 0x05 = 16 Hz     0.0625 sec/frame   205 ms           75 ms
// 0x06 = 32 Hz     0.03125 sec/frame  143 ms           38 ms
// 0x07 = 64 Hz     0.015625 sec/frame 112 ms           19 ms
#ifndef IRSENSOR_H
#define IRSENSOR_H

#include <Wire.h>
#include "MLX90641.h"

//#define DEBUG                             // show calculated and example values for calibration constants
#define OFFSET 0.0                          // posthoc cheap temperature adjustment (shift)
#define I2C_SPEED 100000                    // set I2C clock speed (safe speed is 100 kHz, up to 400 kHz possible)
#define REFRESH_RATE 0x03                   // 0x00 (0.5 Hz) to 0x07 (64 Hz). Default: 0x03 (4 Hz)
#define SAMPLE_DELAY 300                    // delay between reading samples (see refresh rate table)
#define POR_DELAY SAMPLE_DELAY * 2.0 * 1.2  // delay required after power on reset (see refresh rate table)
#define CAL_INT -45.4209807273067           // Intercept of T_meas vs. T_o calibration curve (post-hoc calibration). My value: -45.4209807273067
#define CAL_SLOPE 2.64896693658985          // Slope of T_meas vs. T_o calibration curve (post-hoc calibration). My value: 2.64896693658985
#define IR_WIRE Wire

class IRSensor 
{

public:
    MLX90641 myIRcam;  // declare an instance of class MLX90641
    void begin();
    void initialize();
    String ir_loop();
            
};


#endif