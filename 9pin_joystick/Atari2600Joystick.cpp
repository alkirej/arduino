#include "Atari2600Joystick.h"

#include <Logger.h>

// May include standard 7800 controllers in this class.

#define AT2600JS_DISPLAY_NAME "Retro Joystik"
#define AT7800JS_DISPLAY_NAME "Atari 7800"

#define AT2600JS_UP_PIN   1
#define AT2600JS_DWN_PIN  2
#define AT2600JS_LFT_PIN  3
#define AT2600JS_RGT_PIN  4
#define AT2600JS_BTN_PIN  6
// #define AT2600JS_PWR_PIN  7
#define AT2600JS_BTN_2_PIN 7
#define AT2600JS_GRD_PIN  8

#define AT7800JS_RIGHT_BTN_PIN 5
#define AT7800JS_LEFT_BTN_PIN  9

// !!!
#define AT2600JS_UP_BTN_NUM   0
#define AT2600JS_RGT_BTN_NUM  1
#define AT2600JS_DWN_BTN_NUM  2
#define AT2600JS_LFT_BTN_NUM  3
#define AT2600JS_BTN_BTN_NUM  4
#define AT7800JS_LEFT_BTN_NUM  5
#define AT7800JS_RIGHT_BTN_NUM 6
// !!!
#define AT7800JS_BTN_JUICE_MIN_THRESHOLD 5
#define AT7800JS_BTN_JUICE_MAX_THRESHOLD 100

#define AT2600JS_BTN_NUM_1  0
#define AT2600JS_BTN_NUM_2  1
#define AT7800JS_BTN_LEFT   2
#define AT7800JS_BTN_RIGHT  3

Atari2600Joystick::Atari2600Joystick( PinSet pinNums, bool is7800 ) 
        : LegacyJoystick( is7800 ? AT7800JS_DISPLAY_NAME: AT2600JS_DISPLAY_NAME, pinNums ) {
    Atari2600Joystick::setupPins(pinNums);
    _7800Mode = is7800;

    if ( _7800Mode ) {
        setup7800OnlyPins(pinNums);
    }

    AT2600JS_BTN_PIN_LIST[AT2600JS_UP_BTN_NUM]  = AT2600JS_UP_PIN;
    AT2600JS_BTN_PIN_LIST[AT2600JS_RGT_BTN_NUM] = AT2600JS_RGT_PIN;
    AT2600JS_BTN_PIN_LIST[AT2600JS_DWN_BTN_NUM] = AT2600JS_DWN_PIN;
    AT2600JS_BTN_PIN_LIST[AT2600JS_LFT_BTN_NUM] = AT2600JS_LFT_PIN;
    AT2600JS_BTN_PIN_LIST[AT2600JS_BTN_BTN_NUM] = AT2600JS_BTN_PIN;

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

    pn  = joystickPinToArduinoPin( ardPinNums, AT2600JS_BTN_2_PIN );
    pinMode( pn, INPUT_PULLUP );

}

#define ZERO  512
#define MOVE 1024
#define ATARI_NUM_DIRS 4

struct AtariMove { short  pin; 
                   short axis; 
                   short  val; 
                 };
static const AtariMove dirs[ATARI_NUM_DIRS] =
    { { 1, Y_AXIS, 1024 }, // up
      { 2, Y_AXIS,    0 }, // down
      { 3, X_AXIS,    0 }, // left
      { 4, X_AXIS, 1024 }  // right
    };
      

static void Atari2600Joystick::setup7800OnlyPins( PinSet ardPinNums ) {
    short pn  = joystickPinToArduinoPin( ardPinNums, AT7800JS_LEFT_BTN_PIN );
    pinMode( pn, INPUT );

    pn  = joystickPinToArduinoPin( ardPinNums, AT7800JS_RIGHT_BTN_PIN );
    pinMode( pn, INPUT );
}

static bool Atari2600Joystick::checkFor7800( PinSet ardPinNums ) {
    setup7800OnlyPins(ardPinNums);

    int pn = joystickPinToArduinoPin( ardPinNums, AT7800JS_LEFT_BTN_PIN );
    int juiceLeft = analogRead(pn);
    pn = joystickPinToArduinoPin( ardPinNums, AT7800JS_RIGHT_BTN_PIN );
    int juiceRight = analogRead(pn);

    if ( juiceLeft < AT7800JS_BTN_JUICE_MAX_THRESHOLD && juiceRight < AT7800JS_BTN_JUICE_MAX_THRESHOLD ) {
        return juiceLeft>AT7800JS_BTN_JUICE_MIN_THRESHOLD  ||  juiceRight>AT7800JS_BTN_JUICE_MIN_THRESHOLD;
    }
    return false;
}

static Atari2600Joystick* Atari2600Joystick::checkForAtariJoystick( PinSet ardPinNums ) {
    setupPins(ardPinNums);
    // Determine if either fire button is pressed.
    int  pn  = joystickPinToArduinoPin( ardPinNums, AT2600JS_BTN_PIN );
    bool pressed = !digitalRead(pn);

    if ( pressed ) {
        // !!! check for 7800 btns.
        bool is7800 = checkFor7800( ardPinNums );
        return new Atari2600Joystick( ardPinNums, is7800 );
    }

    return NULL;

}

void Atari2600Joystick::jsStateToUsb() {
    // button(s)
    short pn = arduinoPinFor( AT2600JS_BTN_PIN );
    bool pressed = !digitalRead( pn );
    setBtnState(AT2600JS_BTN_NUM_1, pressed);

    pn = arduinoPinFor( AT2600JS_BTN_2_PIN );
    pressed = !digitalRead( pn );
    setBtnState(AT2600JS_BTN_NUM_2, pressed);

    // 7800 buttons are unique.
    if ( _7800Mode ) {
      int pn = arduinoPinFor( AT7800JS_LEFT_BTN_PIN );
      int juice = analogRead(pn);
      setBtnState( AT7800JS_LEFT_BTN_NUM, juice>AT7800JS_BTN_JUICE_MIN_THRESHOLD);

      pn = arduinoPinFor( AT7800JS_RIGHT_BTN_PIN );
      juice = analogRead(pn);
      setBtnState( AT7800JS_RIGHT_BTN_NUM, juice>AT7800JS_BTN_JUICE_MIN_THRESHOLD);
    }

    // DIRECTION
    short x = 512,
          y = 512;
    for ( int i=0;  i<ATARI_NUM_DIRS; i++ ) {
        AtariMove mv = dirs[i];       
        pn = arduinoPinFor( mv.pin );
        pressed = !digitalRead( pn );
        if (pressed) {
            if ( X_AXIS == mv.axis ) {
                x = mv.val;
            } else {
                y = mv.val;
            } // if ... else
        } // if
    } // for

    AxisState both[2] = { { X_AXIS, x}, {Y_AXIS, y} };
    setAxes( 2, both );
} // fn