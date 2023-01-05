#include "LegacyJoystick.h"

#ifndef TI994A_JOYSTICK_H
#define TI994A_JOYSTICK_H

class TI994aJoystick : public LegacyJoystick {
    public:
        static void setupPins( PinSet ardPinNums );
        static TI994aJoystick* checkForTI994aJoysticks( PinSet ardPinNums );

        inline bool isLeftJoystick() { return _leftJs; };

    private:
        TI994aJoystick( PinSet ardPinNums, bool leftJoystick );

        void jsStateToUsb() override;

        bool _leftJs;
};

#endif
