#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_ICM20649.h>

// Only using default Teensy 4.1 SD this time, sorry!
// Forgot to bring the SPI one home for breadboarding
#include <SD.h>

#define SEALEVEL_PRESSURE_HPA 1013.25
#define DELAY_TIME 1000

File dataStore;

Adafruit_BME280 bme;
Adafruit_ICM20649 icm;
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;

void setup() {
  Serial.begin(9600);
  while(!Serial);

  if (!bme.begin(0x77)) {
    Serial.println("Failed to find BME 280!");
    while (1) { delay(10);}
  }

  if (!icm.begin_I2C()) {
    Serial.println("Failed to find 6-DOF IMU!");
    while (1) { delay(10);}
  }

  if (!SD.begin(BUILTIN_SDCARD)){
    Serial.println("Failed to initialize SD card!");
    while (1) { delay(10); }
  }

  dataStore = SD.open("dataStore.csv", FILE_WRITE);

  if (dataStore){
    dataStore.println("seconds_since_start,temp_c,press_hpa,alt_m,humid_percent,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z");
    dataStore.close();
  } else {
    Serial.println("Failed to open dataStore");
  }

  Serial.println("-- PIMU Breadboard Begin! --");

  Serial.println();
}

void loop() {
  // Requires all three events
  icm.getEvent(&accel, &gyro, &temp);

  float seconds = millis() / 1000.0;

  // Presure, Temperature, Humidity Sensor
  float temp_c = bme.readTemperature();
  float pressure_hpa = bme.readPressure() / 100.0F;
  float altitude_m = bme.readAltitude(SEALEVEL_PRESSURE_HPA);
  float humidity_percent = bme.readHumidity();

  // 6-DOF IMU
  // I wasn't sure whether you all want these to be
  // their own variables or whether you'd like to use
  // the imu directly. Feel free to do any!
  float accel_x = accel.acceleration.x;
  float accel_y = accel.acceleration.y;
  float accel_z = accel.acceleration.z;
  float gyro_x = gyro.gyro.x;
  float gyro_y = gyro.gyro.y;
  float gyro_z = gyro.gyro.z;

  dataStore = SD.open("dataStore.csv", FILE_WRITE);

  // Adding to the CSV; nothing fancy
  // Need to optimize later, just does open and close
  // THIS WILL BE TOO SLOW FOR OUR USE CASE BUT IT'S GOOD ENOUGH
  // FOR BASIC BREADBOARDING
  if (dataStore) {
    dataStore.printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
      seconds, temp_c, pressure_hpa, altitude_m, humidity_percent,
      accel.acceleration.x, accel.acceleration.y, accel.acceleration.z,
      gyro.gyro.x, gyro.gyro.y, gyro.gyro.z);
    // Adding a flush here for safety
    dataStore.flush();
    dataStore.close();
  } else {
    Serial.println("Failed to open dataStore.csv");
  }


  Serial.printf("Temperature = %.2f °C\n", temp_c);
  Serial.printf("Pressure = %.2f hPa\n", pressure_hpa);
  Serial.printf("Altitude (Approx) = %.2f m\n", altitude_m);
  Serial.printf("Humidity = %.2f %\n", humidity_percent);
  Serial.println();
  Serial.printf("IMU Acceleration = %.2f, %.2f, %.2f\n", accel_x, accel_y, accel_z);
  Serial.printf("IMU Gyro = %.2f, %.2f, %.2f\n", gyro_x, gyro_y, gyro_z);


  Serial.println("----------------");

  delay(DELAY_TIME);
}
