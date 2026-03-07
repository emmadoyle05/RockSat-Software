#include "IRSensor.h"
#include <ThreeWayLogger.h>

// ==== Modified for COC RSX ====
// ====== Original author credit ======
// Author: D. Dubins
// Lots of help from: ChatGPT 3.0, Perplexity.AI
// Date: 17-Dec-25
// Notes: the MLX90641 operating voltage is 3-3.6V (typical: 3.3V).
// Use a logic shifter, or connect to an MCU that operates at 3.3V (e.g. NodeMCU).
// After the device powers up and sends data, a thermal stabilization time is required
// before the device can reach the specified accuracy (up to 3 min) - 12.2.2

// helper function to convert pixel row, col to 1D index array
int pixelAddr(int row, int col){
  return (row*16)+col; // convert row, col to 1D array index in 1D array of pixels (e.g. badPixels[])
}

void IRSensor::begin() 
{
    IR_WIRE.begin();                          // SDA, SCL for the ESP32 (SDA: GPIO 21, SDL: GPIO22). Change these to your I2C pins if using a different bus.
    IR_WIRE.setClock(I2C_SPEED);                    // set I2C clock speed (slower=more stable)
    if (myIRcam.setRefreshRate(REFRESH_RATE)) {  // set the page refresh rate (sampling frequency)
        LOGGER.println("Refresh rate adjusted.");
    } else {
        LOGGER.println("Error on adjusting refresh rate.");
    }
    delay(POR_DELAY);  // Power on reset delay (POR), see table above
    LOGGER.println("MLX90641 ESP32 Calibrated Read");
    // Read full EEPROM (0x2400..0x272F)
    if (!myIRcam.readEEPROMBlock(0x2400, EEPROM_WORDS, myIRcam.eeData)) {
        Serial.println("EEPROM read failed!");
        while (1) delay(1000);
    }

    // Mark bad pixels separately here (row indexes 0...11, col indexes 0..15)
    //myIRcam.badPixels[pixelAddr(9,14)]=true;    // mark pixel bad at row 9, column 14
    //myIRcam.badPixels[pixelAddr(11,0)]=true;    // mark pixel bad at row 11, column 0

    // Check EEPROM data:
    #ifdef DEBUG
    LOGGER.println("setup() First 16 words of EEPROM:");
    for (int i = 0; i < 16; i++) {
        LOGGER.println("EEPROM value at address: 0x" + String(0x2400 + i, HEX) + ", value: 0x" + String(eeData[i], HEX));
    }
    LOGGER.println("setup() Suspicious EEPROM value check:");
    for (int i = 0; i < EEPROM_WORDS; i++) {
        if (myIRcam.eeData[i] == 0x0000 || myIRcam.eeData[i] == 0xFFFF) {
        LOGGER.println("EEPROM value suspicious at address: 0x" + String(0x2400 + i, HEX) + ", value: 0x" + String(eeData[i], HEX));
        }
    }
    #endif
    
    initialize();
}

void IRSensor::initialize() 
{
    myIRcam.Vdd = myIRcam.readVdd();  // This should be close to 3.3V. Can read once in setup.
    myIRcam.Ta = myIRcam.readTa();    // should happen inside the loop
    LOGGER.print("Ambient temperature on start: ");
    LOGGER.println(myIRcam.Ta);      // This should be close to ambient temperature (21°C?)
    myIRcam.readPixelOffset();          // only needs to be read once
    myIRcam.readAlpha();                // read sensitivities (fills alpha_pixel[])
    myIRcam.readKta();                  // read Kta coefficients (fills Kta[])
    myIRcam.readKv();                   // read Kv coefficients (fills Kv[])
    myIRcam.KsTa = myIRcam.readKsTa();  // read KsTa coefficient
    LOGGER.println("Finished: read KsTA.");
    myIRcam.readCT();                               // read 8 corner temperatures
    myIRcam.readKsTo();                             // read 8 KsTo coefficients
    myIRcam.readAlphaCorrRange();                   // read sensitivity correction coefficients
    myIRcam.Emissivity = myIRcam.readEmissivity();  // read Emissivity coefficient
    //myIRcam.Emissivity = 0.95;                    // un-comment to over-write Emissivity with hard-coded value here (e.g. 0.95)
    myIRcam.alpha_CP = myIRcam.readAlpha_CP();      // read Sensitivity alpha_CP coefficient
    myIRcam.pix_OS_ref_CP = myIRcam.readOff_CP();   // read offset CP (also called pix_OS_ref_CP)
    myIRcam.Kv_CP = myIRcam.readKv_CP();            // read Kv CP
    myIRcam.KTa_CP = myIRcam.readKTa_CP();          // read KTa_CP
    myIRcam.TGC = myIRcam.readTGC();                // read TGC - do this last (leaves setup function for some odd reason)
}

String IRSensor::ir_loop() 
{
    String result = "";
    unsigned long pollStart = millis();
    while (millis() - pollStart < 500) {  // 500ms max wait (adjust to > 1000 * 1.2 * (1/refresh_rate in Hz))
        if (myIRcam.isNewDataAvailable()) break;
        delay(10);  // Yield to prevent watchdog/I2C lockup
    }
    if (myIRcam.isNewDataAvailable()) {
        myIRcam.clearNewDataBit();

        myIRcam.readTempC();  // read the temperature

        LOGGER.print(myIRcam.Ta);

        for (int r = 0; r < 12; r++) {    // rows
        for (int c = 0; c < 16; c++) {  // columns
            result.append(",");  // print divider for data (needed for both display modes)
            result.append(myIRcam.T_o[r * 16 + c]);  // putting the data in a 16x12 grid
        }
        }
        LOGGER.println("");
    } else {
        LOGGER.println("Timeout: No new data");
        for (int i = 0; i < 12*16; i++)
        {
            result.append(",");
        }
        
        return result;  // Skip this frame
    }
    delay(SAMPLE_DELAY);  // wait for new reading (adjust to desired sample frequency, see refresh rate table in setRefreshRate() for ranges)
    return result;
}