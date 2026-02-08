#include "logging/ThreeWayLogger.h"

/// The Logger global reference.
ThreeWayLogger LOGGER = ThreeWayLogger();

// Predefine
bool setup_usb_serial();
void setup_ext_uart();

void ThreeWayLogger::begin(DualSD* dual) 
{
    dualSd = dual;

    // Setup both UARTs.
    setup_usb_serial();
    setup_ext_uart();
}

void ThreeWayLogger::printf(const String& s, ...) 
{
    // Create a new span of memory with size of string.
    char* buffer = (char*)malloc(s.length());
    
    // Get the arguments
    va_list list;
    va_start(list, s);
    // 'Print' formatted text into string buffer.
    vsnprintf(buffer, s.length(), s.c_str(), list);
    va_end(list);
    
    // Create a new string object and print a new line.
    String str = String(buffer);
    println(str);
    
    // Free allocated memory.
    free(buffer);
}

void ThreeWayLogger::print(const String &s) 
{
    SerialUSB.print(s);
    EXT_SERIAL.print(s);
    dualSd->writeDebug(s);
}

void ThreeWayLogger::println(const String &s) 
{
    SerialUSB.println(s);
    EXT_SERIAL.println(s);
    dualSd->writeDebugln(s);
}


bool setup_usb_serial() 
{
    // Try to connect to USB Serial.
    SerialUSB.begin(BAUD_RATE_RSX);
    
    // Prevent an infinate loop by setting an upper bound.
    int safetyCount = 0;
    while (!SerialUSB && safetyCount < RETRY_MAX) 
    {
        delay(10);
        safetyCount++;
    }
    
    bool error = safetyCount >= RETRY_MAX;
    return !error;
}

void setup_ext_uart() 
{
    EXT_SERIAL.begin(BAUD_RATE_RSX);
}