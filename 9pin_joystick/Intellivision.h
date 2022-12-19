#include "LegacyJoystick.h"

#ifndef INTELLIVISION_H
#define INTELLIVISION_H

#define INTV_PIN_CNT  9

class Intellivision : public LegacyJoystick {
    public:
        static void setupPins( PinSet ardPinNums );
        static Intellivision* checkForIntellivision( PinSet ardPinNums );

        Intellivision( PinSet pinNums );  
    
    private:
        static PinState Intellivision::scanPins( PinSet pinNums );
        static PinState Intellivision::buildPinState( bool states[INTV_PIN_CNT-1]);

        void jsStateToUsb() override;
};

#endif