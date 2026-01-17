#ifndef MPRLS_H
#define MPRLS_H

#include "defines.h"
#include <Wire.h>
#include "Sensor.h"
#include <Adafruit_BMP280.h>


class Pressure : public Sensor
{
  
public:
    /// @brief The adafruit implementation.
    Adafruit_BMP280 pressure;
    /// @brief The I2C address of this pressure sensor.
    const uint8_t address = 0x18;
    /// @brief If the I2C was successful in connecting.
    bool pressureConnected = false;

    Pressure();

    bool connect_to_sensor(int retryDefault = 0) override;    
    void configure_sensor() override;
    String sensor_loop() override;
};


#endif