#include "LegacyJoystick.h"

#ifndef TI994A_JOYSTICK_H
#define TI994A_JOYSTICK_H

class TI994aJoystick : public LegacyJoystick {
    public:
        static void setupPins( PinSet ardPinNums );
        static TI994aJoystick* checkForTI994aJoystick( PinSet ardPinNums, bool leftJoystick );

    private:
        TI994aJoystick( PinSet ardPinNums );

        void jsStateToUsb() override;
};

#endif
