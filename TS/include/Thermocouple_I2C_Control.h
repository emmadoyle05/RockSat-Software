#ifndef Thermocouple_I2C_Control
#define Thermocouple_I2C_Control

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_MCP9600.h>
#include <ThreeWayLogger.h>
#include <DualSD.h>

class ThermocoupleControl 
{

public:
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
    
    String getHotJunctionString();
    
    String getColdJunctionString();
};


#endif