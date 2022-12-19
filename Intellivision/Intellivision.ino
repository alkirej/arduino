/**
    Author:   Jeff Alkire

    Written:  January 2022

    Convert Intellivision controller input into usb input.
*/

/**
   =======================================================
   PIN/WIRE INFORMATION FOR INTELLIVISION 9 PIN CONVERSION DEVICE
   =======================================================

    Device's input port/jack:
        Pin #      Wire Color      Arduino board pin
        =====      ==========      =================
          1          BLUE              6
          2          GREEN             7
          3          YELLOW            
          4          ORANGE            9
          5          RED             GRD
          6          BROWN            10
          7          WHITE            11
          8          BLACK            12
          9          BROWN            13
*/

/**
   ===============================
   INTELLIVISION CONTROLLER PINOUT
   ===============================
   See https://arcarc.xmission.com/Web%20Archives/Deathskull%20(May-2006)/games/tech/intvcont.html
       for more discussion.
*/

#include <common.h>
#include <Joystick.h>

// CONSTANTS
#define WAKE_UP_TIME_IN_MS  10
#define DEBOUNCE_TIME_IN_MS 30
#define NUM_PINS 8
#define NONE -1

// Map controller pins to arduino pins
#define PIN_1  6
#define PIN_2  7
#define PIN_3  8
#define PIN_4  9
// PIN 5 IS THE GROUND
#define PIN_6 10
#define PIN_7 11
#define PIN_8 12
#define PIN_9 13

// Map keys and fire buttons to button numbers on the Controller
#define BUTTON_NUM_1                  1
#define BUTTON_NUM_2                  2
#define BUTTON_NUM_3                  3
#define BUTTON_NUM_4                  4
#define BUTTON_NUM_5                  5
#define BUTTON_NUM_6                  6
#define BUTTON_NUM_7                  7
#define BUTTON_NUM_8                  8
#define BUTTON_NUM_9                  9
#define BUTTON_NUM_CLEAR             10
#define BUTTON_NUM_0                  0
#define BUTTON_NUM_ENTER             11
#define BUTTON_NUM_TOP_FIRE          12
#define BUTTON_NUM_BOTTOM_RIGHT_FIRE 13
#define BUTTON_NUM_BOTTOM_LEFT_FIRE  14

const char* BUTTON_NAMES[] = { "0", "1", "2", "3", "4", 
                               "5", "6", "7", "8", "9", 
                               "Clear", "Enter", 
                               "Fire - Top", "Fire - Right", 
                               "Fire - Left" 
                             };

// Map a pin condition to the appropriate controller input
const int   KEYPAD_BUTTON_COUNT = 12;

const bool KEY_1[]     = { false, false, false, true,  true,  false, false, false }; // toByteArray(24);
const bool KEY_2[]     = { false, false, false, true,  false, true,  false, false }; // toByteArray(40);
const bool KEY_3[]     = { false, false, false, true,  false, false, true,  false }; // toByteArray(72);
const bool KEY_4[]     = { false, false, true,  false, true,  false, false, false }; // toByteArray(20);
const bool KEY_5[]     = { false, false, true,  false, false, true,  false, false }; // toByteArray(36);
const bool KEY_6[]     = { false, false, true,  false, false, false, true,  false }; // toByteArray(68);
const bool KEY_7[]     = { false, true,  false, false, true,  false, false, false }; // toByteArray(18);
const bool KEY_8[]     = { false, true,  false, false, false, true,  false, false }; // toByteArray(34);
const bool KEY_9[]     = { false, true,  false, false, false, false, true,  false }; // toByteArray(66);
const bool KEY_CLEAR[] = { true,  false, false, false, true,  false, false, false }; // toByteArray(17);
const bool KEY_0[]     = { true,  false, false, false, false, true,  false, false }; // toByteArray(33);
const bool KEY_ENTER[] = { true,  false, false, false, false, false, true,  false }; // toByteArray(65);

