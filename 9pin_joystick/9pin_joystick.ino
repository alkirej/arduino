#include <Logger.h>

#include "LegacyJoystick.h"
#include "LegacyJoystickFactory.h"

#include "ShiftRegister.h" // !!!

// !!! del start
#include "Atari2600Paddles.h" // !!!

static const Joystick_ JOYSTICK_OBJECT
                  = Joystick_(
                       JOYSTICK_DEFAULT_REPORT_ID,
                       JOYSTICK_TYPE_JOYSTICK,
                       MAX_BUTTONS_PER_CONTROLLER, // ? buttons
                       0,     // no hat switches,
                       true, // x axis
                       true,  // y axis
                       false, // z axis
                       false, // Rx axis
                       false, // Ry axis
                       false, // Rz axis
                       false, // rudder
                       false, // throttle
                       false,  // accelerator
                       false, // brake
                       false  // steering
                     );
// !!! del end

short PORT_1_PINS[9] = { 13,  12,  11,  10, 9, 6, 5, 4, 3 };

const short RESET_BUTTON = {0};

LegacyJoystick* playerOne = NULL;
LegacyJoystick* playerTwo = NULL;

Logger logger( (HardwareSerial *) &Serial, true);

// !!!
ShiftRegister sr = ShiftRegister(8, 5, 4, 11, 10, 12);
// !!!

void setup() {
    Serial.begin(9600);
    delay(2500);
    logger.logln( "Begin Setup of Legacy 9-pin joystick port.");


//    while ( !playerOne ) {
//        playerOne = LegacyJoystickFactory::lookforNewJoystick( PORT_1_PINS );
//        delay(2);
//    }

//    logger.log( "   ---> ").logln( playerOne->getControllerName() );
    logger.logln( "Setup complete.");
}

void loop() {
    // put your main code here, to run repeatedly:
//    if ( playerOne ) playerOne->jsStateToUsb();
//    if ( playerTwo ) playerTwo->jsStateToUsb();

    // Occassional check for new js
    // Monitor reset button and force verification
    //    and freeing up memory for the playerOne and/or playerTwo objects (memory restore
    //    will occur in the reset button code, not here.).

    delay(1);

// !!!
bool* st = sr.readPins();
for ( int i=0; i<8; i++ ) {
    logger.log(".");
    if ( st[i] ) {
        logger.log(" Button ").log(i+1).log("pressed ");
    }
}
logger.logln();
// !!!
}
