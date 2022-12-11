#include <Keyboard.h>

// TIME BETWEEN CONTROLLER CHECKS
#define WAKE_UP_TIME_IN_MS 5

// PINS IN USE
//#define DB9_UP_PIN      1
#define BOARD_UP_PIN    1

//#define DB9_DOWN_PIN    2
#define BOARD_DOWN_PIN  2

//#define DB9_LEFT_PIN    3
#define BOARD_LEFT_PIN  3

//#define DB9_RIGHT_PIN   4
#define BOARD_RIGHT_PIN 4

//#define DB9_BTN_RIGHT_PIN   5
#define BOARD_BTN_RIGHT_PIN A1

//#define DB9_BTN_ANY_PIN     6
#define BOARD_BTN_ANY_PIN   6

//#define DB9_BTN_LEFT_PIN    9
#define BOARD_BTN_LEFT_PIN  A0


// KEYPRESSES
#define FIRE_LEFT  KEY_LEFT_CTRL
#define FIRE_RIGHT KEY_LEFT_SHIFT

#define MOVE_UP    KEY_UP_ARROW
#define MOVE_DOWN  KEY_DOWN_ARROW
#define MOVE_LEFT  KEY_LEFT_ARROW
#define MOVE_RIGHT KEY_RIGHT_ARROW

// *** JOYSTICK DIRECTION HANDLING CODE ***
// TRUE  = joystick currently held in this direction
// FALSE = joystick NOT currently being held in this direction
boolean upDirState    = false;
boolean downDirState  = false;
boolean leftDirState  = false;
boolean rightDirState = false;

/** 
 *  Read the state of the specified pin.
 */
boolean readPin( int pinNum ) {
  int val;
  
  val = digitalRead( pinNum );
  if ( val == 1 ) 
  {
    return false;
  
  } else 
  {
    return true;
  }
}

/**
 * Check for and handle joystick direction
 */
void checkForMovement() {
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
      // Serial.println( "  Move up start" );
      
    } else {
      // release move key
      Keyboard.release( MOVE_UP );
      // Serial.println( "  Move up over" );
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
      // Serial.println( "  Move down start" );
      
    } else {
      // release move key
      Keyboard.release( MOVE_DOWN );
      // Serial.println( "  Move down over" );
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
      // Serial.println( "  Move left start" );
      
    } else {
      // release move key
      Keyboard.release( MOVE_LEFT );
      // Serial.println( "  Move left over" );
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
      // Serial.println( "  Move right start" );
      
    } else {
      // release move key
      Keyboard.release( MOVE_RIGHT );
      // Serial.println( "  Move right over" );
    }
    rightDirState = !rightDirState;
  }
}

// *** BUTTON HANDLING CODE ***
// TRUE  = currently being pressed
// FALSE = NOT currently being pressed
boolean leftBtnState  = false;
boolean rightBtnState = false;

/**
 * Is the left fire button pressed?
 */
bool isFiringLeft() {
  bool firingLeft = false;
  int val = digitalRead( BOARD_BTN_ANY_PIN );

  if ( val < 1 ) {
    val = analogRead( BOARD_BTN_LEFT_PIN );
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
bool isFiringRight() {
  bool firingRight = false;
  int val = digitalRead( BOARD_BTN_ANY_PIN );

  if ( val < 1 ) {
    val = analogRead( BOARD_BTN_RIGHT_PIN );
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
void checkForButtonPresses() {
  // LEFT BUTTON
  bool newLeftBtnState, newRightBtnState;
  
  newLeftBtnState = isFiringLeft();
  if ( leftBtnState != newLeftBtnState ) 
  {
    if ( newLeftBtnState ) 
    {
      // press left btn
      Keyboard.press( FIRE_LEFT );
      // Serial.println( "  Left  Button Press" );
      
    } else {
      // release left btn
      Keyboard.release( FIRE_LEFT );
      // Serial.println( "  Left  Button Release" );
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
      Keyboard.press( FIRE_RIGHT );
      // Serial.println( "  Right Button Press" );
      
    } else {
      // release btn
      Keyboard.release( FIRE_RIGHT );
      // Serial.println( "  Right Button Release" );
    }
    rightBtnState = !rightBtnState;
  }

}

/**
 * Setup the device
 */
void setup() {
  // Open a connection to the display
  // Serial.begin( 9600 );

  pinMode( BOARD_UP_PIN,    INPUT_PULLUP );
  pinMode( BOARD_DOWN_PIN,  INPUT_PULLUP );
  pinMode( BOARD_LEFT_PIN,  INPUT_PULLUP );
  pinMode( BOARD_RIGHT_PIN, INPUT_PULLUP );
  pinMode( BOARD_BTN_RIGHT_PIN, INPUT );
  pinMode( BOARD_BTN_ANY_PIN,   INPUT_PULLUP );
  pinMode( BOARD_BTN_LEFT_PIN,  INPUT );

  // Wait a second! He he
  delay(1000);

  // Prepare to press keys (as trenslations of the joystick state)
  Keyboard.begin();
} // setup

/**
 * Do this every so often!
 */
void loop() 
{
  readPin( BOARD_UP_PIN );
  readPin( BOARD_DOWN_PIN );
  readPin( BOARD_LEFT_PIN );
  readPin( BOARD_RIGHT_PIN );
  readPin( BOARD_BTN_RIGHT_PIN );
  readPin( BOARD_BTN_ANY_PIN );
  readPin( BOARD_BTN_LEFT_PIN );

  checkForButtonPresses();
  checkForMovement();
  delay( WAKE_UP_TIME_IN_MS );
} // loop
