/**
 * Author:   Jeff Alkire
 *
 * Written:  January 2020
 *
 * Updated:  January 2022
 *               Added debug info.
 *
 *     Allow use of olden days controllers using 9 pin Atari joystick port.
 *     The device has 2 "phases".
 *     Phase 1:  1- Starts at boot.
 *               2- Waits until the user has inserted the controller and pressed
 *                   the # key or fire button.
 *               3- Determines the type of controller based upon this input.
 *
 *     Phase 2:  - Starts at the conclusion of phase 1.
 *               - Device stays in phase 2 until the device is powered down.
 *               - Receives controller inputs, converts them to keypresses
 *                   and sends them up the usb wire to the computer.
 *
 */
 
/**
 * =======================================================
 * PIN/WIRE INFORMATION FOR ATARI 9 PIN CONVERSION DEVICE
 * =======================================================
 *
 *  Device's input port/jack:
 *      Pin #      Wire Color      Arduino board pin
 *      =====      ==========      =================
 *        1          YELLOW             4
 *        2          BLUE               5            
 *        3          RED                6
 *        4          ORANGE             7
 *        5          BLUE              A1
 *        6          YELLOW            A0
 *        7          PURPLE            13
 *        8          BLACK            GND
 *        9          YELLOW             8
 *
 *  LCD
 *    Pin        Wire Color        Arduino board pin
 *    ===        ==========        =================
 *    GND          BLUE               GND 
 *    VCC          PURPLE              5V
 *    SDA          GRAY                 2
 *    SCL          WHITE                3
 */
 
/**
 * ==================
 * CONTROLLER PINOUTS
 * ==================
 * 
 * ATARI 2600 JOYSTICK
 *     Pin        Wire Color        Information
 *     ===        ==========        ===========
 *       1          WHITE             Up
 *       2          BLUE              Down
 *       3          GREEN             Left
 *       4          BROWN             Right
 *       5          -----             NOT CONNECTED
 *       6          ORANGE            Button
 *       7          -----             NOT CONNECTED
 *       8          BLACK             GROUND
 *       9          -----             NOT CONNECTED
 *
 * ATARI 2600 PADDLES
 *     Pin        Information
 *     ===        ===========
 *       1          NOT CONNECTED
 *       2          NOT CONNECTED
 *       3          Paddle 1 Button
 *       4          Paddle 2 Button
 *       5          Paddle 2 Potentiometer Output
 *       6          NOT CONNECTED
 *       7          Power for Potentiometers
 *       8          GROUND
 *       9          Paddle 1 Potentiometer Output
 *
 * ATARI 2600 KEYPAD
 *     Pin        Information
 *     ===        ===========
 *       1          First Row     (1,2,3)
 *       2          Second Row    (4,5,6)
 *       3          Third Row     (7,8,9)
 *       4          Fourth Row    (*,0,#)
 *       5          Left Column   (1,4,7,*)
 *       6          Right Column  (3,6,9,#)
 *       7          +5V -- pulls up pins 5 & 9
 *       8          GROUND
 *       9          Middle Column (2,5,8,0)
 *
 * ATARI 7800 JOYSTICK
 *     Pin        Wire Color        Information
 *     ===        ==========        ===========
 *       1          WHITE             Up
 *       2          BLUE              Down
 *       3          GREEN             Left
 *       4          BROWN             Right
 *       5          RED               Right Button
 *       6          ORANGE            Button (Both)
 *       7          -----             NOT CONNECTED
 *       8          BLACK             GROUND
 *       9          YELLOW            Left Button
 *
 */

#include <Keyboard.h>
#include <Mouse.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DEBUG_MODE false

// TIME BETWEEN CONTROLLER CHECKS
#define WAKE_UP_TIME_IN_MS 5

// PINS IN USE
#define BOARD_UP_PIN    4
#define BOARD_DOWN_PIN  5 
#define BOARD_LEFT_PIN  6
#define BOARD_RIGHT_PIN 7

#define BOARD_BTN_RIGHT_PIN A1
#define BOARD_BTN_ANY_PIN   8
#define BOARD_BTN_LEFT_PIN  A0

