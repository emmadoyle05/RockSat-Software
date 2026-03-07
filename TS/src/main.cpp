#include <Arduino.h>
#include "Thermocouple_I2C_Control.h"
#include <ThreeWayLogger.h>
#include "Thermocouple_I2C_Control.h"
#include "IRSensor.h"

// THINGS TO CHANGE WHEN NEW PCB ARRIVES:

// The pin used to read the timed event.
#define TE_PIN 0
// How often to loop (in ms)
#define SCAN_DELAY 500
// Used with LOGGER.
#define EXT_SERIAL Serial2

// Initialize DualSD manager
DualSD DualSDManager;
// Initialize the thermocouples.
ThermocoupleControl thermocoupleController;
// Initialize the IR sensor
IRSensor irSensor;

// Quickly create the CSV header and write to SD
void createCSVHeader();
// Set all pin modes.
void setupPins();

void setup(){
	Serial.begin(115000);

	LOGGER.begin(&DualSDManager);

	// Setup all digital and analog pins.
	setupPins();

	// Check how many SD cards connected.
	int connectedCount = DualSDManager.begin();
	if (connectedCount < 2){
		LOGGER.printf("ONE OR MORE SD CARDS COULD NOT CONNECT!! Number of SD cards connected: %i.\n", connectedCount);
	}

	createCSVHeader();

	LOGGER.println("Serial Established");

	thermocoupleController.initializeI2C1();

	LOGGER.println("Done with I2C1");

	thermocoupleController.initializeI2C2();

	LOGGER.println("Done with I2C2");

	irSensor.begin();
	LOGGER.println("Started IR.");
}

void loop(){
	LOGGER.println("Getting thermocouple Data");
	//gather thermocouple data:
	String thermocoupleData = thermocoupleController.getThermocoupleData();

	LOGGER.println("Getting TE Signal");
	//check if TE signal is active:
	int TE = digitalRead(TE_PIN);

	LOGGER.println("Saving Data");
	//Save Data:
	double timeInSeconds = millis()/1000.0;
	// time, thermos, TE, IR
	String combinedCSV = String(timeInSeconds).append(",").append(thermocoupleData).append(TE);
	String irArray = irSensor.ir_loop();
	combinedCSV.append(irArray);

	// Save to file.
	DualSDManager.writeln(combinedCSV);
	
	delay(SCAN_DELAY);
}

void createCSVHeader(){
	LOGGER.println("Creating CSV Header");
	String header = "seconds_after_power,hot_junction_1,hot_junction_2,hot_junction_3,hot_junction_4,hot_junction_5,cold_junction_1,cold_junction_2,cold_junction_3,cold_junction_4,cold_junction_5,ADC_1,ADC_2,ADC_3,ADC_4,ADC_5,timed_event_detected_bool";
	
	// For each pixel, write out CSV
	for (int r = 0; r < 12; r++) {    // rows
		for (int c = 0; c < 16; c++) {  // columns
			header.append(",ir_row_");
			header.append(r);
			header.append("_col_");
			header.append(c);
		}
	}

	// Create data file.
	DualSDManager.initializeFiles(header.c_str());
}

void setupPins() 
{
	// TODO
	pinMode(TE_PIN, INPUT);
}