#include <Arduino.h>
#include "Thermocouple_I2C_Control.h"
#include <ThreeWayLogger.h>

#define TE_PIN 0

#define SCAN_DELAY 1000

//initialize DualSD manager
DualSD DualSDManager;

void createCSVHeader();

void setup(){

  Serial.begin(115000);

  LOGGER.begin(&DualSDManager);

  int connectedCount = DualSDManager.begin();

  if (connectedCount < 2){
    LOGGER.printf("ONE OR MORE SD CARDS COULD NOT CONNECT!! Number of SD cards connected: %i.\n", connectedCount);
  }

  createCSVHeader();

  Serial.println("Serial Established");

  initializeI2C1();

  Serial.println("Done with I2C1");

  initializeI2C2();

  Serial.println("Done with I2C2");
}

void loop(){
  
  Serial.println("Getting thermocouple Data");
  //gather thermocouple data:
  String thermocoupleData = getThermocoupleData();

  Serial.println("Getting TE Signal");
  //check if TE signal is active:
  int TE = digitalRead(TE_PIN);

  Serial.println("Saving Data");
  //Save Data:
  double timeInSeconds = millis()/1000.0;
  String combinedCSV = String(timeInSeconds).append(",").append(thermocoupleData).append(TE);
  DualSDManager.writeln(combinedCSV);

  delay(SCAN_DELAY);
}

void createCSVHeader(){
  Serial.println("Creating CSV Header");
  DualSDManager.initializeFiles("seconds_after_power,hot_junction_1,hot_junction_2,hot_junction_3,hot_junction_4,hot_junction_5,cold_junction_1,cold_junction_2,cold_junction_3,cold_junction_4,cold_junction_5,ADC_1,ADC_2,ADC_3,ADC_4,ADC_5,timed_event_detected_bool");
}



