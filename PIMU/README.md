# PIMU Suite

This project uses Platform.IO for vscode testing and programming. Please place any header files into `include/` and add a header gaurd.
`.cpp` source files go into the `src/` directory.

Example header gaurd:
```cpp
#ifndef HEADER_FILE_NAME_H
#define HEADER_FILE_NAME_H

// code here....

#endif
```

# Teensy IMU Config
- Teensy pin 24 is the I2C clock line (SCL)
- Teensy pin 25 is the I2C data line (SDA)
- Power and ground up to you, but I use G pin next to pin 0 for GND and 3V pin next to pin 23 for VIN.


<img width="1273" height="684" alt="teensy imu pinout" src="https://github.com/user-attachments/assets/2cfdc6de-a39c-47fe-a94a-c5fed835de24" />
Teensy image credit to pjrc.com

IMU image credit to Adafruit

# Teensy Thermocouple Config
Same as above.
