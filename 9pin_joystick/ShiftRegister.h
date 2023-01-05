#include <Arduino.h>

#ifndef SHIFT_REGISTER_H
#define SHIFT_REGISTER_H

class ShiftRegister {
    public:
        ShiftRegister( short pinCount,
                       short pwrPin,
                       short grdPin,
                       short latchPin,
                       short clockPin,
                       short dataPin
                     );
        bool* readPins();
        bool  isPressed( short idx );

    private:
        void setupPins();

        void pulseLatch();
        void pulseClock();
        bool readDataPin();

        short _pinCount;

        short _pwrPin;
        short _grdPin;
        short _latchPin;
        short _clockPin;
        short _dataPin;

        bool* _pinStates;
};

#endif
