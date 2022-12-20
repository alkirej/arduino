#include "LegacyJoystick.h"
#include <Logger.h>

#define DEBOUNCE_PERIOD 10

// CONSTRUCTOR
LegacyJoystick::LegacyJoystick( char* name, PinSet pinSet ) {
      _controllerName = name;

      _btnState = new bool[MAX_BUTTONS_PER_CONTROLLER];
      for ( short i=0; i<MAX_BUTTONS_PER_CONTROLLER; i++ ) {
          _btnState[i] = false;
      }

      for ( short i=0; i<PINS_PER_CONTROLLER; i++ ) {
        _pins[i] = pinSet[i];
      }
      _debounceStart = millis();

} // LegacyJoystick::LegacyJoystick

// GETTERS
char* LegacyJoystick::getControllerName() { return _controllerName; }

// SETUP ROUTINE.  MUST BE CALLED WHEN LEGACY_JOYSTICK CHILD IS CREATED (in the factory)
void LegacyJoystick::setup( Joystick_ controller, short firstBtn ) {
    _btnZero = firstBtn-1;
    _controller = controller;

    // update lcd here !!!
}

void LegacyJoystick::jsStateToUsb() { 
    logger.logln("IN LEGACY JOYSTICK BASE CLASS. THIS IS NOT GOOD."); 
}

void LegacyJoystick::startDebounce() {
    _debounceStart = millis();
}

// DEBOUNCE
inline bool LegacyJoystick::inDebounce() {
    return ( millis() < _debounceStart+DEBOUNCE_PERIOD );
}

void LegacyJoystick::setBtnState( short btnIdx, bool newState ) {
    bool oldState = _btnState[btnIdx];
    if ( !inDebounce() && oldState != newState ) {
        startDebounce();
        _controller.setButton( btnNumToReport(btnIdx+1), newState );
        _btnState[btnIdx] = newState;
    }
}

void LegacyJoystick::setAllBtnStates( short count, ButtonState btns[] ) {
    if ( !inDebounce() ) {
        bool change = false;
        for ( int i=0;  i<count;  i++ ) {
          bool crtChange = setSingleBtnState( btns[i].btnNbr, btns[i].pressed );
          if ( !change ) {
              change = crtChange;
          }
        } // for

        if ( change ) {
          startDebounce();
        } 
    } // if
}

bool LegacyJoystick::setSingleBtnState( short btnIdx, bool newState ) {
    bool oldState = _btnState[btnIdx];
    if ( oldState != newState ) {
        _controller.setButton( btnNumToReport(btnIdx+1), newState );
        _btnState[btnIdx] = newState;
    }

    return oldState != newState;
}

void LegacyJoystick::updateButtonStates( short count, short pinNbr[], short btnNbr[] ) {
    if ( !inDebounce() ) {
        bool change = false;
        for ( int i=0;  i<count; i++ ) {
            short pn = arduinoPinFor( pinNbr[i] );
            bool pressed = !digitalRead( pn );
            bool crtChange = setSingleBtnState(btnNbr[i], pressed);
            if ( !change ) {
                change = crtChange;
            }
        } // for

        if ( change ) {
            startDebounce();
        } // if
    } // if !inDebounce()
} // updateBtnState fn

void LegacyJoystick::setAxisTo( short axisNbr, int16_t value ) {
    int16_t scaledValue = value;
    switch (axisNbr)  {
        case X_AXIS:
            _controller.setXAxis( scaledValue );
            break;
        case Y_AXIS:
            _controller.setYAxis( scaledValue );
            break;
        case Z_AXIS:
            _controller.setZAxis( scaledValue );
            break;

        case RX_AXIS:
            _controller.setRxAxis( scaledValue );
            break;
        case RY_AXIS:
            _controller.setRyAxis( scaledValue );
            break;
        case RZ_AXIS:
            _controller.setRzAxis( scaledValue );
            break;

        case RUDDER:
            _controller.setRudder( scaledValue );
            break;
        case THROTTLE:
            _controller.setThrottle( scaledValue );
            break;
        case ACCEL:
            _controller.setAccelerator( scaledValue );
            break;
        case BRAKE:
            _controller.setBrake( scaledValue );
            break;
        case STEERING:
            _controller.setSteering( scaledValue );
            break;

    } // switch
}

void LegacyJoystick::setAxes( short count, AxisState axes[] ) {
    for ( int i=0;  i<count;  i++ ) {
        AxisState crt = axes[i];
        setAxisTo( crt.axis, crt.loc );
    }
}
