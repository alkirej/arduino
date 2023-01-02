#include "LegacyJoystick.h"

#ifndef ATARI_2600_JOYSTICK_H
#define ATARI_2600_JOYSTICK_H

class Atari2600Joystick : public LegacyJoystick {
    public:
        static void setupPins( PinSet ardPinNums );
        static Atari2600Joystick* checkForAtariJoystick( PinSet ardPinNums );

    private:
        Atari2600Joystick( PinSet ardPinNums );

        void jsStateToUsb() override;
        void drvPdlStateToUsb();

        bool _drvPdl=false;
};

#endif
