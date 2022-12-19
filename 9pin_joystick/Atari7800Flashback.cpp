#include "pins_arduino.h"
#include "Atari7800Flashback.h"

#include <Logger.h>

#define AT7800FB_DISPLAY_NAME "Atari 7800 Flashback"
#define AT2600JS_UP_PIN   1
#define AT2600JS_DWN_PIN  2
#define AT2600JS_RGT_PIN  3
#define AT2600JS_LFT_PIN  4
#define AT2600JS_BTN_PIN  6
#define AT2600JS_GRD_PIN  8

#define AT2600JS_UP_BTN_NUM   0
#define AT2600JS_DWN_BTN_NUM  1
#define AT2600JS_RGT_BTN_NUM  2
#define AT2600JS_LFT_BTN_NUM  3
#define AT2600JS_BTN_BTN_NUM  4


Atari7800Flashback::Atari7800Flashback( PinSet pinNums ) : LegacyJoystick( AT7800FB_DISPLAY_NAME, pinNums ) {
    setupPins(pinNums);
}

static void Atari7800Flashback::setupPins( PinSet ardPinNums ) {
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
}

static Atari7800Flashback* Atari7800Flashback::checkForAtari7800Flashback( PinSet ardPinNums ) {
    // !!! Not implemented.
    return NULL;
}

void Atari7800Flashback::jsStateToUsb() {
    // !!! Not implemented yet.
}