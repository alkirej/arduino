#include "TI994aJoystick.h"

#include <Logger.h>

#define PIN_UP 3
#define PIN_DOWN 8
#define PIN_RIGHT 9
#define PIN_LEFT 5
#define PIN_BTN 4

#define PIN_JS1_GRD 2
#define PIN_JS2_GRD 7

#define TI994A_BTN_NUM 0

#define TI994AJS_DISPLAY_NAME "TI-994/a"

TI994aJoystick::TI994aJoystick( PinSet pinNums, bool leftJoystick )
            : LegacyJoystick( TI994AJS_DISPLAY_NAME,
                              pinNums
                            ) {
    TI994aJoystick::setupPins(pinNums);
    _controllerCount;

}

static void TI994aJoystick::setupPins( PinSet ardPinNums ) {
    short pn  = joystickPinToArduinoPin( ardPinNums, PIN_UP );
    pinMode( pn, INPUT_PULLUP );

    pn  = joystickPinToArduinoPin( ardPinNums, PIN_DOWN );
    pinMode( pn, INPUT_PULLUP );

    pn  = joystickPinToArduinoPin( ardPinNums, PIN_RIGHT );
    pinMode( pn, INPUT_PULLUP );

    pn  = joystickPinToArduinoPin( ardPinNums, PIN_LEFT );
    pinMode( pn, INPUT_PULLUP );

    pn  = joystickPinToArduinoPin( ardPinNums, PIN_BTN );
    pinMode( pn, INPUT_PULLUP );

    pn = joystickPinToArduinoPin( ardPinNums, PIN_JS1_GRD );
    pinMode( pn, OUTPUT );
    digitalWrite( pn, HIGH );

    pn = joystickPinToArduinoPin( ardPinNums, PIN_JS2_GRD );
    pinMode( pn, OUTPUT );
    digitalWrite( pn, HIGH );
}


TI994aJoystick* TI994aJoystick::checkForTI994aJoysticks( PinSet ardPinNums ) {
    setupPins(ardPinNums);

    // Setup to read left joystick
    short grd_pn = joystickPinToArduinoPin( ardPinNums, PIN_JS1_GRD );
    pinMode( grd_pn, OUTPUT );
    digitalWrite( grd_pn, LOW );

    short pn  = joystickPinToArduinoPin( ardPinNums, PIN_BTN );
    bool val = !digitalRead( pn );

    if ( val ) {
        return new TI994aJoystick(ardPinNums,true);
    }

    // Done reading left joystick
    digitalWrite( grd_pn, HIGH );

    // Setup to read right joystick
    grd_pn = joystickPinToArduinoPin( ardPinNums, PIN_JS2_GRD );
    pinMode( grd_pn, OUTPUT );
    digitalWrite( grd_pn, LOW );

    pn  = joystickPinToArduinoPin( ardPinNums, PIN_BTN );
    val = !digitalRead( pn );

    if ( val ) {
        return new TI994aJoystick(ardPinNums,false);
    }

    // done reading right joystick
    digitalWrite( grd_pn, HIGH );

    return NULL;
}

#define POS  1024
#define ZERO  512
#define NEG     0

void TI994aJoystick::jsStateToUsb() {
    // Get proper pin to ground for this joystik.
    short de9GrdPn;
    if ( _leftJs ) de9GrdPn = PIN_JS1_GRD;
    else de9GrdPn = PIN_JS2_GRD;

    // Setup to read joystick by grounding proper pin.
    short grdPn = arduinoPinFor( de9GrdPn );
    pinMode( grdPn, OUTPUT );
    digitalWrite( grdPn, LOW );

    // Check for button press
    short pn = arduinoPinFor( PIN_BTN );
    short pressed = !digitalRead( pn );
    setBtnState(TI994A_BTN_NUM, pressed);

    // Check joystick location
    // DIRECTION
    short x = ZERO,
          y = ZERO;

    pn = arduinoPinFor( PIN_RIGHT );
    pressed = !digitalRead( pn );
    if ( pressed ) {
        x = POS;
    } else {
        pn = arduinoPinFor( PIN_LEFT );
        pressed = !digitalRead( pn );
        if ( pressed ) x = NEG;
    }

    pn = arduinoPinFor( PIN_UP );
    pressed = !digitalRead( pn );
    if ( pressed ) {
        y = POS;
    } else {
        pn = arduinoPinFor( PIN_DOWN );
        pressed = !digitalRead( pn );
        if ( pressed ) y = NEG;
    }


    AxisState both[2] = { {X_AXIS, x}, {Y_AXIS, y} };
    setAxes( 2, both );

    // Done reading joystick
    digitalWrite( grdPn, HIGH );
}
