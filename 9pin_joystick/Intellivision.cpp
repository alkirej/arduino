#include "Intellivision.h"

#include <Logger.h>


#define INTV_GND_PIN  5

#define INTV_DISPLAY_NAME "Intellivision"
#define F false
#define T true

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
    scanPins(ardPinNums);
    return NULL;
}

static PinState Intellivision::buildPinState( bool states[INTV_PIN_CNT-1]) {
    unsigned int allStates=0;
    for (int i=0;  i<INTV_PIN_CNT-1; i++ ) {
        int pinVal = states[i] ? 1 : 0;
        allStates <<= 1;
        allStates += pinVal;
    }

logger.log("pin state:").log(allStates).log("  ").logln((char)allStates);

    return (char) allStates;
}

static PinState Intellivision::scanPins( PinSet pinNums ) {
    bool states[INTV_PIN_CNT-1] = {false,false,false,false, false,false,false,false};
    for ( int i=1;  i<=INTV_PIN_CNT-1;  i++ ) {
        if ( INTV_GND_PIN != i ) {
            short pn = joystickPinToArduinoPin(pinNums, i);
            states[i] = !digitalRead( pn );
        } // if
    } // for

    return buildPinState(states);
} // fn


void Intellivision::jsStateToUsb() {
  // !!! COMING SOON !!!
}