// Atari 2600 booster grip buttons
#define BOARD_BOOSTER_GRIP_TRIGGER A1
#define BOARD_BOOSTER_GRIP_THUMB   A0

// Atari 2600 paddle
#define BOARD_LEFT_PADDLE_BTN  6
#define BOARD_RIGHT_PADDLE_BTN 7
#define BOARD_LEFT_PADDLE_LOC  A0
#define BOARD_RIGHT_PADDLE_LOC A1
#define BOARD_POWER     13

// KEYPRESSES
#define BUTTON_1_PLAYER_1  KEY_LEFT_CTRL
#define BUTTON_2_PLAYER_1  KEY_LEFT_ALT
#define BUTTON_3_PLAYER_1  ' '

#define BUTTON_1_PLAYER_2 'A'
#define BUTTON_2_PLAYER_2 'S'
#define BUTTON_3_PLAYER_2 'Q'

#define MOVE_UP    KEY_UP_ARROW
#define MOVE_DOWN  KEY_DOWN_ARROW
#define MOVE_LEFT  KEY_LEFT_ARROW
#define MOVE_RIGHT KEY_RIGHT_ARROW


// *** BUTTON HANDLING CODE ***
// TRUE  = currently being pressed
// FALSE = NOT currently being pressed
boolean fireBtnState   = false;
boolean leftBtnState   = false;
boolean rightBtnState  = false;

// *** DIFFERENT CONTROLLERS SUPPORTED BY THIS DEVICE ***
#define MODE_NOT_SET             0 
#define MODE_ATARI_2600_JOYSTICK 1
#define MODE_ATARI_2600_PADDLE   2
#define MODE_ATARI_2600_KEYBOARD 3
#define MODE_ATARI_7800          4

int controllerMode = MODE_NOT_SET;

// *** LEFT AND RIGHT PADDLE POSITION DATA
#define PADDLE_MOVEMENT_THRESHOLD 15
#define PADDLE_MOVEMENT_FACTOR    1

int leftPaddlePosition;
int rightPaddlePosition;

// *** KEYBOARD CONTROLLER AND VIDEO TOUCH PAD
#define NUM_ROWS 4
#define NUM_COLS 3
int rowPins[NUM_ROWS] = { 4, 5, 6, 7 };
int colPins[NUM_COLS] = { A1, A0, 8 };
int At2600KeyboardCharToSend[NUM_ROWS][NUM_COLS]
                  = { {    KEY_F11, (int) '2', (int) '3' },
                      {  (int) '4',   KEY_F12, (int) '6' },
                      {  (int) '7', (int) '8', (int) '9' },
                      {  (int) '-', (int) '0', (int) '=' }
                     };
bool buttonState[NUM_ROWS][NUM_COLS]
                  = { { false, false, false },
                      { false, false, false },
                      { false, false, false },
                      { false, false, false }
                     };

// *** JOYSTICK DIRECTION HANDLING CODE ***
// TRUE  = joystick currently held in this direction
// FALSE = joystick NOT currently being held in this direction
boolean upDirState    = false;
boolean downDirState  = false;
boolean leftDirState  = false;
boolean rightDirState = false;

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

/**
 * Log a message to the serial output (if we are in debug mode and can read it).
 */
void serialLog( char* message ) {
    if ( DEBUG_MODE ) {
        Serial.println( message );
    }
}

void serialLogNoEol( char* message ) {
    if ( DEBUG_MODE ) {
        Serial.print( message );
    }
}

/**
 * Make very sure this is a 7800 controller fire press.
 */
boolean isThisAn7800Button() {
  serialLogNoEol( "Is this an Atari 7800 button? " );

  if ( isFiringRight() || isFiringLeft () ) {
    // value found on a button press. Is is small enough to be a 7800?
    int leftVal  = analogRead( BOARD_BTN_LEFT_PIN );
    int rightVal = analogRead( BOARD_BTN_RIGHT_PIN );

    if ( leftVal<100 && rightVal<100 ) {
      if ( DEBUG_MODE ) {
          Serial.println( "YES!" );
      }
      return true;
    }
  }
  serialLog( "no" );

  return false;
}

