#ifndef DEFINES_H
#define DEFINES_H

/// The upper bound of error retries.
#define RETRY_MAX 256

/// The wire to use
#define PIMU_WIRE Wire2

/// The baud rate for USB serial
#define BAUD_RATE_RSX 115200

/// How long to wait in ms for each update loop.
#define LOOP_DELAY 100

/// The external sd card chip select pin.
#define EXTERNAL_SD_CS 10

/// The pin for the timed event.
#define TE_PIN 33

/// The external testing serial.
#define EXT_SERIAL Serial8

/// Extra debugging
//#define PRINT_TO_CONSOLE

#endif