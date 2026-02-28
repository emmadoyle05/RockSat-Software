#ifndef Thermocouple_I2C_Control
#define Thermocouple_I2C_Control

String getHotJunction();

String getColdJunction();

String getADC();

String getThermocoupleData();

void initializeI2C1();

void initializeI2C2();

void printHotJunctions();

void printColdJunctions();

void printADCs();

void printAMPnum(int num);

#endif