const bool* KEYPAD_KEYS[KEYPAD_BUTTON_COUNT] 
                          = { KEY_1,     KEY_2, KEY_3,
                              KEY_4,     KEY_5, KEY_6,
                              KEY_7,     KEY_8, KEY_9,
                              KEY_CLEAR, KEY_0, KEY_ENTER
                            };
const int  KEYPAD_BUTTON_NUMS[KEYPAD_BUTTON_COUNT]
                         = { BUTTON_NUM_1,     BUTTON_NUM_2, BUTTON_NUM_3,
                             BUTTON_NUM_4,     BUTTON_NUM_5, BUTTON_NUM_6, 
                             BUTTON_NUM_7,     BUTTON_NUM_8, BUTTON_NUM_9,
                             BUTTON_NUM_CLEAR, BUTTON_NUM_0, BUTTON_NUM_ENTER
                           };
const bool FIRE_TOP[]          = { false, false, false, false, true,  false, true,  false };
const bool FIRE_BOTTOM_RIGHT[] = { false, false, false, false, true,  true,  false, false };
const bool FIRE_BOTTOM_LEFT[]  = { false, false, false, false, false, true,  true,  false };

const bool DIRECTION_NORTH[]      = { false, true,  false, false, false, false, false, false };
const bool DIRECTION_N_BY_NE[]    = { false, true,  false, false, false, false, false, true  };
const bool DIRECTION_NORTH_EAST[] = { false, true,  true,  false, false, false, false, true  };
const bool DIRECTION_E_BY_NE[]    = { false, true,  true,  false, false, false, false, false };
const bool DIRECTION_EAST[]       = { false, false, true,  false, false, false, false, false };

const bool DIRECTION_E_BY_SE[]    = { false, false, true,  false, false, false, false, true  };
const bool DIRECTION_SOUTH_EAST[] = { false, false, true,  true,  false, false, false, true  };
const bool DIRECTION_S_BY_SE[]    = { false, false, true,  true,  false, false, false, false };
const bool DIRECTION_SOUTH[]      = { false, false, false, true,  false, false, false, false };
const bool DIRECTION_S_BY_SW[]    = { false, false, false, true,  false, false, false, true  };
const bool DIRECTION_SOUTH_WEST[] = { true,  false, false, true,  false, false, false, true  };
const bool DIRECTION_W_BY_SW[]    = { true,  false, false, true,  false, false, false, false };

const bool DIRECTION_WEST[]       = { true,  false, false, false, false, false, false, false };
const bool DIRECTION_W_BY_NW[]    = { true,  false, false, false, false, false, false, true  };
const bool DIRECTION_NORTH_WEST[] = { true,  true,  false, false, false, false, false, true  };
const bool DIRECTION_N_BY_NW[]    = { true,  true,  false, false, false, false, false, false };

// Note: the order of these directions is important as
//       some directions have similar pins to others
//       to match.
const int  DIRECTION_COUNT = 16;
const bool* DIRECTIONS[DIRECTION_COUNT]
  = { DIRECTION_NORTH_EAST, DIRECTION_SOUTH_EAST, DIRECTION_SOUTH_WEST, DIRECTION_NORTH_WEST,
      DIRECTION_N_BY_NE,    DIRECTION_E_BY_NE,    DIRECTION_E_BY_SE,    DIRECTION_S_BY_SE,
      DIRECTION_S_BY_SW,    DIRECTION_W_BY_SW,    DIRECTION_W_BY_NW,    DIRECTION_N_BY_NW,
      DIRECTION_NORTH,      DIRECTION_EAST,       DIRECTION_SOUTH,      DIRECTION_WEST
    };
const char* DIRECTION_NAMES[DIRECTION_COUNT]
  = { "North-East",         "South-East",        "South-West",        "North-West",
      "North x North-East", "East x North-East", "East x South-East", "South x South-East",
      "South x South-West", "West x South-West", "West x North-West", "North x North-West",
      "North",              "East",              "South",             "West"
    };

