#include "pins_arduino.h"
#include "Atari2600Paddles.h"

#include <Logger.h>

#define AT2600PDL_DISPLAY_NAME "Atari 2600 Paddles"

#define AT2600PDL_LEFT_BUTTON_PIN   3
#define AT2600PDL_RIGHT_BUTTON_PIN  4

#define AT2600PDL_GROUND_PIN        8
#define AT2600PDL_POWER_PIN         7

#define AT2600PDL_LEFT_POSITION     9
#define AT2600PDL_RIGHT_POSITION    5

#define AT2600PDL_LEFT_BTN_NUM      0
#define AT2600PDL_RIGHT_BTN_NUM     1

const static short AT2600PDL_BTN_NUMS[ Atari2600Paddles::AT2600PDL_BTN_COUNT ] = 
                   { AT2600PDL_LEFT_BTN_NUM, AT2600PDL_RIGHT_BTN_NUM };
const static short AT2600PDL_BTN_PIN_LIST[ Atari2600Paddles::AT2600PDL_BTN_COUNT ] = 
                   { AT2600PDL_LEFT_BUTTON_PIN, AT2600PDL_RIGHT_BUTTON_PIN };

Atari2600Paddles::Atari2600Paddles( PinSet pinNums ) : LegacyJoystick( AT2600PDL_DISPLAY_NAME, pinNums ) {
    setupPins(pinNums);
}

static void Atari2600Paddles::setupPins( PinSet ardPinNums ) {
    int pn  = joystickPinToArduinoPin( ardPinNums, AT2600PDL_GROUND_PIN );
    pinMode( pn, OUTPUT );
    digitalWrite( pn, LOW );

    pn  = joystickPinToArduinoPin( ardPinNums, AT2600PDL_POWER_PIN );
    pinMode( pn, OUTPUT );
    digitalWrite( pn, HIGH );

    pn  = joystickPinToArduinoPin( ardPinNums, AT2600PDL_LEFT_BUTTON_PIN );
    pinMode( pn, INPUT_PULLUP );
    
    pn  = joystickPinToArduinoPin( ardPinNums, AT2600PDL_RIGHT_BUTTON_PIN );
    pinMode( pn, INPUT_PULLUP );

    pn  = joystickPinToArduinoPin( ardPinNums, AT2600PDL_LEFT_POSITION );
    pinMode( pn, INPUT );

    pn  = joystickPinToArduinoPin( ardPinNums, AT2600PDL_RIGHT_POSITION );
    pinMode( pn, INPUT );
}


static Atari2600Paddles* Atari2600Paddles::checkForAtari2600Paddles( PinSet ardPinNums ) {
    Atari2600Paddles::setupPins( ardPinNums );

    // Determine if either fire button is pressed.
    int  pn  = joystickPinToArduinoPin( ardPinNums, AT2600PDL_LEFT_BUTTON_PIN );
    bool pressed = !digitalRead(pn);
    int  pn2 = joystickPinToArduinoPin( ardPinNums, AT2600PDL_RIGHT_BUTTON_PIN );
    bool pressed2 = !digitalRead(pn2);

    if ( pressed || pressed2 ) {
        return new Atari2600Paddles( ardPinNums );
    }

    return NULL;
}

void Atari2600Paddles::jsStateToUsb() {
    updateButtonStates( 2, AT2600PDL_BTN_PIN_LIST, AT2600PDL_BTN_NUMS );

    // PADDLE POSITIONS
    short pn = arduinoPinFor( AT2600PDL_LEFT_POSITION );
//pinMode(pn,INPUT);
//digitalWrite(pn,LOW);
    setAxisTo( X_AXIS, analogRead(pn) );
//digitalWrite(pn,HIGH);

    pn = arduinoPinFor( AT2600PDL_RIGHT_POSITION );
    setAxisTo( Y_AXIS, analogRead(pn) );
}
