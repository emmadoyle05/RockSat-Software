#include "sensors/IMU.h"
#include "logging/ThreeWayLogger.h"

String IMU::sensor_loop() 
{
    if (imuConnected)
    {        
        // == Credit: dsp-mark (https://github.com/dsp-mark) ==
        // Get the IMU data
        imu.getEvent(&acceleration, &gyro, &temp);
        
        float seconds = millis() / 1000.0;

        // 6-DOF IMU
        float accel_x = acceleration.acceleration.x;
        float accel_y = acceleration.acceleration.y;
        float accel_z = acceleration.acceleration.z;
        float gyro_x = gyro.gyro.x;
        float gyro_y = gyro.gyro.y;
        float gyro_z = gyro.gyro.z;

#ifdef PRINT_TO_CONSOLE
        LOGGER.printf("IMU Acceleration = %.2f, %.2f, %.2f m/s/s\n", accel_x, accel_y, accel_z);
        LOGGER.printf("IMU Gyro = %.2f, %.2f, %.2f m/s/s\n", gyro_x, gyro_y, gyro_z);
        LOGGER.println(seconds);
#endif
        // == End Credit. ==
        
        char buffer[200];
        snprintf(buffer, 200, ",%f,%f,%f,%f,%f,%f", accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z);
        return String(buffer);
    }
    else 
    {
        // Try to connect once more while looping to try and salvage the IMU.
        LOGGER.print("Attempting to connect to IMU in main loop...");
        // Pass in RETRY_MAX - 1 to only try once.
        connect_to_sensor(RETRY_MAX - 1);
        
        // TODO: set imuConnected to false if I2C no longer connects.
    }
    
    // No data
    return ",,,,,,";
}

/// Try and connect to the IMU.
/// @param retryDefault This is the default value for the initial retry count.
/// @return True if connected.
bool IMU::connect_to_sensor(int retryDefault) 
{
    // Connect to the IMU.
    int safetyCount = retryDefault;
    while (!imu.begin_I2C(0x68, &PIMU_WIRE) && safetyCount < RETRY_MAX) 
    {
        LOGGER.println("Cannot connect to IMU! Retrying...");
        safetyCount++;
        delay(100);
    }
    
    // Check for IMU
    imuConnected = safetyCount < RETRY_MAX;
    return imuConnected;
}

/// @brief Configure the IMU settings.
void IMU::configure_sensor() 
{
    // Measure up to 30 Gs
    imu.setAccelRange(ICM20649_ACCEL_RANGE_30_G);
    // 500 degrees per secoond.
    imu.setGyroRange(ICM20649_GYRO_RANGE_500_DPS);
    
    // == Copied from Adafruit ==
    imu.setAccelRateDivisor(4095);
    uint16_t accel_divisor = imu.getAccelRateDivisor();
    float accel_rate = 1125 / (1.0 + accel_divisor);

    LOGGER.print("Accelerometer data rate divisor set to: ");
    LOGGER.println(accel_divisor);
    LOGGER.print("Accelerometer data rate (Hz) is approximately: ");
    LOGGER.println(accel_rate);

    //  icm.setGyroRateDivisor(255);
    uint8_t gyro_divisor = imu.getGyroRateDivisor();
    float gyro_rate = 1100 / (1.0 + gyro_divisor);

    LOGGER.print("Gyro data rate divisor set to: ");
    LOGGER.println(gyro_divisor);
    LOGGER.print("Gyro data rate (Hz) is approximately: ");
    LOGGER.println(gyro_rate);
    // == End copied from adafruit. ==
}