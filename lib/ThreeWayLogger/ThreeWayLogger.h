#ifndef THREEWAYLOGGER_H
#define THREEWAYLOGGER_H

#include <Arduino.h>
#include <DualSD.h>

/// @brief A utility to log to serial, a debug file, and the external UART in one call.
class ThreeWayLogger 
{
public:
    /// The SD cards to write to
    DualSD* dualSd;

    /// @brief Print a formatted string
    /// @param s the string
    /// @param ... The params to be formatted
    void printf(const String& s, ...);
    
    /// @brief Print to log.
    void print(const String &s);
    
    /// @brief Print a line to log.
    void println(const String &s);
    
    /// @brief Print a line to log.
    void println(const Printable &p);
    
    /// @brief Print to log.
    void print(const Printable &p);
    
    /// @brief Start the logger
    void begin(DualSD* dual);
};

/// @brief A hacky way to make a global var lol.
extern ThreeWayLogger LOGGER;

#endif