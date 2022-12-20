#include <Joystick.h>

#ifndef LEGACY_JOYSTICK_H
#define LEGACY_JOYSTICK_H

#define PINS_PER_CONTROLLER 9

#define LO_AXIS_VALUE -1024 //-32767
#define HI_AXIS_VALUE  1024 // 32767

#define ANALOG_PIN_MIN    0
#define ANALOG_PIN_MAX 1023

// 32767 - (-32767) + 1 = distance in axis values (65,535)
// 1023 - 0 + 1 = distance in analog pins (1,024)
// 65,535 / 1,024 = 63.9990234375
//#define AXIS_CONVERSION_FACTOR 63.9990234375
#define AXIS_CONVERSION_FACTOR 64

// 15 for port 1 and 15 for port 2
#define MAX_BUTTONS_PER_CONTROLLER 30

#define X_AXIS    1
#define Y_AXIS    2
#define Z_AXIS    3
#define RX_AXIS   4
#define RY_AXIS   5
#define RZ_AXIS   6
#define RUDDER    7
#define THROTTLE  8
#define ACCEL     9
#define BRAKE    10
#define STEERING 11

struct ButtonState {
    short btnNbr;
    bool  pressed;
};
struct AxisState {
    short   axis;
    int16_t loc;
};

typedef short PinSet[PINS_PER_CONTROLLER];
typedef unsigned char PinState;

class LegacyJoystick {
    public:
      static short inline joystickPinToArduinoPin( PinSet ardPinNums, short at2600kpPinNum ) {
          return ardPinNums[ at2600kpPinNum-1 ];
      }

      char* getControllerName();

      inline short arduinoPinFor( short atariPin ) { return _pins[atariPin-1]; }
      virtual void jsStateToUsb();

      void setup(Joystick_ controller, short firstBtn );

    protected:
      // Data points
      char*     _controllerName;
      Joystick_ _controller;
      unsigned long _debounceStart;
      bool*     _btnState;
      short     _btnZero;
      PinSet    _pins;

      // private constructor
      LegacyJoystick( char* controllerName, PinSet pinSet );

      inline short btnNumToReport( short btnIdx ) { return btnIdx + _btnZero; }

      // debounce routines.
      void startDebounce();
      bool inDebounce();

      void setAllBtnStates( short count, ButtonState btns[] );
      void updateButtonStates( short btnCount, short pinNbr[], short btnNbr[] );
      bool setSingleBtnState(short pinNbr, bool newState);

      void setBtnState( short btnIdx, bool newState );

      void setAxisTo( short axisNbr, int16_t value );
      void setAxes( short count, AxisState axes[] );

      int16_t arduinoPotToJoystickAxis( int16_t potValue );
}; // class Legacy Joystick

#endif