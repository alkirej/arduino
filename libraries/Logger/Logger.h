#include <HardwareSerial.h>

#ifndef LOGGER_H
#define LOGGER_H

/*
    IMPORTANT:  To use the logger, add this line to the top of your main sketch to create a logger object.

    Logger logger( (HardwareSerial *) &Serial, true);

 */

class Logger {
    private:
        bool _debug;
        HardwareSerial* _port;

    public:
        Logger( HardwareSerial* serialPort, bool debug_mode );

        Logger logBool( bool b );
        Logger logBoolln( bool b );
        Logger logChar( char letter );
        Logger logCharln( char letter );
        Logger log( long number );
        Logger logln( long number );
        Logger log( const char* message );
        Logger logln( const char* message );

        Logger logln();

    private:
        void startSerial();
}; // Logger class

extern Logger logger;


#endif
