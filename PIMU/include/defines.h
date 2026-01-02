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

#endif