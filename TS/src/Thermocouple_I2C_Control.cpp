#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_MCP9600.h>
#include <ThreeWayLogger.h>
#include <DualSD.h>

//Amplifiers on I2C Bus 1:
Adafruit_MCP9600 Amp1;
Adafruit_MCP9600 Amp2;
Adafruit_MCP9600 Amp3;

const uint8_t Address1 = 0x66; //J1
const uint8_t Address2 = 0x65; //J2
const uint8_t Address3 = 0x65; //No Jump

//Amplifiers on I2C Bus 2:
Adafruit_MCP9600 Amp4;
Adafruit_MCP9600 Amp5;

const uint8_t Address4 = 0x67; //No jump
const uint8_t Address5 = 0x65; //J2

bool ADDR_ONE_CHECK = false;
bool ADDR_TWO_CHECK = false;
bool ADDR_THREE_CHECK = false;
bool ADDR_FOUR_CHECK = false;
bool ADDR_FIVE_CHECK = false;


void initializeI2C1(){

  Serial.println("Initializing I2C_1");
  LOGGER.println("Initializing I2C_1");

  Wire1.begin();

  int debounce = 0;

  while (!Amp1.begin(Address1, &Wire1) && debounce < 5){
    debounce++;
    Serial.println("Amplifier 1 not found!");
    LOGGER.println("Amplifier 1 not found!");
    delay(500);
  }

    debounce = 0;

  if (Amp1.begin(Address1, &Wire1)){
    ADDR_ONE_CHECK = true;
    Amp1.setADCresolution(MCP9600_ADCRESOLUTION_18);
    Amp1.setThermocoupleType(MCP9600_TYPE_N);
  }

  while (!Amp2.begin(Address2, &Wire1) && debounce < 5){
    debounce++;
    Serial.println("Amplifier 2 not found!");
    delay(500);
  }

  debounce = 0;

  if (Amp2.begin(Address2, &Wire1)){
    ADDR_TWO_CHECK = true;
    Amp2.setADCresolution(MCP9600_ADCRESOLUTION_18);
    Amp2.setThermocoupleType(MCP9600_TYPE_N);
  }

  while (!Amp3.begin(Address3, &Wire1) && debounce < 5){
    debounce++;
    Serial.println("Amplifier 3 not found!");
    delay(500);
  }

  debounce = 0;

  if (Amp3.begin(Address3, &Wire1)){
    ADDR_THREE_CHECK = true;
    Amp3.setADCresolution(MCP9600_ADCRESOLUTION_18);
    Amp3.setThermocoupleType(MCP9600_TYPE_N);
  }

  Serial.println("All amplifiers found!");
  Serial.println("I2C_1 Established");
  LOGGER.printf("%0f : I2C_1 Initialized", millis() / 1000);
}

void initializeI2C2(){

  int debounce = 0;

  Serial.println("Initializing I2C_2");
  
  Wire2.begin();

  while (!Amp4.begin(Address4, &Wire2) && debounce < 5){
    Serial.println("Amplifier 4 not found");
    delay(500);
  }

  if (Amp4.begin(Address4, &Wire2)){
    ADDR_FOUR_CHECK = true;
    Amp4.setADCresolution(MCP9600_ADCRESOLUTION_18);
    Amp4.setThermocoupleType(MCP9600_TYPE_N);
  }

  while (!Amp5.begin(Address5, &Wire2) && debounce < 5){
    Serial.println("Amplifier 5 not found!");
    delay(500);  
  }

  if (Amp5.begin(Address5, &Wire2)){
    ADDR_FIVE_CHECK = true;
    Amp5.setADCresolution(MCP9600_ADCRESOLUTION_18);
    Amp5.setThermocoupleType(MCP9600_TYPE_N);
  }

  Serial.println("All amplifiers found!");
  Serial.println("I2C_2 Established");
  LOGGER.printf("%0f : I2C_2 Initialized", millis() / 1000);
}

String getHotJunctionString(){
  String hotJunctionString;

  hotJunctionString.append(Amp1.readThermocouple());
  hotJunctionString.append(",");
  hotJunctionString.append(Amp2.readThermocouple());
  hotJunctionString.append(",");
  hotJunctionString.append(Amp3.readThermocouple());
  hotJunctionString.append(",");
  hotJunctionString.append(Amp4.readThermocouple());
  hotJunctionString.append(",");
  hotJunctionString.append(Amp5.readThermocouple());
  hotJunctionString.append(",");

  return hotJunctionString;
}

