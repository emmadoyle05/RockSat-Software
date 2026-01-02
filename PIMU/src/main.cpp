#include "main.h"
#include "defines.h"
#include "sensors/IMU.h"
#include "sensors/Thermocouple.h"

/// If the serial did not connect.
bool faultySerial = false;
/// If the IMU did not connect.
bool faultyIMU = false;

/// The IMU connection.
IMU imu;
Thermocouple thermo1 = Thermocouple(0x67);

void setup() 
{
    // Setup serial.
    if (setup_serial()) 
    {
        // Should be connected.
        SerialUSB.println("Connected to the serial.");
    }
    
    if (thermo1.connect_to_thermo()) 
    {
        SerialUSB.printf("Thermocouple with adress %d connected!\n", thermo1.address);
        thermo1.configure_thermo();
    }
    
    //Setup the IMU.
    if (imu.connect_to_imu())
    {
        SerialUSB.println("IMU connected!!");
        imu.configure_imu();
    }
    else
    {
        SerialUSB.println("IMU not connected. Moving on without IMU :(");
    }
}

void loop() 
{
    imu.imu_loop();
    thermo1.thermo_loop();
    delay(LOOP_DELAY);
}


/// Start the serial connection.
/// @return True if connected.
bool setup_serial() 
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
    
    faultySerial = safetyCount >= RETRY_MAX;
    return !faultySerial;
}
