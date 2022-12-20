#include "Intellivision.h"

#include <Logger.h>


#define INTV_GND_PIN  5

#define INTV_DISPLAY_NAME "Intellivision"
#define INTV_NUM_FIRE_BTNS 3
#define INTV_NUM_KEYPAD_BTNS 12
#define INTV_NUM_AXES  2
#define INTV_NUM_DIRS 17

#define F false
#define T true

#define ID_BTN_NUM 14
#define FIRE_BTN_MASK B00001110
#define MOVEMENT_MASK B11110001

struct IntvButton { PinState ps; short btnNbr; };
static const IntvButton FIRE_BUTTONS[INTV_NUM_FIRE_BTNS] = 
    { { B00001010,  0 }, // top fire
      { B00001100,  1 }, // bottom left fire
      { B00000110,  2 }, // bottom right fire
    };
static const IntvButton KEYPAD_BUTTONS[INTV_NUM_KEYPAD_BTNS] = 
    { { B00011000,  3 }, // 1
      { B00010100,  4 }, // 2
      { B00010010,  5 }, // 3
      { B00101000,  6 }, // 4
      { B00100100,  7 }, // 5
      { B00100010,  8 }, // 6
      { B01001000,  9 }, // 7
      { B01000100, 10 }, // 8
      { B01000010, 11 }, // 9
      { B10001000, 12 }, // clear
      { B10000100, 13 }, // 0
      { B10000010, 14 } // enter
    };

static void Intellivision::setupPins( PinSet ardPinNums ) {
    for ( int i=1;  i<=INTV_PIN_CNT;  i++ ) {
        short pn = joystickPinToArduinoPin( ardPinNums, i );
        if ( i==INTV_GND_PIN ) {
            pinMode( pn, OUTPUT );
            digitalWrite( pn, LOW );

        } else {
            pinMode( pn, INPUT_PULLUP );
        }
    } // for
} // fn.

Intellivision::Intellivision( PinSet pinNums ) : LegacyJoystick( INTV_DISPLAY_NAME, pinNums ) {
    setupPins(pinNums);
}

static Intellivision* Intellivision::checkForIntellivision( PinSet ardPinNums ) {
    setupPins(ardPinNums);
    PinState st = scanPins(ardPinNums);
    if ( st == KEYPAD_BUTTONS[10].ps ) {
        return new Intellivision(ardPinNums);
    }

    return NULL;
}

static PinState Intellivision::buildPinState( bool states[INTV_PIN_CNT-1]) {
    unsigned int allStates=0;
    for (int i=0;  i<INTV_PIN_CNT-1; i++ ) {
        int pinVal = states[i] ? 1 : 0;
        allStates <<= 1;
        allStates += pinVal;
    }

    return (PinState) allStates;
}

static PinState Intellivision::scanPins( PinSet pinNums ) {
    bool states[INTV_PIN_CNT-1] = {false,false,false,false, false,false,false,false};

    for ( int i=1;  i<=INTV_PIN_CNT;  i++ ) {
        short pn = joystickPinToArduinoPin(pinNums, i);
        if ( i < INTV_GND_PIN ) {
            states[i-1] = !digitalRead( pn );
        } else if ( i > INTV_GND_PIN ) { 
            states[i-2] = !digitalRead(pn);
        } // if
    } // for

    return buildPinState(states);
} // fn


#define ZERO     0
#define FULL  1024 // 32767
#define HALF   512 // 16384

struct IntvMove { PinState ps; 
                  short     x; 
                  short     y; 
                };
static const IntvMove dirs[INTV_NUM_DIRS] =
    { { B00000000,  ZERO,  ZERO }, // centered.
      
      { B01000000,  ZERO, -FULL }, // N
      { B01000001,  HALF, -FULL }, // N x NE
      { B01100001,  FULL, -FULL }, // NE
      { B01100000,  FULL, -HALF }, // E x NE

      { B00100000,  FULL,  ZERO }, // E
      { B00100001,  FULL,  HALF }, // E x SE
      { B00110001,  FULL,  FULL }, // SE
      { B00110000,  HALF,  FULL }, // S x SE

      { B00010000,  ZERO,  FULL }, // S
      { B00010001, -HALF,  FULL }, // S x SW
      { B10010001, -FULL,  FULL }, // SW
      { B10010000, -FULL,  HALF }, // W x SW

      { B10000000, -FULL,  ZERO }, // W
      { B10000001, -FULL, -HALF }, // W x NW
      { B11000001, -FULL, -FULL }, // NW
      { B11000000, -HALF, -FULL }  // N x NW
    };

void Intellivision::jsStateToUsb() {
    PinState ps = scanPins(_pins);
    ButtonState fireButtons[INTV_NUM_FIRE_BTNS],
                keypButtons[INTV_NUM_KEYPAD_BTNS];

    for ( int i=0;  i<INTV_NUM_FIRE_BTNS; i++ ) {
        PinState fireOnly = ps & FIRE_BTN_MASK;
        fireButtons[i] = { FIRE_BUTTONS[i].btnNbr, fireOnly==FIRE_BUTTONS[i].ps };
    }
    setAllBtnStates( INTV_NUM_FIRE_BTNS, fireButtons);

    for ( int i=0;  i<INTV_NUM_KEYPAD_BTNS; i++ ) {
        keypButtons[i] = { KEYPAD_BUTTONS[i].btnNbr, ps==KEYPAD_BUTTONS[i].ps };
    }
    setAllBtnStates( INTV_NUM_KEYPAD_BTNS, keypButtons);

    for ( int j=0;  j<INTV_NUM_DIRS;  j++ ) {
        PinState moveOnly = ps & MOVEMENT_MASK;
        IntvMove dir=dirs[j];
        if ( moveOnly==dir.ps ) {
            AxisState both[2] = { {X_AXIS,dir.x}, 
                                  {Y_AXIS,dir.y} 
                                };
            setAxes( 2, both );
        }
    }
}