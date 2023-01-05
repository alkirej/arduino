#include <Logger.h>
Logger logger( (HardwareSerial *) &Serial, true);

short PINS[9] = { 13,  12,  11,  10, 9, 6, 5, 4, 3 };

short lp=3,
      cp=4,
      dp=2,
      pp=7,
      gp=8;

short latchPin=PINS[lp-1],
      clockPin=PINS[cp-1],
      dataPin =PINS[dp-1],
      pwrPin  =PINS[pp-1],
      grdPin  =PINS[gp-1];

bool gPinState[9] = { false, false, false,
                      false, false, false,
                      false, false, false
                    };

void latch() {
  digitalWrite(latchPin, HIGH);
  delayMicroseconds(1);
  digitalWrite(latchPin, LOW);
}


void setup() {
  delay(2500);

  // data input pin
  pinMode(dataPin, INPUT);
  
  // clock and latch pins
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  
  digitalWrite(clockPin, LOW);
  digitalWrite(latchPin, LOW);

  // power and ground
  pinMode(pwrPin, OUTPUT);
  pinMode(grdPin, OUTPUT);
  
  digitalWrite(pwrPin, HIGH);
  digitalWrite(grdPin, LOW);

  logger.logln("===== TIME TO START ====");
}

void clock() {
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
}

bool* pinStates() {
  for ( int i=0; i<9; i++ ) {
    gPinState[i]=digitalRead(PINS[i]);
  }
  return gPinState;
}

short loopCt = 0;
void loop() {
  for ( int i=0; i<8; i++ ) {
    if (0==i) {
      latch();
    } else {
      clock();
    }
    bool val = !digitalRead(dataPin);
    logger.log(val);
    if (3==i) logger.log(" ");
  }
  logger.logln();
  delay(250);
}

/*

int nesData  = 12;
int nesClock = 10;
int nesLatch = 11;
int power    =  5;
int ground   =  4;

void setup() {
// Initialize serial port speed for the serial terminal
  Serial.begin(9600);
  
  // Set appropriate pins to inputs
  pinMode(nesData, INPUT);
  
  // Set appropriate pins to outputs
  pinMode(nesClock, OUTPUT);
  pinMode(nesLatch, OUTPUT);
  
  // Set initial states
  digitalWrite(nesClock, LOW);
  digitalWrite(nesLatch, LOW); 


  pinMode(power, OUTPUT);
  digitalWrite(power, HIGH);
  pinMode(ground, OUTPUT );
  digitalWrite(ground, LOW );
}

void loop() {
  delay(100);

  logger.logln("===== LATCHING =====");
  digitalWrite(nesLatch, HIGH);
  digitalWrite(nesLatch, LOW);

  if (!digitalRead(nesData)) {
    logger.logln("button 1 pressed.");
  }

  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (!digitalRead(nesData) ) {
    logger.logln("button 2 pressed.");
  }

  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (!digitalRead(nesData) ) {
    logger.logln("button 3 pressed.");
  }

  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (!digitalRead(nesData) ) {
    logger.logln("button 4 pressed.");
  }

  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (!digitalRead(nesData) ) {
    logger.logln("button 5 pressed.");
  }
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (!digitalRead(nesData) ) {
    logger.logln("button 6 pressed.");
  }

  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (!digitalRead(nesData) ) {
    logger.logln("button 7 pressed.");
  }

  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (!digitalRead(nesData) ) {
    logger.logln("button 8 pressed.");
  }

}
*/