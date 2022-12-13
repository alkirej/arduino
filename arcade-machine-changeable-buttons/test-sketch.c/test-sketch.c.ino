#include <HID-Project.h>
#include <LiquidCrystal_I2C.h>

#define DEBUG_MODE true


#define PIN_VOLUME_DIAL_PUSH_TO_MUTE 4
#define VOLUME_PIN_1 6
#define VOLUME_PIN_2 7
#define SELECT_PIN 8
#define BACK_PIN 9
#define DEBOUNCE_DELAY 10ul
#define MSG_DURATION 3000ul
#define DELTA_BETWEEN_SCANS 2ul

#define UP_ARROW_CHAR 1
#define DN_ARROW_CHAR 2
#define LT_ARROW_CHAR 3
#define RT_ARROW_CHAR 4

#define LCD_UPPER_ROW 0
#define LCD_LOWER_ROW 1
#define LCD_LEFT true
#define LCD_RIGHT false

// ML = Message Location
#define ML_MUTE_ROW LCD_UPPER_ROW
#define ML_MUTE_SIDE LCD_RIGHT

#define ML_VOL_ROW LCD_UPPER_ROW
#define ML_VOL_SIDE LCD_LEFT

#define ML_SEL_ROW LCD_LOWER_ROW
#define ML_SEL_SIDE LCD_LEFT

#define ML_BACK_ROW LCD_LOWER_ROW
#define ML_BACK_SIDE LCD_RIGHT

//
#define MUTE_ARROW LT_ARROW_CHAR
#define VOL_ARROW RT_ARROW_CHAR
#define SEL_ARROW DN_ARROW_CHAR
#define BACK_ARROW DN_ARROW_CHAR
#define NO_ARROW 0

// CHAR CODES FOR SPECIAL CHARS NOT FOUND IN KEYBOARD.H
#define KC_MUTE MEDIA_VOLUME_MUTE
#define KC_VOL_DOWN MEDIA_VOLUME_DOWN
#define KC_VOL_UP MEDIA_VOLUME_UP

#define KC_SELECT KEY_ENTER
#define KC_BACK KEY_ESC

// BUTTON PRESS TIMESTAMPS (bpts)
unsigned long bptsMute = 1;
unsigned long bptsSel = 1;
unsigned long bptsBack = 1;

// DISPLAY MESSAGE TIMESTAMPS (dmts)
unsigned long dmtsMute = 1;
unsigned long dmtsVol = 1;
unsigned long dmtsSel = 1;
unsigned long dmtsBack = 1;

// CURRENT BUTTON STATES
bool mutePressed = false,
     lastVolPin = true,
     selPressed = false,
     backPressed = false;

// LCD DISPLAY OBJECTS
LiquidCrystal_I2C volumeLcd(0x27, 16, 2);

LiquidCrystal_I2C allLcds[] = { volumeLcd };

// ARROW ICONS
byte UP_ARROW[8] = { B00100,
                     B01110,
                     B10101,
                     B00100,
                     B00100,
                     B00100,
                     B00100,
                     B00100 
                   };
byte DN_ARROW[8] = { B00100,
                     B00100,
                     B00100,
                     B00100,
                     B00100,
                     B10101,
                     B01110,
                     B00100 
                   };
byte RT_ARROW[8] = { B00111,
                     B00011,
                     B00101,
                     B01000,
                     B10000,
                     B10000,
                     B10000,
                     B10000 
                   };
byte LT_ARROW[8] = { B11100,
                     B11000,
                     B10100,
                     B00010,
                     B00001,
                     B00001,
                     B00001,
                     B00001 
                   };

// DEBUG LOG ROUTINES
void serialLog(char* message) {
  if (DEBUG_MODE) {
    Serial.print(message);
  }
}
void serialLogLn(char* message) {
  if (DEBUG_MODE) {
    Serial.println(message);
  }
}

// DISPLAY TO LCD ROUTINES
void lcdMessage(LiquidCrystal_I2C lcd,
                int row,
                bool onLeft,
                int beforeIcon,
                char* msg,
                int afterIcon) {

  if (onLeft) {
    lcd.setCursor(0, row);
  } else {
    lcd.setCursor(8, row);
    lcd.print( " " );
  }

  if (beforeIcon) {
    lcd.write(beforeIcon);
  }

  lcd.print(msg);

  if (afterIcon) {
    lcd.write(afterIcon);
  }
}
void broadcast(char* msg1, char* msg2) {
  volumeLcd.setCursor(0, 0);
  volumeLcd.print(msg1);
  volumeLcd.setCursor(0, 1);
  volumeLcd.print(msg2);
}

unsigned long clearMsg(unsigned long now,
                       unsigned long dmts,
                       LiquidCrystal_I2C lcd,
                       int row,
                       bool onLeft,
                       int beforeIcon,
                       char* msg,
                       int afterIcon
                      ) {
  if (dmts > 0) {
    if (now > dmts + MSG_DURATION) {
      lcdMessage(lcd, row, onLeft, beforeIcon, msg, afterIcon);
      return 0;
    }  // inner if
  }    // outer if
  return dmts;
}

void clearMessages(unsigned long now) {
  dmtsMute = clearMsg(now, dmtsMute, volumeLcd, ML_MUTE_ROW, ML_MUTE_SIDE, NO_ARROW, " Mute ", MUTE_ARROW);
  dmtsVol = clearMsg(now, dmtsVol, volumeLcd, ML_VOL_ROW, ML_VOL_SIDE, VOL_ARROW, " Volume", NO_ARROW );
  dmtsSel = clearMsg(now, dmtsSel, volumeLcd, ML_SEL_ROW, ML_SEL_SIDE, SEL_ARROW, " Select", NO_ARROW );
  dmtsBack = clearMsg(now, dmtsBack, volumeLcd, ML_BACK_ROW, ML_BACK_SIDE, NO_ARROW, " Back ", BACK_ARROW );
}

