#include <SD.h>

#ifndef DUALSD_H
#define DUALSD_H

class DualSD {

    private:

        File teensySDFile;
        File externalSDFile;
        File logFile;

        String teensySDFileName;
        String externalSDFileName;
        String logFileName;

        SDClass SDEXT;

        int teensy_cspin;
        int external_cspin;

        bool isTeensyConnected;
        bool isExtConnected;

    public:

        DualSD();

        int begin();

        int exists(char* filename);

        int initializeFiles(const char* dataHeaders);

        int writeln(String data, bool debug = false);
        int write(String data, bool debug = false);
        
        int writeDebug(String data);
        int writeDebugln(String data);

        int read(char* teensyOutput, char* extOutput);

        unsigned short generateNewFileID();
        
        void writeLnToDebug(String str);
        

};

#endif