/**
 * Has someone pressed the fire button on the left paddle?
 */
boolean isLeftPaddleFirePressed() {
  return ( digitalRead( BOARD_LEFT_PADDLE_BTN ) < 1 );
}

/**
 * Has someone pressed the fire button on the right paddle?
 */
bool isRightPaddleFirePressed() {
  return ( digitalRead( BOARD_RIGHT_PADDLE_BTN ) < 1 );
}

void displayControllerMode() {
  serialLog( "============================" );
  serialLog( "Display the controller mode." );
  serialLog( "============================" );
  serialLog( "    See LCD for results" );
  serialLog( "============================" );

  lcd.clear();
  switch ( controllerMode ) {
    case MODE_NOT_SET:
        lcd.setCursor(4,0);
        lcd.print( F("UNKNOWN") );
        lcd.setCursor(3,1);
        lcd.print( F("CONTROLLER") );
        break;

    case MODE_ATARI_2600_JOYSTICK:
        lcd.setCursor(3,0);
        lcd.print( F("Atari 2600") );
        lcd.setCursor(4,1);
        lcd.print( F("Joystick") );
        break;

    case MODE_ATARI_2600_PADDLE:
        lcd.setCursor(3,0);
        lcd.print( F("Atari 2600") );
        lcd.setCursor(4,1);
        lcd.print( F("Paddles") );
        break;

    case MODE_ATARI_2600_KEYBOARD:
        lcd.setCursor(3,0);
        lcd.print( F("Atari 2600") );
        lcd.setCursor(4,1);
        lcd.print( F("Keyboard") );
        break;
        
    case MODE_ATARI_7800:
        lcd.setCursor(3,0);
        lcd.print( F("Atari 7800") );
        lcd.setCursor(4,1);
        lcd.print( F("Controls") );
        break;
  }
}

void setupKeyboardPins() {
  // serialLog( "Setup Keyboard Pins" );

  for ( int n=0; n<NUM_COLS; n++ ) {
    pinMode( colPins[n], OUTPUT );
    digitalWrite( colPins[n], HIGH );
  }
  pinMode( BOARD_POWER, OUTPUT );
  digitalWrite( BOARD_POWER, HIGH );

  for ( int n=0; n<NUM_ROWS; n++ ) {
    pinMode( rowPins[n], INPUT_PULLUP );
  }
}

void setupStandardPins() {
  // serialLog( "Setup Standard Pins" );
    
  pinMode( BOARD_UP_PIN,    INPUT_PULLUP );
  pinMode( BOARD_DOWN_PIN,  INPUT_PULLUP );
  pinMode( BOARD_LEFT_PIN,  INPUT_PULLUP );
  pinMode( BOARD_RIGHT_PIN, INPUT_PULLUP );
  pinMode( BOARD_BTN_RIGHT_PIN, INPUT );
  pinMode( BOARD_BTN_ANY_PIN,   INPUT_PULLUP );
  pinMode( BOARD_BTN_LEFT_PIN,  INPUT );

  pinMode( BOARD_POWER, OUTPUT );
  digitalWrite( BOARD_POWER, false );
}

// Note: based on 0 based indexes
#define ATARI_2600_KEYBOARD_POUND_ROW 3
#define ATARI_2600_KEYBOARD_POUND_COL 2

bool checkForPressedPound() {
  bool returnVal;
  digitalWrite( colPins[ATARI_2600_KEYBOARD_POUND_COL], LOW );
  returnVal = !digitalRead( rowPins[ATARI_2600_KEYBOARD_POUND_ROW] );
  digitalWrite( colPins[ATARI_2600_KEYBOARD_POUND_COL], HIGH );
  return returnVal;
}
/**
 * Figure out which controller we are using
 */
