#include "sensors/Thermocouple.h"

Thermocouple::Thermocouple(uint8_t address) 
    : address(address)
{
    
}

bool Thermocouple::connect_to_thermo(int retryDefault) 
{
    // Connect to the IMU.
    int safetyCount = retryDefault;
    while (!thermo.begin(address, &PIMU_WIRE) && safetyCount < RETRY_MAX) 
    {
        SerialUSB.print("Cannot connect to IMU! Retrying...");
        safetyCount++;
        delay(100);
    }
    
    // Check for IMU
    thermoConnected = safetyCount < RETRY_MAX;
    return thermoConnected;
}

void Thermocouple::configure_thermo() 
{
    thermo.setAmbientResolution(RES_ZERO_POINT_25);
    thermo.setADCresolution(MCP9600_ADCRESOLUTION_18);
    thermo.setThermocoupleType(MCP9600_TYPE_N);
    thermo.setFilterCoefficient(3);
    thermo.enable(true);
}

void Thermocouple::thermo_loop() 
{
    if (thermoConnected) 
    {        
        float seconds = millis() / 1000.0;

        SerialUSB.print("Hot junction: ");
        SerialUSB.print(thermo.readThermocouple());
        SerialUSB.println("deg C");
        
        SerialUSB.print("Cold junction: ");
        SerialUSB.print(thermo.readAmbient());
        SerialUSB.println("deg C");
        
        SerialUSB.print("ADC: "); 
        SerialUSB.print(thermo.readADC() * 2); 
        SerialUSB.println(" uV");
    }
    else 
    {
        // Try to connect once more while looping to try and salvage the IMU.
        SerialUSB.printf("Attempting to connect to thermocouple with address %d in main loop...\n", address);
        // Pass in RETRY_MAX - 1 to only try once.
        connect_to_thermo(RETRY_MAX - 1);
        
        // TODO: set imuConnected to false if I2C no longer connects.
    }
}