void printHotJunctions(){
  Serial.println("Hot Junctions:");

  //I2C Bus 1:
  Serial.printf("One: %0.2f\n", Amp1.readThermocouple());
  Serial.printf("Two: %0.2f\n", Amp2.readThermocouple());
  Serial.printf("Three: %0.2f\n", Amp3.readThermocouple());
  //I2C Bus 2:
  Serial.printf("Four: %0.2f\n", Amp4.readThermocouple());
  Serial.printf("Five: %0.2f\n", Amp5.readThermocouple());
}

String getColdJunctionString(){
  String coldJunctionString;

  coldJunctionString.append(Amp1.readAmbient());
  coldJunctionString.append(",");
  coldJunctionString.append(Amp2.readAmbient());
  coldJunctionString.append(",");
  coldJunctionString.append(Amp3.readAmbient());
  coldJunctionString.append(",");
  coldJunctionString.append(Amp4.readAmbient());
  coldJunctionString.append(",");
  coldJunctionString.append(Amp5.readAmbient());
  coldJunctionString.append(",");

  return coldJunctionString;
}

void printColdJunctions(){
  Serial.println("Cold Junctions:");

  //I2C Bus 1:
  Serial.printf("One: %0.2f\n", Amp1.readAmbient());
  Serial.printf("Two: %0.2f\n", Amp2.readAmbient());
  Serial.printf("Three: %0.2f\n", Amp3.readAmbient());
  //I2C Bus 2:
  Serial.printf("Four: %0.2f\n", Amp4.readAmbient());
  Serial.printf("Five: %0.2f\n", Amp5.readAmbient());
}

String getADC(){
  String ADC;

  ADC.append(Amp1.readADC());
  ADC.append(",");
  ADC.append(Amp2.readADC());
  ADC.append(",");
  ADC.append(Amp3.readADC());
  ADC.append(",");
  ADC.append(Amp4.readADC());
  ADC.append(",");
  ADC.append(Amp5.readADC());
  ADC.append(",");

  return ADC;
}

void printADCs(){
  Serial.println("ADCs:");

  //I2C Bus 1:
  Serial.printf("One: %0.2f\n", Amp1.readADC());
  Serial.printf("Two: %0.2f\n", Amp2.readADC());
  Serial.printf("Three: %0.2f\n", Amp3.readADC());
  
  //I2C Bus 2:
  Serial.printf("Four: %0.2f\n", Amp4.readADC());
  Serial.printf("Five: %0.2f\n", Amp5.readADC());
}

void printAMPnum(int num){
  switch (num)
  {
  case 1:
    Serial.printf("Hot Junction One: %0.2f\n", Amp1.readThermocouple());
    Serial.printf("Cold Junction One: %0.2f\n", Amp1.readAmbient());
    Serial.printf("ADC One: %0.2f\n", Amp1.readADC());
    break;
  case 2:
    Serial.printf("Hot Junction One: %0.2f\n", Amp2.readThermocouple());
    Serial.printf("Cold Junction Two: %0.2f\n", Amp2.readAmbient());
    Serial.printf("ADC Two: %0.2f\n", Amp2.readADC());
    break;
  case 3:
    Serial.printf("Hot Junction Three: %0.2f\n", Amp3.readThermocouple());
    Serial.printf("Cold Junction Three: %0.2f\n", Amp3.readAmbient());
    Serial.printf("ADC Three: %0.2f\n", Amp3.readADC());
    break;
  case 4:
    Serial.printf("Hot Junction Four: %0.2f\n", Amp4.readThermocouple());
    Serial.printf("Cold Junction Four: %0.2f\n", Amp4.readAmbient());
    Serial.printf("ADC Four: %0.2f\n", Amp4.readADC());
    break;
  case 5:
    Serial.printf("Hot Junction Five: %0.2f\n", Amp5.readThermocouple());
    Serial.printf("Cold Junction Five: %0.2f\n", Amp5.readAmbient());
    Serial.printf("ADC Five: %0.2f\n", Amp5.readADC());
    break;
  default:
    Serial.println("Invalid thermocouple selected");
    break;
  }
}

String getThermocoupleData(){
  return getHotJunctionString().append(getColdJunctionString()).append(getADC());
}

