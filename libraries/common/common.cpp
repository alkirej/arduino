/*
 * Author:  Jeffery Alkire
 * Written: January 2022
 * 
 * Contains: 
 *     Simpleish functions I use in most every sketch I write.
 */

#include <common.h>
#include <util/delay.h>
#include <HardwareSerial.h>

bool DEBUG_MODE = true;

HardwareSerial* serial = NULL;

/**
 * Should we be in debug mode???
 */
void setupLogger( HardwareSerial* serialPortObject,
		  bool            debugging 
		) {
        serial     = serialPortObject;
	DEBUG_MODE = debugging;
	waitForSerial();
}

/**
 * When in debug mode, don't start until the monitor is connected.
 */
void waitForSerial() {
    // Use Serial port in debug mode only.
    if ( DEBUG_MODE )
    {
        // _delay_ms( 5000 );
        serial->begin(9600); 

        // Wait for the serial port to be opened
        while ( serial->available() )  {
            serialLogLn( "NOT available" );
            _delay_ms( 100 );
	}

	serialLogLn( serial->available() );

        serialLogLn( "=============================" );
        serialLogLn( "ARDUNIO BOOT SEQUENCE (Setup)" );
        serialLogLn( "=============================" );
    }
}

/**
 * Log a message to the serial output (if we are in debug mode and can read it).
 */
void serialLog( bool b ) {
	serial->print( b ? "True" : "False" );
}
void serialLogLn( bool b ) {
	serialLog( b );
	serialLogLn();
}

void serialLog( const bool bitmap[], int bitmapLength ) {
	for ( int i=0;  i<bitmapLength;  i+=1 )
	{
		serial->print( bitmap[i] ? "1" : "0" );
	}
}
void serialLogLn( const bool bitmap[], int bitmapLength ) {
	serialLog( bitmap, bitmapLength );
	serialLogLn();
}

void serialLog( char letter ) {
    if ( DEBUG_MODE ) {
        serial->print( letter );
    }
}
void serialLogLn( char letter ) {
    serialLog( letter );
    serialLogLn();
}

void serialLog( int number ) {
    char stringValue[8];
    
    sprintf( stringValue, "%d", number);
    serialLog( stringValue );
}
void serialLogLn( int number ) {
    serialLog( number );
    serialLogLn();
}

void serialLog( long number ) {
    char stringValue[16];
    
    sprintf( stringValue, "%d", number);
    serialLog( stringValue );
}
void serialLogLn( long number ) {
    serialLog( number );
    serialLogLn();
}

void serialLog( const char* message ) {
    if ( DEBUG_MODE ) {
        serial->print( message );
    }
}
void serialLogLn( const char* message ) {
    serialLog( message );
    serialLogLn() ;
}

void serialLogLn() {
    if ( DEBUG_MODE ) {
        serial->println();
        serial->flush();
    }
}

