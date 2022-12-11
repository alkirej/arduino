/**
 * Author:   Jeff Alkire
 *
 * Written:  January 2020
 *     When the joystick rotated right (clockwise), send a period (.).
 *     When it is rotated left (counterclockwise), send a comma(,).
 * Udpated:  January 2022
 *     Adding support for frontline in MAME as follows:
 *         - Frontline arcade used some kind of rotating joystick or
 *           or paddle for movement of the gun.
 *         - MAME implents the gun direction using an 8 way joystick.
 *         - This requires tracking the currently pointed direction
 *         - When the joystick is rotated, the new direction is 
 *           determined. Then one or two keypress(es) associated with 
 *           the direction are "pressed".
 *         - Ordinal directions (N S E W) require only one press while
 *           non-ordinal directions (NE SE NW SW) require two similarly
 *           to how MAME handles joysticks.
 *
 *
 * Implement a rotating joystick.  The issue is as follows:
 * 1-Mame is expecting an input when the joystick is rotated. 
 *   If the joystick is rotated clockwise, one key is pressed, and
 *   if the joystick is rotated counter-clockwise a second key is 
 *   pressed. (Note: once the key is pressed, it can be released.)
 * 2-The joystick recognizes 12 states. One for each click the joystick
 *   makes as it completes a circuit (circle). This state is always 
 *   active until the joystick is rotated.
 * 3-Since this is not a direct map, some low level logic is necessary
 *   to convert the natural state of the rotating joystick into
 *   the input MAME wants.
 *   
 * To implement this, we
 * 1-Connect the 4 joystick directions to joystick buttons as is normal.
 * 2-Connect 13 wires of the rotation circuit board (1 for each of the 12
 *   states mentioned in 2 above plus a ground) to a our arduino.
 * 3-Each time the Arduino loops, it determines the active pin.
 * 4-If the active pin has changed, then it is time to send one or more keypresses.
 */

#include <Keyboard.h>
#include <common.h>

// To debug or not to debug
#define DEBUG_MODE false

// keys to use for clockwise and counter-clockwise rotations
#define CLOCKWISE_KEY '.'
#define COUNTER_CLOCKWISE_KEY ','
#define DIRECTION_COUNT 8

#define UP_KEY    'u'
#define DOWN_KEY  'd'
#define RIGHT_KEY 'r'
#define LEFT_KEY  'l'
#define EMPTY_KEY 0

#define DEBOUNCE_MS         10
#define KEYPRESS_DOWN_MS    50
#define FIRST_PIN            2
#define MIDDLE_PIN           8
#define LAST_PIN_BEFORE_GAP 10
#define FIRST_PIN_AFTER_GAP 14
#define LAST_PIN            16

int charsToSend[DIRECTION_COUNT][2]
        = { { UP_KEY,    EMPTY_KEY },
            { UP_KEY,    RIGHT_KEY },
            { RIGHT_KEY, EMPTY_KEY },
            { RIGHT_KEY, DOWN_KEY  },
            { DOWN_KEY,  EMPTY_KEY },
            { DOWN_KEY,  LEFT_KEY  },
            { LEFT_KEY,  EMPTY_KEY },
            { LEFT_KEY,  UP_KEY    }
        };


// Remember the pin that was active the last time we checked.
int previousState = 0;
// Remember which of the 8 directions we returned last time.
int gunDirection  = 0;

// Set all pins to a high, readable state. (This is standard for arcade
// controls that receive electricity via these pins.)
void initializePins() 
{
    serialLogLn( "--> Initializing Pins <--" );

    for ( int i=FIRST_PIN; i<=LAST_PIN_BEFORE_GAP; i++ )
    {
        serialLog( "   <Init pin " ); 
        serialLog( i );
        serialLogLn( ">" );
        pinMode( i, INPUT_PULLUP );
    }
    for ( int i=FIRST_PIN_AFTER_GAP; i<=LAST_PIN; i++ )
    {
        serialLog( "   <Init pin " ); 
        serialLog( i );
        serialLogLn( ">" );
        pinMode( i, INPUT_PULLUP );
    }
}

// Check the pins and determine which one is low (meaning active).
int getActivePin()
{
    int pinState;

    for ( int i=FIRST_PIN; i<=LAST_PIN_BEFORE_GAP; i++ )
    {
        pinState = digitalRead( i );
        if ( pinState == 0 )
            return i;
    }
    for ( int i=FIRST_PIN_AFTER_GAP; i<=LAST_PIN; i++ )
    {
        pinState = digitalRead( i );
        if ( pinState == 0 )
            return i;
    }

    return previousState;
}

