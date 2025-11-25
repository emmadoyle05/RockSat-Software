#ifndef THERMOCOUPLE_H
#define THERMOCOUPLE_H


#include "defines.h"
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_MCP9600.h>

/// @brief The thermocouple sensor wrapper.
class Thermocouple 
{
public:
    /// @brief The adafruit implementation.
    Adafruit_MCP9600 thermo;
    /// @brief The I2C address of this thermocouple.
    uint8_t address = 0;
    /// @brief If the I2C was successful in connecting.
    bool thermoConnected = false;
    
    /// @brief Create a new thermocouple instance.
    /// @param address The I2C address this thermocouple was set to.
    Thermocouple(uint8_t address = 0x67);
    
    /// @brief Connect to the thermocouple amp board.
    /// @param retryDefault The number of tries to start at before reaching the max.
    /// @return True of connected.
    bool connect_to_thermo(int retryDefault = 0);
    
    /// @brief Configure the settings and accuracy of the thermocouple.
    void configure_thermo();
    
    /// @brief The loop function for handling logic.
    void thermo_loop();
};

#endif