// SETUP FOR EACH INTERFACE
void setupVolumeDial() {
  // INITIALIZE PINS
  pinMode(PIN_VOLUME_DIAL_PUSH_TO_MUTE, INPUT_PULLUP);
  pinMode(VOLUME_PIN_1, INPUT_PULLUP);
  pinMode(VOLUME_PIN_2, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  pinMode(BACK_PIN, INPUT_PULLUP);
}

// SETUP LCD
void setupLcds() {
  serialLogLn("   | Setup LCD");
  for (int i = 0; i < sizeof(allLcds); i++) {
    LiquidCrystal_I2C lcd = allLcds[0];
    lcd.begin();
    lcd.backlight();
    lcd.clear();
  }  // for

  volumeLcd.createChar(UP_ARROW_CHAR, UP_ARROW);
  volumeLcd.createChar(DN_ARROW_CHAR, DN_ARROW);
  volumeLcd.createChar(LT_ARROW_CHAR, LT_ARROW);
  volumeLcd.createChar(RT_ARROW_CHAR, RT_ARROW);
}

void setupKeyboard() {
  serialLogLn("   | Setup Keyboard");
  Consumer.begin();
  Keyboard.begin();
  delay(100);
}

// MAIN SETUP ROUTINE
void setup() {
  delay(100);
  if (DEBUG_MODE) {
    Serial.begin(9600);
    while (!Serial) {}
  }

  serialLogLn("Beginning Setup");
  setupKeyboard();
  setupLcds();
  broadcast("    Setup in    ", "    Progress    ");

  setupVolumeDial();

  serialLogLn("Setup Complete");
}

// READ INPUT FROM BUTTONS AND DIALS
boolean readPin(int pinNum) {
  int val = digitalRead(pinNum);
  /*
  if ( DEBUG_MODE && val != 1 ) {
      char pinNumAsString[2];
      itoa( pinNum, pinNumAsString, 10 );
      serialLog( "--> Pin Activated (" );
      serialLog( pinNumAsString );
      serialLogLn( "), <--" );  
  }
 */
  return (val != 1);
}

unsigned long scanPin(unsigned long now,
                      int pin,
                      unsigned long bpts,
                      void (*btnPressedFnPtr)(unsigned long),
                      bool& existingState) {
  bool newState = readPin(pin);
  // if the state hasn't changed, nothing to do.
  if (newState == existingState) {
    // do nothing.

    // if the state has changed, but the debounce delay has not passed, do nothing
  } else if (now <= DEBOUNCE_DELAY + bpts) {
    // do nothing

    // Things have changed
  } else {
    existingState = newState;
    // If the button was just pressed, update the lcd
    if (existingState) {
      btnPressedFnPtr(now);
      return now;
    }
  }

  return bpts;
}

void scanDial(unsigned long now,
              int pin_1,
              int pin_2,
              void (*btnPressFnPtr)(unsigned long, bool),
              bool& existingState) {
  bool pin_1State = digitalRead(pin_1),
       pin_2State = digitalRead(pin_2);

  if (pin_1State && !existingState) {
    btnPressFnPtr(now, !pin_2State);
  }

  existingState = pin_1State;
}

void scanPins(unsigned long now) {
  bptsMute = scanPin(now, PIN_VOLUME_DIAL_PUSH_TO_MUTE, bptsMute, &muteBtnPress, mutePressed);
  scanDial(now, VOLUME_PIN_1, VOLUME_PIN_2, &volDialTurned, lastVolPin);
  bptsSel = scanPin(now, SELECT_PIN, bptsSel, &selBtnPress, selPressed);
  bptsBack = scanPin(now, BACK_PIN, bptsBack, &backBtnPress, backPressed);
}

void volDialTurned(unsigned long now, bool clockwise) {
  // Send MUTE keypress
  lcdMessage(volumeLcd, ML_VOL_ROW, ML_VOL_SIDE, VOL_ARROW, " VOL  ", clockwise ? UP_ARROW_CHAR : DN_ARROW_CHAR);
  if (clockwise) {
    //serialLogLn("Volume dial turned clockwise.");
    Consumer.write(KC_VOL_UP);
  } else {
    //serialLogLn("Volume dial turned counter-clockwise.");
    Consumer.write(KC_VOL_DOWN);
  }
  dmtsVol = now;
}

void muteBtnPress(unsigned long now) {
  // Indicate on LCD
  lcdMessage(volumeLcd, ML_MUTE_ROW, ML_MUTE_SIDE, NO_ARROW, " MUTE ", MUTE_ARROW );

  // press the mute key
  Consumer.write(KC_MUTE);
  dmtsMute = now;
}

void selBtnPress(unsigned long now) {
  // Indicate on LCD
  lcdMessage(volumeLcd, ML_SEL_ROW, ML_SEL_SIDE, SEL_ARROW, " SELECT", NO_ARROW );
  dmtsSel = now;

  // press the select key
  Keyboard.write(KC_SELECT);
}

void backBtnPress(unsigned long now) {
  // Indicate on LCD
  lcdMessage(volumeLcd, ML_BACK_ROW, ML_BACK_SIDE, NO_ARROW, " BACK ", BACK_ARROW );
  dmtsBack = now;

  // press the select key
  Keyboard.write(KC_BACK);
}

void loop() {
  unsigned long now = millis();
  scanPins(now);
  clearMessages(now);
}