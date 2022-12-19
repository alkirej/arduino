#include "LegacyJoystick.h"

#ifndef ATARI_2600_KEYPAD_H
#define ATARI_2600_KEYPAD_H

#define AT2600KP_NUM_ROWS 4
#define AT2600KP_NUM_COLS 3


class Atari2600Keypad : public LegacyJoystick {
    public:
        static void setupPins( PinSet ardPinNums );
        static Atari2600Keypad* checkForAtari2600Keypad( PinSet ardPinNums );

        Atari2600Keypad( PinSet pinNums );  
    
    private:

        static bool checkForPressedPound( PinSet ardPinNums );

        void examinOneRowOfButtons( short row );
        void jsStateToUsb() override;
};

#endif
