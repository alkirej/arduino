#include "Atari2600Joystick.h"

#include <Logger.h>

// May include standard 7800 controllers in this class.

#define AT2600JS_DISPLAY_NAME "Atari 2600 Compat"

#define AT2600JS_UP_PIN   1
#define AT2600JS_DWN_PIN  2
#define AT2600JS_LFT_PIN  3
#define AT2600JS_RGT_PIN  4
#define AT2600JS_BTN_PIN  6

#define AT2600JS_BTN_2_PIN 7
#define AT2600JS_GRD_PIN  8

#define AT7800JS_RIGHT_BTN_PIN 5
#define AT7800JS_PWR_PIN       7
#define AT7800JS_LEFT_BTN_PIN  9

#define AT2600JS_BTN_NUM_1  0
#define AT2600JS_BTN_NUM_2  1
#define AT2600JS_BTN_NUM_3  2

Atari2600Joystick::Atari2600Joystick( PinSet pinNums ) : LegacyJoystick( AT2600JS_DISPLAY_NAME, pinNums ) {
    Atari2600Joystick::setupPins(pinNums);
}

static void Atari2600Joystick::setupPins( PinSet ardPinNums ) {
    int pn  = joystickPinToArduinoPin( ardPinNums, AT2600JS_GRD_PIN );
    pinMode( pn, OUTPUT );
    digitalWrite( pn, LOW );

    pn  = joystickPinToArduinoPin( ardPinNums, AT2600JS_UP_PIN );
    pinMode( pn, INPUT_PULLUP );

    pn  = joystickPinToArduinoPin( ardPinNums, AT2600JS_DWN_PIN );
    pinMode( pn, INPUT_PULLUP );

    pn  = joystickPinToArduinoPin( ardPinNums, AT2600JS_RGT_PIN );
    pinMode( pn, INPUT_PULLUP );

    pn  = joystickPinToArduinoPin( ardPinNums, AT2600JS_LFT_PIN );
    pinMode( pn, INPUT_PULLUP );

    pn  = joystickPinToArduinoPin( ardPinNums, AT2600JS_BTN_PIN );
    pinMode( pn, INPUT_PULLUP );

    // pn  = joystickPinToArduinoPin( ardPinNums, AT2600JS_BTN_2_PIN );
    // pinMode( pn, INPUT_PULLUP );
    // pinMode( pn, OUTPUT );
    // digitalWrite( pn, LOW );

}

#define ZERO  512
#define POSITIVE 1024
#define NEGATIVE 0

#define ATARI_NUM_DIRS 4

struct AtariMove { short  pin;
                   short axis;
                   short  val;
                 };
static const AtariMove dirs[ATARI_NUM_DIRS] =
    { { 1, Y_AXIS, POSITIVE }, // up
      { 2, Y_AXIS, NEGATIVE }, // down
      { 3, X_AXIS, NEGATIVE }, // left
      { 4, X_AXIS, POSITIVE }  // right
    };

static Atari2600Joystick* Atari2600Joystick::checkForAtariJoystick( PinSet ardPinNums ) {
    setupPins(ardPinNums);
    // Determine if either fire button is pressed.
    int  pn  = joystickPinToArduinoPin( ardPinNums, AT2600JS_BTN_PIN );
    bool pressed = !digitalRead(pn);
    if ( pressed ) {
        return new Atari2600Joystick( ardPinNums );
    } else {
        return NULL;
    }
}

static bool last[2] = {0,0};

void Atari2600Joystick::drvPdlStateToUsb() {
    short x = ZERO;
    short pn = arduinoPinFor( dirs[0].pin );
    bool  byte1 = !digitalRead( pn );

    pn = arduinoPinFor( dirs[1].pin );
    bool byte2 = !digitalRead( pn );

    if ( last[0]==byte1 && last[1]==byte2 ) {
        // no movement.
    } else {
        logger.log( byte1 ).log(byte2).log(" ");
        if ( last[0] == last[1] ) {
            if ( byte2 == last[1] ) {
                x = NEGATIVE;
                logger.log("Left");
            } else {
                x = POSITIVE;
                logger.log("Right");
            }
        } else {
            if ( byte2 == last[1] ) {
                x = POSITIVE;
                logger.log("Right");
            } else {
                x = NEGATIVE;
                logger.log("Left");
            }
        }

        logger.logln();
        last[0] = byte1;
        last[1] = byte2;
    }
    AxisState xy[2] = { {X_AXIS, x}, {Y_AXIS, ZERO} };
    setAxes( 2, xy );
}

void Atari2600Joystick::jsStateToUsb() {
    // button(s)
    short pn = arduinoPinFor( AT2600JS_BTN_PIN );
    bool pressed = !digitalRead( pn );
    setBtnState(AT2600JS_BTN_NUM_1, pressed);

    // DRIVING PADDLE HANDLES DIRECTION WITH A SPECIAL ALGORITHM
    if ( _drvPdl ) {
        drvPdlStateToUsb();
        return;
    }

    pn = arduinoPinFor( AT2600JS_BTN_2_PIN );
    pressed = !digitalRead( pn );
    setBtnState(AT2600JS_BTN_NUM_2, pressed);

    // DIRECTION
    short x = ZERO,
          y = ZERO;
    for ( int i=0;  i<ATARI_NUM_DIRS; i++ ) {
        AtariMove mv = dirs[i];
        pn = arduinoPinFor( mv.pin );
        pressed = !digitalRead( pn );
        if (pressed) {
            if ( X_AXIS == mv.axis ) {
                x = mv.val;
            } else {
                if ( y==ZERO ) {
                    y = mv.val;
                } else {
                    _drvPdl = true;
                }
            } // if ... else
        } // if
    } // for

    AxisState both[2] = { {X_AXIS, x}, {Y_AXIS, y} };
    setAxes( 2, both );
} // fn
