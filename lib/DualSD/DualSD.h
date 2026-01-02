#include <SD.h>

#ifndef DUALSD_H
#define DUALSD_H

class DualSD {

    private:

        File teensySDFile;
        File externalSDFile;

        String teensySDFileName;
        String externalSDFileName;

        int teensy_cspin;
        int external_cspin;

    public:

        DualSD();

        int begin(int teensy_cspin, int external_cspin);

        int exists(char* filename);

        int initializeFiles(const char* dataHeaders);

        int write(String data);

        int read(char* teensyOutput, char* extOutput);

        unsigned short generateNewFileID();


};

#endif