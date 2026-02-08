#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20649.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "defines.h"
#include "sensors/Sensor.h"

/// @brief The IMU handler.
class IMU : public Sensor
{

public:
    /// If the IMU is connected or not.
    bool imuConnected = false;
    
    /// The IMU connection handled by Adafruit.
    Adafruit_ICM20649 imu;

    /// The accel data.
    sensors_event_t acceleration;
    /// The gyro data.
    sensors_event_t gyro;
    /// The temp data.
    sensors_event_t temp;
    
    /// Try to connect to the IMU
    /// @param retryDefault The starting value for how many attempts to try to connect.
    /// @return True if connected.
    bool connect_to_sensor(int retryDefault = 0) override;
    
    /// @brief Configure the settings for the IMU.
    void configure_sensor() override;
    
    /// @brief The loop for the IMU to collect data.
    /// @returns The CSV string.
    String sensor_loop() override;
    
};

#endif