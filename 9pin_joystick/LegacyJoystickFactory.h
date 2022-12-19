#include "LegacyJoystick.h"

#ifndef LEGACY_JOYSTICK_FACTORY_H
#define LEGACY_JOYSTICK_FACTORY_H

class LegacyJoystickFactory {
    public:
        static LegacyJoystick* lookforNewJoystick( PinSet pinNums );
        static void resetAllPorts();  // DON'T FORGET TO FREE SPACE FOR LEGACY_JOYSTICK OBJECTS IN EXISTANCE.

    private:
        LegacyJoystickFactory();

        static void initialize();

}; // LegacyJoystickFactory

#endif