// Has there been a clockwise move detected?
// If a move has been detected before this routine is called, then
// a false return indicates a counterclockwise rotation.
// A true indicates a clockwise rotation.
boolean moveClockwise( int beforePin, int afterPin )
{
    if ( beforePin+1 == afterPin )
    {
        return true;  
    } 
    else if ( beforePin == afterPin+1 )
    {
        return false;
    } 
    else if ( beforePin == FIRST_PIN_AFTER_GAP && afterPin == LAST_PIN_BEFORE_GAP )
    {
        return false;
    }
    else if ( beforePin == LAST_PIN_BEFORE_GAP && afterPin == FIRST_PIN_AFTER_GAP )
    {
        return true;
    }
    else if ( beforePin == LAST_PIN && afterPin == FIRST_PIN ) {
        return true;
    }
    else if ( beforePin == FIRST_PIN && afterPin == LAST_PIN ) {
        return false;
    }
    else 
    {
        return (afterPin < MIDDLE_PIN);
    }
}

// Update the gun point direction clockwise.
void updateDirectionClockwise() {
    gunDirection = ( gunDirection + 1 ) % DIRECTION_COUNT;
}

// Generate a keypress indicating a clockwise rotation
void moveClockwiseKeypress()
{
    serialLog( CLOCKWISE_KEY );
    serialLogLn();
    Keyboard.press( CLOCKWISE_KEY );
    delay(KEYPRESS_DOWN_MS);
    Keyboard.release( CLOCKWISE_KEY );
}

// Update the gun point direction counter-clockwise.
void updateDirectionCounterClockwise() {
    gunDirection = ( gunDirection - 1 );
    if ( gunDirection < 0 )
    {
        gunDirection = DIRECTION_COUNT - 1;
    }
}

// Generate a keypress indicating a counter-clockwise rotation
void moveCounterClockwiseKeypress()
{
    serialLog( COUNTER_CLOCKWISE_KEY );
    serialLogLn();
    Keyboard.press( COUNTER_CLOCKWISE_KEY );
    delay(KEYPRESS_DOWN_MS);
    Keyboard.release( COUNTER_CLOCKWISE_KEY );
}

// Release the keys for the current direction
void releaseCurrentDirectionKeys() {
    serialLog( "   Release keys:" );

    for ( int i=0; i<2; i++ )
    {
        Keyboard.release( charsToSend[gunDirection][i] );
        serialLog( " " );
        serialLog( char(charsToSend[gunDirection][i]) );
    }
    serialLogLn();
}

// Press keys for the current direction.
void pressCurrentDirectionKeys() {
    serialLog( "    Press   keys:" );

    for ( int i=0; i<2; i++ )
    {
        Keyboard.press( charsToSend[gunDirection][i] );
        serialLog( " " );
        serialLog( char(charsToSend[gunDirection][i]) );
    }
    
    serialLogLn();
}

// Call this routine when the state of the pins has changed.
// This routine will determine which way the joystick was rotated
// and send the appropriate keypress out the usb cable.
void processStateChange( int beforePin, int afterPin )
{
    serialLog( "Processing State Change. before: " );
    serialLog( beforePin );
    serialLog( "  after: " );
    serialLog( afterPin );
    serialLogLn();

    bool clockwiseDir = moveClockwise( beforePin, afterPin );
    if ( clockwiseDir )
    {
        updateDirectionClockwise();
        pressCurrentDirectionKeys();
        moveClockwiseKeypress();
    } 
    else 
    {
        updateDirectionCounterClockwise();
        pressCurrentDirectionKeys();
        moveCounterClockwiseKeypress();
    }
    delay(KEYPRESS_DOWN_MS);
    releaseCurrentDirectionKeys();
}



// Setup routine that initializes the state of the Arduino device
void setup() {
    // put your setup code here, to run once:
    delay( 100 );

    // Use Serial port in debug mode only.
    if ( DEBUG_MODE )
    {
        // Wait for the serial port to be opened
        while (!Serial) {
            // flash led
            digitalWrite( 13, HIGH );
            delay(500);
            digitalWrite( 13, LOW );
            delay(500);
        }

        Serial.begin( 9600 );
        Serial.println( "Starting ..." );
        Serial.println();
        Serial.flush();
    }

    initializePins();
    delay(10);
    previousState = getActivePin();

    pressCurrentDirectionKeys();
    delay(KEYPRESS_DOWN_MS);
    releaseCurrentDirectionKeys();
}

// Constantly check if the joystick has been rotated and
// process that change of state.
void loop() {
    int preDebounceState = getActivePin();
    if ( preDebounceState != previousState )
    {
        int currentState = getActivePin();
        if ( currentState == preDebounceState )
        {
            processStateChange( previousState, currentState );
        }
        previousState = currentState;
    }
}
