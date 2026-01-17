#ifndef MPRLS_H
#define MPRLS_H

#include "defines.h"
#include <Wire.h>
#include "Sensor.h"
#include <Adafruit_MPRLS.h>


class MPRLS : public Sensor
{
  
public:
    /// @brief The adafruit implementation.
    Adafruit_MPRLS pressure;
    /// @brief The I2C address of this pressure sensor.
    const uint8_t address = 0x18;
    /// @brief If the I2C was successful in connecting.
    bool pressureConnected = false;

    bool connect_to_sensor(int retryDefault = 0) override;    
    void configure_sensor() override;
    String sensor_loop() override;
};


#endif