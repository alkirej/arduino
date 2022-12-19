#include "LegacyJoystick.h"

#ifndef ATARI_2600_PADDLES_H
#define ATARI_2600_PADDLES_H

class Atari2600Paddles : public LegacyJoystick {
    public:
        static const short AT2600PDL_BTN_COUNT = 2;

        static void setupPins( PinSet ardPinNums );
        static Atari2600Paddles* checkForAtari2600Paddles( PinSet ardPinNums );

        Atari2600Paddles( PinSet pinNums );  
    
    private:
        short AT2600PDL_BTN_PIN_LIST[ AT2600PDL_BTN_COUNT ];

        void jsStateToUsb() override;
};

#endif