const int FULL_XY_VALUE     =  32767;
const int THREE_QTR_XY_VALUE = 32767; // 26479;
const int HALF_XY_VALUE      = 32767; // 20191;
const int QUARTER_XY_VALUE   = 16384; // 13903;
const int ZERO_XY_VALUE      =     0;

// x,y values for each possible direction
// the x,y values are used to indicate to the OS what
// direction our "analog joystick" is pointing.
const int MOVE_CENTER_X     =  ZERO_XY_VALUE;
const int MOVE_CENTER_Y     =  ZERO_XY_VALUE;

const int MOVE_NORTH_X      =  ZERO_XY_VALUE;
const int MOVE_NORTH_Y      = -FULL_XY_VALUE;
const int MOVE_N_BY_NE_X    =  QUARTER_XY_VALUE;
const int MOVE_N_BY_NE_Y    = -THREE_QTR_XY_VALUE;
const int MOVE_NORTH_EAST_X =  HALF_XY_VALUE;
const int MOVE_NORTH_EAST_Y = -HALF_XY_VALUE;
const int MOVE_E_BY_NE_X    =  THREE_QTR_XY_VALUE;
const int MOVE_E_BY_NE_Y    = -QUARTER_XY_VALUE;
const int MOVE_EAST_X       =  FULL_XY_VALUE;
const int MOVE_EAST_Y       =  ZERO_XY_VALUE;
const int MOVE_E_BY_SE_X    =  THREE_QTR_XY_VALUE;
const int MOVE_E_BY_SE_Y    =  QUARTER_XY_VALUE;
const int MOVE_SOUTH_EAST_X =  HALF_XY_VALUE;
const int MOVE_SOUTH_EAST_Y =  HALF_XY_VALUE;
const int MOVE_S_BY_SE_X    =  QUARTER_XY_VALUE;
const int MOVE_S_BY_SE_Y    =  THREE_QTR_XY_VALUE;
const int MOVE_SOUTH_X      =  ZERO_XY_VALUE;
const int MOVE_SOUTH_Y      =  FULL_XY_VALUE;
const int MOVE_S_BY_SW_X    = -QUARTER_XY_VALUE;
const int MOVE_S_BY_SW_Y    =  THREE_QTR_XY_VALUE;
const int MOVE_SOUTH_WEST_X = -HALF_XY_VALUE;
const int MOVE_SOUTH_WEST_Y =  HALF_XY_VALUE;
const int MOVE_W_BY_SW_X    = -THREE_QTR_XY_VALUE;
const int MOVE_W_BY_SW_Y    =  QUARTER_XY_VALUE;
const int MOVE_WEST_X       = -FULL_XY_VALUE;
const int MOVE_WEST_Y       =  ZERO_XY_VALUE;
const int MOVE_W_BY_NW_X    = -THREE_QTR_XY_VALUE;
const int MOVE_W_BY_NW_Y    = -QUARTER_XY_VALUE;
const int MOVE_NORTH_WEST_X = -HALF_XY_VALUE;
const int MOVE_NORTH_WEST_Y = -HALF_XY_VALUE;
const int MOVE_N_BY_NW_X    = -QUARTER_XY_VALUE;
const int MOVE_N_BY_NW_Y    = -THREE_QTR_XY_VALUE;

const int DIRECTION_X_COORDS[DIRECTION_COUNT]
  = { MOVE_NORTH_EAST_X, MOVE_SOUTH_EAST_X, MOVE_SOUTH_WEST_X, MOVE_NORTH_WEST_X,
      MOVE_N_BY_NE_X,    MOVE_E_BY_NE_X,    MOVE_E_BY_SE_X,    MOVE_S_BY_SE_X,
      MOVE_S_BY_SW_X,    MOVE_W_BY_SW_X,    MOVE_W_BY_NW_X,    MOVE_N_BY_NW_X,
      MOVE_NORTH_X,      MOVE_EAST_X,       MOVE_SOUTH_X,      MOVE_WEST_X
    };
