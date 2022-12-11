/**
 * Author:  Jeffery Alkire
 * Written: January 2022
 * 
 * Contains: 
 *     Simpleish functions I use in most every sketch I write.
 */

#ifndef COMMON_H
#define COMMON_H

#include <HardwareSerial.h>

/**
 * Call at beginning of setup() function to set debug mode on or off.
 * 
 * Note: default is set to on which will wait until it senses a serial
 *       monitor before it begins its work.
 */
void setupLogger( HardwareSerial* serialPortObject,
		  bool            debugging 
		);

/**
 * When in debug mode, don't start until the monitor is connected.
 */
void waitForSerial();

/**
 * Log a message to the serial output (if we are in debug mode and can read it).
 */
void serialLog( bool b );
void serialLogLn( bool b );

void serialLog( const bool bitmap[], int bitmapLength );
void serialLogLn( const bool bitmap[], int bitmapLength );

void serialLog( char letter );
void serialLogLn( char letter );

void serialLog( int number );
void serialLogLn( int number );

void serialLog( long number );
void serialLogLn( long number );

void serialLog( const char* message );
void serialLogLn( const char* message );

void serialLogLn();

#endif
