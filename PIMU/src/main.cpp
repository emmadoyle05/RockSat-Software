#include "main.h"
#include "defines.h"
#include "sensors/IMU.h"
#include "sensors/Pressure.h"
#include <DualSD.h>

/// If the serial did not connect.
bool faultySerial = false;
/// If the IMU did not connect.
bool faultyIMU = false;

/// The IMU connection.
IMU imu;
Pressure pressure = Pressure();

DualSD dualSd;

void setup()
{
    pinMode(TE_PIN, INPUT);

    // Setup serial.
    if (setup_serial()) 
    {
        // Should be connected.
        SerialUSB.println("Connected to the serial.");
    }
    
    if (pressure.connect_to_sensor()) 
    {
        SerialUSB.println("Pressure sensor connected!");
        pressure.configure_sensor();
    }
    else
    {
        SerialUSB.println("Pressure not connected. Moving on without pressure :(");
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
    
    int connectedCount = dualSd.begin(BUILTIN_SDCARD, EXTERNAL_SD_CS);
    if (connectedCount < 2)
        SerialUSB.printf("ONE OR MORE SD CARDS COULD NOT CONNECT!! Number of SD cards connected: %i.\n", connectedCount);
    dualSd.initializeFiles("seconds_after_power,pressure_Pa,accel_x_m/s/s,accel_y_m/s/s,accel_z_m/s/s,gyro_x_rad/s,gyro_y_rad/s,gyro_z_rad/s,timed_event_detected_bool");
}

void loop() 
{
    String imuStr = imu.imu_loop();
    String pressStr = pressure.sensor_loop();
    
    int te = digitalRead(TE_PIN);
    
    String combinedCsv = String(millis() / 1000.0).append(pressStr).append(imuStr).append(",").append(te);
    dualSd.write(combinedCsv);
    
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
