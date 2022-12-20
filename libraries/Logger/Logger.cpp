/*
 * Author:  Jeffery Alkire
 * Written: January 2022
 * Purpose: Simpleish functions I use for logging in every sketch.
 *
 * Updated: December 2022
 *          Moved into a class for easier coding.
 */
#include <util/delay.h>
#include <HardwareSerial.h>

#include "Logger.h"

Logger::Logger( HardwareSerial* serialPort, bool debug_mode ) {
    // initialize variable
    _port  = serialPort;
    _debug = debug_mode;

    // wait for port to connect.
    startSerial();
}

Logger Logger::logBool( bool b ) {
    if ( _debug ) {
      _port->print( b ? "true" : "false" );
    }
    return *this;
}
Logger Logger::logBoolln( bool b ) {
    logBool( b );
    return logln();
}

Logger Logger::logChar( char letter ) {
    if ( _debug ) {
        _port->print( letter );
    }
    return *this;
}
Logger Logger::logCharln( char letter ) {
    logChar( letter );
    return logln();
}

Logger Logger::logln( long number ) {
    log( number );
    return logln();
}

Logger Logger::log( long number ) {
    if ( _debug ) {
        _port->print( number );
    }
    return *this;
}

Logger Logger::log( const char* message ) {
    if ( _debug ) {
        _port->print( message );
    }
    return *this;
}

Logger Logger::logln( const char* message ) {
    log( message );
    return logln() ;
}

Logger Logger::logln() {
    if ( _debug ) {
        _port->println();
        _port->flush();
    }
    return *this;
}

void Logger::startSerial() {
    // Use Serial port in debug mode only.
    if ( _debug ) {
        _port->begin(9600);
        _delay_ms( 1000 );

        // Wait for the serial port to be opened
        //while (!_port) { }

        logln( "=============================" );
        logln( "ARDUNIO BOOT SEQUENCE (Setup)" );
        logln( "=============================" );
    }
} // startSerial fn
