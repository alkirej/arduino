#include "LegacyJoystick.h"
#include "ShiftRegister.h"

#ifndef ATARI_7800_FLASHBAK_H
#define ATARI_7800_FLASHBACK_H

class Atari7800Flashback : public LegacyJoystick {
    public:
        static void setupPins( PinSet ardPinNums );
        static Atari7800Flashback* checkForAtari7800Flashback( PinSet ardPinNums );

        Atari7800Flashback( PinSet pinNums );

    private:
        void jsStateToUsb() override;

        ShiftRegister _shReg = ShiftRegister();
};

#endif
