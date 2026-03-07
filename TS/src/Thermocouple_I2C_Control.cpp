#include "Thermocouple_I2C_Control.h"

#define RETRY_MAX 256
// In ms
#define RETRY_DELAY 100 

void ThermocoupleControl::initializeI2C1(){

  //Serial.println("Initializing I2C_1");
  LOGGER.println("Initializing I2C_1");

  Wire1.begin();

  int debounce = 0;

  while (!Amp1.begin(Address1, &Wire1) && debounce < RETRY_MAX){
    debounce++;
    //Serial.println("Amplifier 1 not found!");
    LOGGER.println("Amplifier 1 not found!");
    delay(RETRY_DELAY);
  }

  debounce = 0;

  if (Amp1.begin(Address1, &Wire1)){
    ADDR_ONE_CHECK = true;
    Amp1.setADCresolution(MCP9600_ADCRESOLUTION_18);
    Amp1.setThermocoupleType(MCP9600_TYPE_N);
  }

  while (!Amp2.begin(Address2, &Wire1) && debounce < RETRY_MAX){
    debounce++;
    LOGGER.println("Amplifier 2 not found!");
    delay(RETRY_DELAY);
  }

  debounce = 0;

  if (Amp2.begin(Address2, &Wire1)){
    ADDR_TWO_CHECK = true;
    Amp2.setADCresolution(MCP9600_ADCRESOLUTION_18);
    Amp2.setThermocoupleType(MCP9600_TYPE_N);
  }

  while (!Amp3.begin(Address3, &Wire1) && debounce < RETRY_MAX){
    debounce++;
    LOGGER.println("Amplifier 3 not found!");
    delay(RETRY_DELAY);
  }

  debounce = 0;

  if (Amp3.begin(Address3, &Wire1)){
    ADDR_THREE_CHECK = true;
    Amp3.setADCresolution(MCP9600_ADCRESOLUTION_18);
    Amp3.setThermocoupleType(MCP9600_TYPE_N);
  }

  LOGGER.println("All amplifiers found!");
  LOGGER.println("I2C_1 Established");
  LOGGER.printf("%0f : I2C_1 Initialized", millis() / 1000.0);
}

void ThermocoupleControl::initializeI2C2(){

  int debounce = 0;

  LOGGER.println("Initializing I2C_2");
  
  Wire2.begin();

  while (!Amp4.begin(Address4, &Wire2) && debounce < RETRY_MAX){
    LOGGER.println("Amplifier 4 not found");
    delay(RETRY_MAX);
  }

  if (Amp4.begin(Address4, &Wire2)){
    ADDR_FOUR_CHECK = true;
    Amp4.setADCresolution(MCP9600_ADCRESOLUTION_18);
    Amp4.setThermocoupleType(MCP9600_TYPE_N);
  }

  while (!Amp5.begin(Address5, &Wire2) && debounce < RETRY_MAX){
    LOGGER.println("Amplifier 5 not found!");
    delay(RETRY_DELAY);  
  }

  if (Amp5.begin(Address5, &Wire2)){
    ADDR_FIVE_CHECK = true;
    Amp5.setADCresolution(MCP9600_ADCRESOLUTION_18);
    Amp5.setThermocoupleType(MCP9600_TYPE_N);
  }

  LOGGER.println("All amplifiers found!");
  LOGGER.println("I2C_2 Established");
  LOGGER.printf("%0f : I2C_2 Initialized", millis() / 1000.0);
}

String ThermocoupleControl::getHotJunctionString(){
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

void ThermocoupleControl::printHotJunctions(){
  LOGGER.println("Hot Junctions:");

  //I2C Bus 1:
  LOGGER.printf("One: %0.2f\n", Amp1.readThermocouple());
  LOGGER.printf("Two: %0.2f\n", Amp2.readThermocouple());
  LOGGER.printf("Three: %0.2f\n", Amp3.readThermocouple());
  //I2C Bus 2:
  LOGGER.printf("Four: %0.2f\n", Amp4.readThermocouple());
  LOGGER.printf("Five: %0.2f\n", Amp5.readThermocouple());
}

String ThermocoupleControl::getColdJunctionString(){
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

void ThermocoupleControl::printColdJunctions(){
  LOGGER.println("Cold Junctions:");

  //I2C Bus 1:
  LOGGER.printf("One: %0.2f\n", Amp1.readAmbient());
  LOGGER.printf("Two: %0.2f\n", Amp2.readAmbient());
  LOGGER.printf("Three: %0.2f\n", Amp3.readAmbient());
  //I2C Bus 2:
  LOGGER.printf("Four: %0.2f\n", Amp4.readAmbient());
  LOGGER.printf("Five: %0.2f\n", Amp5.readAmbient());
}

String ThermocoupleControl::getADC(){
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

void ThermocoupleControl::printADCs(){
  LOGGER.println("ADCs:");

  //I2C Bus 1:
  LOGGER.printf("One: %0.2f\n", Amp1.readADC());
  LOGGER.printf("Two: %0.2f\n", Amp2.readADC());
  LOGGER.printf("Three: %0.2f\n", Amp3.readADC());
  
  //I2C Bus 2:
  LOGGER.printf("Four: %0.2f\n", Amp4.readADC());
  LOGGER.printf("Five: %0.2f\n", Amp5.readADC());
}

void ThermocoupleControl::printAMPnum(int num){
  switch (num)
  {
  case 1:
    LOGGER.printf("Hot Junction One: %0.2f\n", Amp1.readThermocouple());
    LOGGER.printf("Cold Junction One: %0.2f\n", Amp1.readAmbient());
    LOGGER.printf("ADC One: %0.2f\n", Amp1.readADC());
    break;
  case 2:
    LOGGER.printf("Hot Junction Two: %0.2f\n", Amp2.readThermocouple());
    LOGGER.printf("Cold Junction Two: %0.2f\n", Amp2.readAmbient());
    LOGGER.printf("ADC Two: %0.2f\n", Amp2.readADC());
    break;
  case 3:
    LOGGER.printf("Hot Junction Three: %0.2f\n", Amp3.readThermocouple());
    LOGGER.printf("Cold Junction Three: %0.2f\n", Amp3.readAmbient());
    LOGGER.printf("ADC Three: %0.2f\n", Amp3.readADC());
    break;
  case 4:
    LOGGER.printf("Hot Junction Four: %0.2f\n", Amp4.readThermocouple());
    LOGGER.printf("Cold Junction Four: %0.2f\n", Amp4.readAmbient());
    LOGGER.printf("ADC Four: %0.2f\n", Amp4.readADC());
    break;
  case 5:
    LOGGER.printf("Hot Junction Five: %0.2f\n", Amp5.readThermocouple());
    LOGGER.printf("Cold Junction Five: %0.2f\n", Amp5.readAmbient());
    LOGGER.printf("ADC Five: %0.2f\n", Amp5.readADC());
    break;
  default:
    LOGGER.println("Invalid thermocouple selected");
    break;
  }
}

String ThermocoupleControl::getThermocoupleData(){
  return getHotJunctionString().append(getColdJunctionString()).append(getADC());
}

