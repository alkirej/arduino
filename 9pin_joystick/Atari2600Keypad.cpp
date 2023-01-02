#include "Atari2600Keypad.h"
#include "LegacyJoystick.h"

#define AT2600KP_POWER_PIN 7
#define AT2600KP_GROUND_PIN 8

#define AT2600KP_DISPLAY_NAME "Atari 2600 Keypad"
#define AT2600KP_BTN_COUNT 12

#define AT2600KP_POUND_ROW 4
#define AT2600KP_POUND_COL 3


// Write to a colum pin and read the row pins to see if any
// are currently pressed.
const int AT2600KP_ROW_PINS[AT2600KP_NUM_ROWS] = { 1, 2, 3, 4 };
const int AT2600KP_COL_PINS[AT2600KP_NUM_COLS] = { 5, 9, 6 };

Atari2600Keypad::Atari2600Keypad( PinSet pinNums ) : LegacyJoystick( AT2600KP_DISPLAY_NAME, pinNums ) {
    setupPins(pinNums);
}

static bool Atari2600Keypad::checkForPressedPound( PinSet ardPinNums ) {
    bool returnVal;
    int  jsColPin, jsRowPin;

    jsRowPin = AT2600KP_ROW_PINS[AT2600KP_POUND_ROW-1];
    digitalWrite( joystickPinToArduinoPin(ardPinNums,jsRowPin), LOW );

    jsColPin = AT2600KP_COL_PINS[AT2600KP_POUND_COL-1];
    returnVal = !digitalRead( joystickPinToArduinoPin(ardPinNums,jsColPin) );

    return returnVal;
}

static Atari2600Keypad* Atari2600Keypad::checkForAtari2600Keypad( PinSet ardPinNums ) {
    Atari2600Keypad::setupPins(ardPinNums);
    if ( checkForPressedPound(ardPinNums) ) {
        return new Atari2600Keypad( ardPinNums );
    } else { 
        return NULL;
    }
}

static void Atari2600Keypad::setupPins( PinSet ardPinNums ) {
    // Setup pins for an atari 2600 keypad
    int nbr, pn;

    // GROUND PIN
    pinMode( AT2600KP_GROUND_PIN, OUTPUT );
    digitalWrite( AT2600KP_GROUND_PIN, LOW );

    // SET ALL ROW PINS HIGH OUTPUT
    for ( int n=0; n<AT2600KP_NUM_ROWS;n++ ) {
        int pn  = joystickPinToArduinoPin( ardPinNums, AT2600KP_ROW_PINS[n] );
        pinMode( pn, OUTPUT );
        digitalWrite( pn, HIGH );
    }
    // SET COL PINS AS INPUT 
    for ( int n=0; n<AT2600KP_NUM_COLS;n++ ) {
        int pn  = joystickPinToArduinoPin( ardPinNums, AT2600KP_COL_PINS[n] );
        pinMode( pn, INPUT_PULLUP );
    }

    // SET THE POWER PIN TO HAVE POWER.
    pn  = joystickPinToArduinoPin( ardPinNums, AT2600KP_POWER_PIN );
    pinMode( pn, OUTPUT );
    digitalWrite( pn, HIGH );
}

void Atari2600Keypad::jsStateToUsb() {
    for ( int i=0;  i<AT2600KP_NUM_ROWS; i++ ) {
        examinOneRowOfButtons(i);
    }
}

// 0 based index of rows.  (0-3) not (1-4)
void Atari2600Keypad::examinOneRowOfButtons( short row ) {

    // Set the pin corresponding to this row to low.
    short rowPin = arduinoPinFor(  AT2600KP_ROW_PINS[row]  );

    digitalWrite( rowPin, LOW );
    for ( int i=0;  i<AT2600KP_NUM_COLS ; i++ ) {
        short colPin = arduinoPinFor(  AT2600KP_COL_PINS[i]  );
        bool pressed = !digitalRead( colPin );

        short btnNum = 3+row*AT2600KP_NUM_COLS + i; // Keypad buttons start at 3
        setBtnState( btnNum, pressed );
    } // for

    // Return pin to up state.
    digitalWrite( rowPin, HIGH );
}

