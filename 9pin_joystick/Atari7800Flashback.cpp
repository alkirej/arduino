// #include "pins_arduino.h"
#include "Atari7800Flashback.h"

#include <Logger.h>

#define AT7800FB_DISPLAY_NAME "Atari 7800 Flashback"

#define AT7800FB_BTN_1      0
#define AT7800FB_BTN_2      1
#define AT7800FB_BTN_PAUSE  2
#define AT7800FB_BTN_SELECT 3
#define AT7800FB_UP         4
#define AT7800FB_DOWN       5
#define AT7800FB_RIGHT      6
#define AT7800FB_LEFT       7

#define ZERO  512
#define POS  1024
#define NEG     0

#define UP    4
#define DOWN  5
#define RIGHT 7
#define LEFT  6

#define SHIFT_REG_PIN_COUNT 8
#define PWR_PIN 7
#define GRD_PIN 8
#define LATCH_PIN 3
#define CLOCK_PIN 4
#define DATA_PIN  2

Atari7800Flashback::Atari7800Flashback( PinSet pinNums ) : LegacyJoystick( AT7800FB_DISPLAY_NAME, pinNums ) {
    // _shReg.setup(8,5,4,11,10,12);
    _shReg.setup( SHIFT_REG_PIN_COUNT,
                  arduinoPinFor( PWR_PIN ),
                  arduinoPinFor( GRD_PIN ),
                  arduinoPinFor( LATCH_PIN ),
                  arduinoPinFor( CLOCK_PIN ),
                  arduinoPinFor( DATA_PIN )
                );
}

static Atari7800Flashback* Atari7800Flashback::checkForAtari7800Flashback( PinSet ardPinNums ) {
    ShiftRegister sr = ShiftRegister();
    sr.setup(8, 5, 4, 11, 10, 12);
    sr.readPins();

    // We found our joystick.
    if ( sr.isPressed(0) ) {
        return new Atari7800Flashback( ardPinNums );
    }

    return NULL;
}

void Atari7800Flashback::jsStateToUsb() {
    // Buttons:  (0) "fire" left  (1) "fire" right  (2) Pause  (3) Select
    _shReg.readPins();
    for ( int i=0; i<4; i++ ) {
        setBtnState(i, _shReg.isPressed(i) );
    }

    // x & y axis
    short x,y;

    if ( _shReg.isPressed(UP) ) {
        y = POS;
    } else if ( _shReg.isPressed(DOWN) ) {
        y = NEG;
    } else {
        y = ZERO;
    }

    if ( _shReg.isPressed(RIGHT) ) {
        x = POS;
    } else if ( _shReg.isPressed(LEFT) ) {
        x = NEG;
    } else {
        x = ZERO;
    }

    AxisState axes[2] = { {X_AXIS, x}, {Y_AXIS, y} };
    setAxes( 2, axes );
}
