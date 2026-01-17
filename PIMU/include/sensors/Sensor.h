#ifndef SENSOR_H
#define SENSOR_H


class Sensor 
{
public:
    /// @brief Connect to the sensor.
    /// @param retryDefault The number of tries to start at before reaching the max.
    /// @return True of connected.
    virtual bool connect_to_sensor(int retryDefault = 0) = 0;
    
    /// @brief Configure the settings and accuracy of the sensor.
    virtual void configure_sensor() = 0;
    
    /// @brief The loop function for handling logic.
    virtual String sensor_loop() = 0;
};


#endif