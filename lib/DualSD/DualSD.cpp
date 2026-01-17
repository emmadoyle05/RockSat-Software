#include <Arduino.h>
#include <SD.h>
#include <DualSD.h>
#include <string.h>

DualSD::DualSD() {
  
  // what's good, I do literally nothing rn
  
}

// returns number of connected SD cards. Hopefully 2, but can be 1 or 0.
int DualSD::begin(int teensy_cspin, int external_cspin) {

    isTeensyConnected = SD.begin(BUILTIN_SDCARD);
    isExtConnected = SDEXT.begin();

    this->teensy_cspin = teensy_cspin;
    this->external_cspin = external_cspin;
    
    if (isTeensyConnected && isExtConnected) 
    {
        return 2;
    }
    else if (isTeensyConnected || isExtConnected) 
    {
        return 1;
    }
    else 
    {
        return 0;
    }
}

// returns 1 if a file exists on teensy or external, returns -1 if it doesn't exist
int DualSD::exists(char* filename) {

  digitalWrite(teensy_cspin, LOW);
  digitalWrite(external_cspin, HIGH);

  if (isTeensyConnected && SD.exists(filename)) { return 1; }

  digitalWrite(teensy_cspin, HIGH);
  digitalWrite(external_cspin, LOW);

  if (isExtConnected && SDEXT.exists(filename)) { return 1; }

  return -1;

};

// initializes files with ehaders (used for CSV)
// returns 1 if successful, returns -1 if not
int DualSD::initializeFiles(const char* dataHeaders) {
  
  unsigned short id = this->generateNewFileID();

  // 19 and 18 are exactly enough to fit the filename
  teensySDFileName = "tnsy_data_log_";
  externalSDFileName = "ext_data_log_";
  
  teensySDFileName.append(id);
  teensySDFileName.append(".csv");
  externalSDFileName.append(id);
  externalSDFileName.append(".csv");

  // create file and have starter CSV line
  // checks to see if the files already exist
  // if they do, we don't need to initialize
  if ( (SD.exists(teensySDFileName.c_str()) && SDEXT.exists(externalSDFileName.c_str())) ) { return -1; }
      
  this->write(dataHeaders);

  return 1;

}

// writes data to both sd cards
// returns 1 is successful
int DualSD::write(String data) {

  // ensure teensy sd card is selected and external sd is not
  // https://forum.arduino.cc/t/two-sd-cards-on-the-same-project/1012605
  digitalWrite(teensy_cspin, LOW);
  digitalWrite(external_cspin, HIGH);
  
  bool teensySuccess = false;
  bool extSuccess = false;
  
  if (isTeensyConnected) 
  {      
    teensySDFile = SD.open(teensySDFileName.c_str(), FILE_WRITE);
    if (!(teensySDFile.println(data) != sizeof(data))) { return -1; } ;
    teensySDFile.close();
    teensySuccess = true;
  }

  // ensure external sd card is selected and teensy sd is not
  digitalWrite(teensy_cspin, HIGH);
  digitalWrite(external_cspin, LOW);
  
  if (isExtConnected) 
  {      
    externalSDFile = SDEXT.open(externalSDFileName.c_str(), FILE_WRITE);
    if (!(externalSDFile.println(data) != sizeof(data))) { return -1; };
    externalSDFile.close();
    extSuccess = true;
  }

  return teensySuccess || extSuccess ? 1 : -1;

}

int DualSD::read(char* teensyOutput, char* extOutput) {

  // file is not opened or does not exist
  if (!teensySDFile) { return -1; }

  // outputs for whatever is read
  *teensyOutput = teensySDFile.read();
  *extOutput = externalSDFile.read();
  
  return 1;

}

unsigned short DualSD::generateNewFileID() {

  // add code to make id iterate every time a file is created
  // get the last id created in the filesystem
  SDClass* sdToUse = &SD;
  if (!isTeensyConnected && isExtConnected) 
  {
    sdToUse = &SDEXT;
  }

  File root = sdToUse->open("/");

  if (!root.isDirectory()) { Serial.println("Somehow this ain't a directory bruh."); return 0; }

  char* id;

  root.rewindDirectory();

  unsigned short fileCount = 1;

  File currFile;

  for (currFile = root.openNextFile(); currFile != NULL; currFile = root.openNextFile()) {

    fileCount++;

  }

  return fileCount;

}