int determineControllerMode() 
{
  serialLog( "=========================" );
  serialLog( "DETERMINE CONTROLLER MODE" );
  serialLog( "=========================" );
  
  lcd.clear(); 
  lcd.setCursor(3,0);
  lcd.print( F("PRESS # OR") );

  lcd.setCursor(1,2);
  lcd.setCursor(1,1);
  lcd.print( F("ANY FIRE BUTTON") );

    
  while ( MODE_NOT_SET == controllerMode ) {
    delay( WAKE_UP_TIME_IN_MS );

    // check for controllers without fire buttons (only numeric keypad at this point)
    setupKeyboardPins();
    if ( checkForPressedPound() ) {
        controllerMode = MODE_ATARI_2600_KEYBOARD;
    }
    setupStandardPins();

    // check for controllers with fire buttons
    if ( isFirePressed() ) {
      serialLog( "--> Button press detected <--" );
        
      if ( isThisAn7800Button() ) {
        controllerMode = MODE_ATARI_7800;
      } else {
        controllerMode = MODE_ATARI_2600_JOYSTICK;
      }

    } else if ( isLeftPaddleFirePressed() || isRightPaddleFirePressed() ) {
      serialLog( "--> Button press detected (from a paddle) <--" );
      
      controllerMode = MODE_ATARI_2600_PADDLE;
      leftPaddlePosition  = analogRead( BOARD_LEFT_PADDLE_LOC );
      rightPaddlePosition = analogRead( BOARD_RIGHT_PADDLE_LOC );

      digitalWrite( BOARD_POWER, true );
    } // if isFirePressed

  } // while

  // set pins up proper if the keyboard is found
  if ( controllerMode == MODE_ATARI_2600_KEYBOARD ) {
    setupKeyboardPins();
  }
  
  displayControllerMode();
  return controllerMode;
} // determineControllerMode function

/** 
 *  Read the state of the specified pin.
 */
boolean readPin( int pinNum ) 
{
  int val;
  
  val = digitalRead( pinNum );
  if ( val == 1 ) 
  {
    return false; 
  } 

  char pinNumAsString[2];
  itoa( pinNum, pinNumAsString, 10 );
  serialLogNoEol( "--> Pin Activated (" );
  serialLogNoEol( pinNumAsString );
  serialLog( "), <--" );  

  return true;
}

/**
 * Check for and handle joystick direction
 */
void checkForJoystickMovement() 
{
  boolean newUpDirState,
          newDownDirState,
          newLeftDirState,
          newRightDirState;

  // GOING UP?
  newUpDirState    = readPin( BOARD_UP_PIN );  
  if ( upDirState != newUpDirState ) 
  {
    if ( newUpDirState ) 
    {
      // press move key
      Keyboard.press( MOVE_UP );
      
    } else {
      // release move key
      Keyboard.release( MOVE_UP );
    }
    upDirState = !upDirState;
  }

  // GOING DOWN?
  newDownDirState  = readPin( BOARD_DOWN_PIN );  
  if ( downDirState != newDownDirState ) 
  {
    if ( newDownDirState ) 
    {
      // press move key
      Keyboard.press( MOVE_DOWN );
      
    } else {
      // release move key
      Keyboard.release( MOVE_DOWN );
    }
    downDirState = !downDirState;
  }

  // GOING LEFT?
  newLeftDirState  = readPin( BOARD_LEFT_PIN );  
    if ( leftDirState != newLeftDirState ) 
  {
    if ( newLeftDirState ) 
    {
      // press move key
      Keyboard.press( MOVE_LEFT );
      
    } else {
      // release move key
      Keyboard.release( MOVE_LEFT );
    }
    leftDirState = !leftDirState;
  }

  newRightDirState = readPin( BOARD_RIGHT_PIN );  
  if ( rightDirState != newRightDirState ) 
  {
    if ( newRightDirState ) 
    {
      // press move key
      Keyboard.press( MOVE_RIGHT );
      
    } else {
      // release move key
      Keyboard.release( MOVE_RIGHT );
    }
    rightDirState = !rightDirState;
  }
}

/**
 * See if either paddle has moved.
 */
