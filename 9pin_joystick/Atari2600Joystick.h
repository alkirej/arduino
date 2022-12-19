#include "LegacyJoystick.h"

#ifndef ATARI_2600_JOYSTICK_H
#define ATARI_2600_JOYSTICK_H

class Atari2600Joystick : public LegacyJoystick {
    public:
        static const short AT2600JS_BTN_COUNT = 5;

        static void setupPins( PinSet ardPinNums );
        static Atari2600Joystick* checkForAtariJoystick( PinSet ardPinNums );
    
    private:
        static void setup7800OnlyPins( PinSet ardPinNums );
        static bool checkFor7800( PinSet ardPinNums );

        short AT2600JS_BTN_PIN_LIST[ AT2600JS_BTN_COUNT ];

        Atari2600Joystick( PinSet ardPinNums, bool is7800 );

        void jsStateToUsb() override;
        bool _7800Mode;
};

#endif
