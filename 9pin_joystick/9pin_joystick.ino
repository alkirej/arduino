#include <Logger.h>

#include "LegacyJoystick.h"
#include "LegacyJoystickFactory.h"

short PORT_1_PINS[9] = { 13u,  12,  11,  10, 9, 6, 5, 4, 3 };

const short RESET_BUTTON = {0};

LegacyJoystick* playerOne = NULL;
LegacyJoystick* playerTwo = NULL;

Logger logger( (HardwareSerial *) &Serial, true);

void setup() {
    // Serial.begin(9600);
    logger.logln( "Begin Setup of Legacy 9-pin joystick port.");

    while ( !playerOne && !playerTwo ) {
        playerOne = LegacyJoystickFactory::lookforNewJoystick( PORT_1_PINS );
        delay(2);
    }

    logger.log( "   ---> ").logln( playerOne->getControllerName() );
    logger.logln( "Setup complete.");
}

void loop() {
    // put your main code here, to run repeatedly:
    if ( playerOne ) playerOne->jsStateToUsb();
    if ( playerTwo ) playerTwo->jsStateToUsb();

    // Occassional check for new js
    // Monitor reset button and force verification
    //    and freeing up memory for the playerOne and/or playerTwo objects (memory restore
    //    will occur in the reset button code, not here.).

    delay(2);
}