void checkForPaddleMovement() {
  boolean foundMovement = false;
  int rightLoc = analogRead( BOARD_RIGHT_PADDLE_LOC );
  int leftLoc  = analogRead( BOARD_LEFT_PADDLE_LOC );
  
  if ( abs(rightLoc - rightPaddlePosition) > PADDLE_MOVEMENT_THRESHOLD ) {
    foundMovement = true;
  } else {
    rightLoc = rightPaddlePosition;
  }

  if ( abs(leftLoc - leftPaddlePosition) > PADDLE_MOVEMENT_THRESHOLD ) {
    foundMovement = true;
  } else { 
    leftLoc = leftPaddlePosition;
  }

  if ( foundMovement ) {
    int leftMovement  = (leftLoc- leftPaddlePosition ) * PADDLE_MOVEMENT_FACTOR;
    int rightMovement = (rightLoc-rightPaddlePosition) * PADDLE_MOVEMENT_FACTOR;
    Mouse.begin();
    Mouse.move( leftMovement, rightMovement, 0 );
    Mouse.end();
    
    leftPaddlePosition  = leftLoc;
    rightPaddlePosition = rightLoc;
  }
}

/**
 * Check for and handle joystick direction
 */
void checkForMovement() 
{
  if ( MODE_ATARI_2600_PADDLE == controllerMode ) {
    checkForPaddleMovement(); 
  } else {
    checkForJoystickMovement();
  }
}
/**
 * Are any fire buttons being pressed?
 */
bool isFirePressed() {
  return  digitalRead( BOARD_BTN_ANY_PIN ) < 1;
}

/**
 * Is the left fire button pressed?
 */
bool isFiringLeft() 
{
  bool firingLeft = false;

  if ( isFirePressed() ) {
    int val = analogRead( BOARD_BTN_LEFT_PIN );
    if ( val > 2 ) 
    {
      firingLeft = true;
    }
  }

  return firingLeft;
}

/**
 * Is the right fire button pressed?
 */
bool isFiringRight() 
{
  bool firingRight = false;

  if ( isFirePressed() ) {
    int val = analogRead( BOARD_BTN_RIGHT_PIN );
    if ( val > 2 ) 
    {
      firingRight = true;
    }
  }

  return firingRight;
}

/**
 * Check for and handle button presses
 */
void checkForButtonPresses() 
{
  switch ( controllerMode ) {
    case MODE_ATARI_2600_JOYSTICK:
      checkFor2600JoystickButtonPress();
      break;

    case MODE_ATARI_2600_PADDLE:
      checkFor2600PaddleButtonPress();
      break;
    
    case MODE_ATARI_7800:
      checkFor7800ButtonPresses();
      break;      
  }
}

/**
 * Check for and handle button presses on an Atari 2600 controller
 * We will use one of the 7800 button states (the left one) to save mem.
 */
void checkFor2600JoystickButtonPress() 
{
  bool isFiring = isFirePressed();
  if ( fireBtnState != isFiring )
  {
    if ( isFiring )
    {
      // press fire btn
      Keyboard.press( BUTTON_1_PLAYER_1 );
        
    } else 
    {
      // release fire btn
      Keyboard.release( BUTTON_1_PLAYER_1 );
    }
    fireBtnState = !fireBtnState;
  }
}

/**
 * Check for and handle button presses on an Atari 7800 controller
 */
void checkFor7800ButtonPresses() 
{
  // LEFT BUTTON
  bool newLeftBtnState, newRightBtnState;
  
  newLeftBtnState = isFiringLeft();
  if ( leftBtnState != newLeftBtnState ) 
  {
    if ( newLeftBtnState ) 
    {
      // press left btn
      Keyboard.press( BUTTON_1_PLAYER_1 );
      
    } else {
      // release left btn
      Keyboard.release( BUTTON_1_PLAYER_1 );
    }
    leftBtnState = !leftBtnState;
  }

  // RIGHT BUTTON
  newRightBtnState = isFiringRight();
  if ( rightBtnState != newRightBtnState ) 
  {
    if ( newRightBtnState ) 
    {
      // press btn
      Keyboard.press( BUTTON_2_PLAYER_1 );
      
    } else {
      // release btn
      Keyboard.release( BUTTON_2_PLAYER_1 );
    }
    rightBtnState = !rightBtnState;
  }

}

