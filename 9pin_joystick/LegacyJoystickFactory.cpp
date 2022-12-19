#include "LegacyJoystick.h"
#include "LegacyJoystickFactory.h"

#include "Atari2600Keypad.h"
#include "Atari2600Paddles.h"
#include "Atari2600Joystick.h"
// #include "Atari2600/Atari7800Flashback.h"
#include "Intellivision.h"

#define FIRST_BUTTON_PORT_1 0
#define FIRST_BUTTON_PORT_2 15

static bool p1_exists = false,
            p2_exists = false;

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

static bool jsSetupComplete = false;

static void LegacyJoystickFactory::initialize() {
    if ( !jsSetupComplete ) {
        JOYSTICK_OBJECT.begin();
        JOYSTICK_OBJECT.setXAxisRange(LO_AXIS_VALUE, HI_AXIS_VALUE);
        JOYSTICK_OBJECT.setYAxisRange(LO_AXIS_VALUE, HI_AXIS_VALUE);
        JOYSTICK_OBJECT.setZAxisRange(LO_AXIS_VALUE, HI_AXIS_VALUE);

        JOYSTICK_OBJECT.setRxAxisRange(LO_AXIS_VALUE, HI_AXIS_VALUE);
        JOYSTICK_OBJECT.setRyAxisRange(LO_AXIS_VALUE, HI_AXIS_VALUE);
        JOYSTICK_OBJECT.setRzAxisRange(LO_AXIS_VALUE, HI_AXIS_VALUE);

        JOYSTICK_OBJECT.setRudderRange(LO_AXIS_VALUE, HI_AXIS_VALUE);
        JOYSTICK_OBJECT.setThrottleRange(LO_AXIS_VALUE, HI_AXIS_VALUE);
        JOYSTICK_OBJECT.setAcceleratorRange(LO_AXIS_VALUE, HI_AXIS_VALUE);
        JOYSTICK_OBJECT.setBrakeRange(LO_AXIS_VALUE, HI_AXIS_VALUE);
        JOYSTICK_OBJECT.setSteeringRange(LO_AXIS_VALUE, HI_AXIS_VALUE);
    }
}

static LegacyJoystick* LegacyJoystickFactory::lookforNewJoystick( PinSet pinNums ) {
    if ( p2_exists ) return NULL;

    initialize();
    LegacyJoystick* newJs = NULL;

    if ( !newJs ) {
      newJs = Intellivision::checkForIntellivision( pinNums );
    }

    if ( !newJs ) {
      // newJs = Atari7800Flashback::checkForAtari7800Flashback( pinNums );
    }
    if ( !newJs ) {
      newJs = Atari2600Joystick::checkForAtariJoystick( pinNums );
    }
    if ( !newJs ) {
      newJs = Atari2600Keypad::checkForAtari2600Keypad( pinNums );
    }
    if ( !newJs ) {
      newJs = Atari2600Paddles::checkForAtari2600Paddles( pinNums );
    }

    if ( newJs ) {
        short firstBtn=FIRST_BUTTON_PORT_1;
        if ( p1_exists ) firstBtn=FIRST_BUTTON_PORT_2;
        newJs->setup( JOYSTICK_OBJECT, firstBtn );
        if ( p1_exists ) p2_exists=true; 
        else p1_exists=true;
    }

    return newJs;
}

static void resetAllPorts() {
    p1_exists=false;
    p2_exists=false;
}


LegacyJoystickFactory::LegacyJoystickFactory() {}