const int DIRECTION_Y_COORDS[DIRECTION_COUNT]
  = { MOVE_NORTH_EAST_Y, MOVE_SOUTH_EAST_Y, MOVE_SOUTH_WEST_Y, MOVE_NORTH_WEST_Y,
      MOVE_N_BY_NE_Y,    MOVE_E_BY_NE_Y,    MOVE_E_BY_SE_Y,    MOVE_S_BY_SE_Y,
      MOVE_S_BY_SW_Y,    MOVE_W_BY_SW_Y,    MOVE_W_BY_NW_Y,    MOVE_N_BY_NW_Y,
      MOVE_NORTH_Y,      MOVE_EAST_Y,       MOVE_SOUTH_Y,      MOVE_WEST_Y
    };

const int PULLUP_PIN[] = {
  PIN_1, PIN_2, PIN_3,
  PIN_4, PIN_6, PIN_7,
  PIN_8, PIN_9
};

Joystick_ joystick = Joystick_(
                       0x03,
                       JOYSTICK_TYPE_JOYSTICK,
                       15,    // 15 buttons
                       0,     // no hat switches,
                       true,  // x axis
                       true,  // y axis
                       false, // no z axis
                       false, // no Rx axis
                       false, // no Ry axis
                       false, // no Rz axis
                       false, // no rudder
                       false, // no throttle
                       false, // no accelerator
                       false, // no brake
                       false  // no steering
                     );

int  buttonBeingPressed = NONE;
int  lastPinCondition   = NONE;

// Set the analog Joystick x,y coordinates
void setJoystickDir( int xCoord, int yCoord ) {
    joystick.setXAxis( xCoord );
    joystick.setYAxis( yCoord );
}

/**
    Read the pins, look for the low values and return an array of
    bits (in the form of an integer). A 1 indicates the controller
    has ground the pin.  A 0 indicates it has not.

    The controller grounds pins according to the table at the top of
    this file.
*/
bool pinsVal[NUM_PINS] = {false, false, false, false, false, false, false, false };
bool* readPins() {
  int crtPin;

  for ( int i = NUM_PINS - 1; i >= 0; i-- ) {
    crtPin = digitalRead( PULLUP_PIN[i] );
    pinsVal[i] = crtPin ? false : true;
  } // for

  // serialLogLn( pinsVal, 8 ); 
  return pinsVal;
}

/**
   Determine if the condition supplied (button press or movement
   press) is met by the given condition of the pins.
*/
bool isConditionMet( const bool* pins, const bool* condition )
{
  for ( int i = 0; i < NUM_PINS; i++ )
  {
    if ( condition[i] ) {
      // serialLog( i );
      if ( !pins[i] ) {
        return false;
      }
    }
  } // for loop

  return true;
}

int currentFire = NONE;
void handleFireButtons( const bool* pins )
{
    int  newFire = NONE;

    // Check each fire button to see if it is being pressed.
    if ( isConditionMet( pins, FIRE_TOP ) )
        newFire = BUTTON_NUM_TOP_FIRE;
        
    else if ( isConditionMet( pins, FIRE_BOTTOM_RIGHT ) )
         newFire = BUTTON_NUM_BOTTOM_RIGHT_FIRE;
         
    else if ( isConditionMet( pins, FIRE_BOTTOM_LEFT ) )
         newFire = BUTTON_NUM_BOTTOM_LEFT_FIRE;

    // If a fire button was pressed and/or released, update state.
    if ( currentFire != newFire ) 
    {
        if ( currentFire != NONE )
        {
            joystick.setButton( currentFire, false );
            logButtonRelease( currentFire );
        }
                      
        currentFire = newFire;
        if ( currentFire != NONE )
        {
            joystick.setButton( currentFire, true );
            delay( DEBOUNCE_TIME_IN_MS );
            logButtonPress( currentFire );
        }
    }
}

