#include "main.h"
#include "defines.h"
#include "sensors/IMU.h"
#include "sensors/Pressure.h"
#include <DualSD.h>
#include <ThreeWayLogger.h>

/// If the serial did not connect.
bool faultySerial = false;
/// If the IMU did not connect.
bool faultyIMU = false;

/// The IMU connection.
IMU imu;
/// The pressure sensor
Pressure pressure = Pressure();
/// The SD manager
DualSD dualSd;

void createCSVHeader();

void setup()
{
    pinMode(TE_PIN, INPUT);

    LOGGER.begin(&dualSd);
    
    // Check SD card
    int connectedCount = dualSd.begin();
    if (connectedCount < 2)
        LOGGER.printf("ONE OR MORE SD CARDS COULD NOT CONNECT!! Number of SD cards connected: %i.\n", connectedCount);

    // This will save the first line as a CSV header.
    createCSVHeader();

    // Setup pressure
    if (pressure.connect_to_sensor()) 
    {
        LOGGER.println("Pressure sensor connected!");
        pressure.configure_sensor();
    }
    else
    {
        LOGGER.println("Pressure not connected. Moving on without pressure :(");
    }
    
    //Setup the IMU.
    if (imu.connect_to_sensor())
    {
        LOGGER.println("IMU connected!!");
        imu.configure_sensor();
    }
    else
    {
        LOGGER.println("IMU not connected. Moving on without IMU :(");
    }
}

void loop() 
{
    // Get the data as CSV strings
    String imuStr = imu.sensor_loop();
    String pressStr = pressure.sensor_loop();
    
    // Check for TE
    int te = digitalRead(TE_PIN);
    
    // Save the data inta a line of CSV
    double timeInSeconds = millis() / 1000.0;
    String combinedCsv = String(timeInSeconds).append(pressStr).append(imuStr).append(",").append(te);
    dualSd.writeln(combinedCsv);
    
    delay(LOOP_DELAY);
}

/// @brief The CSV header string
void createCSVHeader() 
{
    dualSd.initializeFiles("seconds_after_power,pressure_Pa,accel_x_m/s/s,accel_y_m/s/s,accel_z_m/s/s,gyro_x_rad/s,gyro_y_rad/s,gyro_z_rad/s,timed_event_detected_bool");
}