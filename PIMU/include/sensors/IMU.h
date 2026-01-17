#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20649.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "defines.h"

/// @brief The IMU handler.
class IMU 
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
    bool connect_to_imu(int retryDefault = 0);
    
    /// @brief Configure the settings for the IMU.
    void configure_imu();
    
    /// @brief The loop for the IMU to collect data.
    String imu_loop();
    
};

#endif