/**
 * Look for Atari 2600 keyboard # key.
 */
void checkForKeyboardPresses() {
  for ( int n=0; n<NUM_COLS; n++ ) {
    digitalWrite( colPins[n], LOW );
    for ( int m=0; m<NUM_ROWS; m++ ) {
      bool pressed = !digitalRead( rowPins[m] );
      if ( pressed != buttonState[m][n] ) {
        buttonState[m][n] = !buttonState[m][n];
        if ( pressed )
        {
          Keyboard.press( At2600KeyboardCharToSend[m][n] );
        } else
        {
          Keyboard.release( At2600KeyboardCharToSend[m][n] );
        }
      } // if new state
    } // rows for
    digitalWrite( colPins[n], HIGH );
  } // columns for
} // end method

/**
 * Is the button on the left paddle being pressed?
 */
boolean isLeftPaddleButtonPressed() {
  return  digitalRead( BOARD_LEFT_PADDLE_BTN ) < 1;
}

/**
 * Is the button on the right paddle being pressed?
 */
boolean isRightPaddleButtonPressed() {
  return  digitalRead( BOARD_RIGHT_PADDLE_BTN ) < 1;
}

/**
 * Check for and handle button presses on an Atari 2600 controller
 * We will use one of the 7800 button states (the left one) to save mem.
 */
void checkFor2600PaddleButtonPress() {
  // RIGHT BUTTON
  bool newBtnState = isRightPaddleButtonPressed();
  if ( rightBtnState != newBtnState ) 
  {
    if ( newBtnState ) 
    {
      // press btn
      Keyboard.press( BUTTON_1_PLAYER_2 );
    
    } else {
      // release btn
      Keyboard.release( BUTTON_1_PLAYER_2 );
    }
    rightBtnState = !rightBtnState;
  }

  // LEFT BUTTON
  newBtnState = isLeftPaddleButtonPressed();
  if ( leftBtnState != newBtnState ) 
  {
    if ( newBtnState ) 
    {
      // press btn
      Keyboard.press( BUTTON_1_PLAYER_1 );
      
    } else {
      // release btn
      Keyboard.release( BUTTON_1_PLAYER_1 );
    }
    leftBtnState = !leftBtnState;
  }
}

void waitForSerial() {
    // Use Serial port in debug mode only.
    if ( DEBUG_MODE )
    {
          lcd.clear(); 
          lcd.setCursor(3,0);
          lcd.print( F("WAITING FOR") );
          lcd.setCursor(1,1);
          lcd.print( F("SERIAL MONITOR") );

        // Wait for the serial port to be opened
        while (!Serial) {
            // flash led
            digitalWrite( 13, HIGH );
            delay(500);
            digitalWrite( 13, LOW );
            delay(500);
        }
    }
}

/**
 * Setup the device
 */
void setup() 
{
  delay(100);
  lcd.begin();
  lcd.backlight();

  waitForSerial();
  
  // Special Note: this method will set the pins into the proper alignment based on the controller
  determineControllerMode();

  // Wait a second! He he
  delay(1000);

  // Prepare to press keys (as translations of the joystick state)
  Keyboard.begin();

  if ( DEBUG_MODE ) {
      Serial.begin(9600);
  }
} // setup

/**
 * Do this every so often!
 */
void loop() 
{
  if ( controllerMode == MODE_ATARI_2600_KEYBOARD ) {
    // special case: numeric keyboard
    checkForKeyboardPresses();
    
  } else {
    // typical control with directions and button(s)
    readPin( BOARD_UP_PIN );
    readPin( BOARD_DOWN_PIN );
    readPin( BOARD_LEFT_PIN );
    readPin( BOARD_RIGHT_PIN );
    readPin( BOARD_BTN_RIGHT_PIN );
    readPin( BOARD_BTN_ANY_PIN );
    readPin( BOARD_BTN_LEFT_PIN );
  
    checkForButtonPresses();
    checkForMovement();
  }
  
  delay( WAKE_UP_TIME_IN_MS );
} // loop
