#include "sensors/MPRLS.h"

bool MPRLS::connect_to_sensor(int retryDefault)
{
    // Connect to the IMU.
    int safetyCount = retryDefault;
    while (!pressure.begin(0x18, &PIMU_WIRE) && safetyCount < RETRY_MAX) 
    {
        SerialUSB.println("Cannot connect to Pressure! Retrying...");
        safetyCount++;
        delay(100);
    }
    
    // Check for IMU
    pressureConnected = safetyCount < RETRY_MAX;
    return pressureConnected;
}

void MPRLS::configure_sensor() 
{
    
}

String MPRLS::sensor_loop() 
{
    if (pressureConnected) 
    {        
        // in hPa
        float pressureReading = pressure.readPressure();
        SerialUSB.printf("Pressure: %f hPa\n");
    }
    else 
    {
        // Try to connect once more while looping to try and salvage the IMU.
        SerialUSB.print("Attempting to connect to pressure in main loop...");
        // Pass in RETRY_MAX - 1 to only try once.
        connect_to_sensor(RETRY_MAX - 1);
    }
    return String("");
}