bool handleDirectionCondition( const bool* pins, int directionIndex )
{
  const bool* condition = DIRECTIONS[ directionIndex ];

  bool  conditionMet = isConditionMet( pins, condition );

  if ( conditionMet ) {
    setJoystickDir( DIRECTION_X_COORDS[directionIndex],
                    DIRECTION_Y_COORDS[directionIndex]
                  );
    logDirection( directionIndex );
  }

  return conditionMet;
}

int currentButton = NONE;
bool handleKeypadInput( const bool* pins, int index )
{
    bool conditionMet = isConditionMet( pins, KEYPAD_KEYS[index] );
    if ( conditionMet )
    {
        if ( currentButton != KEYPAD_BUTTON_NUMS[index] ) 
        {
            if ( currentButton != NONE )
                clearKeypadButtons();
    
            currentButton = KEYPAD_BUTTON_NUMS[index];
            joystick.setButton( currentButton, true );
            delay( DEBOUNCE_TIME_IN_MS );
            logButtonPress( currentButton );
        }
    }
    return conditionMet;
}

void clearKeypadButtons()
{
    if ( currentButton != NONE ) 
    {
        joystick.setButton( currentButton, false );
        logButtonRelease( currentButton );
        currentButton = NONE;
    }
}

// ROUTINES TO LOG DEBUG INFO
void logPins( const bool pins[] )
{
  serialLog( "   Pins: " );
  serialLogLn( pins, NUM_PINS );
}

void logButtonPress( int buttonNum ) 
{
    serialLog( BUTTON_NAMES[buttonNum] );
    serialLogLn( " pressed" );    
}

void logButtonRelease( int buttonNum ) 
{
    serialLog( BUTTON_NAMES[buttonNum] );
    serialLogLn( " released" );    
}

void logDirection( int dirIdx )
{
  serialLog( "   Dir: " );
  serialLog( DIRECTION_NAMES[dirIdx] );
  serialLog( " (" );
  serialLog( DIRECTION_X_COORDS[dirIdx] );
  serialLog( ", " );
  serialLog( DIRECTION_Y_COORDS[dirIdx] );
  serialLogLn( ")" );
}
void logPinInit( int pinIdx )
{
  serialLog( "   Pin #" );
  serialLog( pinIdx );
  serialLog( " (" );
  serialLog( PULLUP_PIN[pinIdx] );
  serialLogLn( ") initialized" );
}

// SETUP ARDUINO ROUTINE (CALLED AT BOOT)
void setup() {
  delay(100);

  setupLogger( (HardwareSerial *) &Serial, false );
  joystick.begin();
  joystick.setXAxisRange(-32767, 32767);
  joystick.setYAxisRange(-32767, 32767);

  // Pullup the pins
  for ( int i = 0; i < NUM_PINS; i++ ) {
    pinMode( PULLUP_PIN[i], INPUT_PULLUP );
    logPinInit( i );
  }
  serialLogLn();
}

// ARDUINO LOOP - CALLED CONSTANTLY IN A LOOP
void loop() {
    bool* pins     = readPins();
    bool  inputFound = false;

    // HANDLE FIRE BUTTONS
    handleFireButtons( pins );
  
    // HANDLE KEYPAD
    for ( int i=0; i < KEYPAD_BUTTON_COUNT; i++ )
    {
        inputFound = handleKeypadInput( pins, i );
        if ( inputFound ) return;
    }
    clearKeypadButtons();

    // HANDLE MOVEMENT
    for ( int i=0; i < DIRECTION_COUNT; i++ )
    {
        inputFound = handleDirectionCondition( pins, i );
        if ( inputFound ) return;
    }
    setJoystickDir( MOVE_CENTER_X, MOVE_CENTER_Y );

  
    delay( WAKE_UP_TIME_IN_MS );
}
