#include "sensors/Pressure.h"
#include <ThreeWayLogger.h>

bool Pressure::connect_to_sensor(int retryDefault)
{
    // Connect to the IMU.
    int safetyCount = retryDefault;
    while (!pressure.begin(119U) && safetyCount < RETRY_MAX) 
    {
        LOGGER.println("Cannot connect to Pressure! Retrying...");
        safetyCount++;
        delay(100);
    }
    
    // Check for IMU
    pressureConnected = safetyCount < RETRY_MAX;
    return pressureConnected;
}

void Pressure::configure_sensor() 
{
    // Nothing
}

Pressure::Pressure() 
{
    pressure = Adafruit_BMP280(&PIMU_WIRE);
}

String Pressure::sensor_loop() 
{
    if (pressureConnected) 
    {        
        // in hPa
        float pressureReading = pressure.readPressure();
        
#ifdef PRINT_TO_CONSOLE
        LOGGER.printf("Pressure: %f Pa\n", pressureReading);
#endif

        return String(",").append(pressureReading);
    }
    else 
    {
        // Try to connect once more while looping to try and salvage the IMU.
        LOGGER.print("Attempting to connect to pressure in main loop...");
        // Pass in RETRY_MAX - 1 to only try once.
        connect_to_sensor(RETRY_MAX - 1);
    }
    
    return ",";
}