#include "ShiftRegister.h"
#include <Logger.h>

ShiftRegister::ShiftRegister() { }

void ShiftRegister::setup( short pinCount,
                           short pwrPin,
                           short grdPin,
                           short latchPin,
                           short clockPin,
                           short dataPin
                         ) {
    _pinCount = pinCount;

    _pwrPin   = pwrPin;
    _grdPin   = grdPin;
    _latchPin = latchPin;
    _clockPin = clockPin;
    _dataPin  = dataPin;

    _pinStates = new bool[_pinCount];

    setupPins();
}

void ShiftRegister::setupPins() {
  // data input pin
  pinMode(_dataPin, INPUT);

  // clock and latch pins
  pinMode(_clockPin, OUTPUT);
  pinMode(_latchPin, OUTPUT);

  digitalWrite(_clockPin, LOW);
  digitalWrite(_latchPin, LOW);

  // power and ground
  pinMode(_pwrPin, OUTPUT);
  pinMode(_grdPin, OUTPUT);

  digitalWrite(_pwrPin, HIGH);
  digitalWrite(_grdPin, LOW);
}

void ShiftRegister::pulseLatch() {
    // pulse the latch pin
    digitalWrite( _latchPin, HIGH );
    digitalWrite( _latchPin, LOW );
}

void ShiftRegister::pulseClock() {
    // pulse clock.
    digitalWrite( _clockPin, HIGH );
    digitalWrite( _clockPin, LOW );
}

bool ShiftRegister::readDataPin() {
    return !digitalRead( _dataPin );
}


bool* ShiftRegister::readPins() {
    for ( int i=0; i<_pinCount; i++ ) {
        if ( 0 == i ) pulseLatch();
        else pulseClock();

        _pinStates[i] = readDataPin();
    }

    return _pinStates;
}

bool ShiftRegister::isPressed(short idx) {
    return _pinStates[idx];
}
