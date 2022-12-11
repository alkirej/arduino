/**
 * Author:  Jeffery Alkire
 * Written: January 2020 (approx)
 *
 * Updated: January 2022
 *              Increased logging using newly created <common> library.
 *
 * 
 * ============================================================
 * PIN/WIRE INFORMATION FOR ATARI 5200 15 PIN CONVERSION DEVICE
 * ============================================================
 *
 *  Device's input port/jack:
 *      Pin #      Wire Color      Arduino board pin
 *      =====      ==========      =================
 *        1          GRAY                0
 *        2          PURPLE              1
 *        3          BLUE                4
 *        4          GREEN               5
 *        5          YELLOW              6            
 *        6          ORANGE              7
 *        7          RED                 8
 *        8          BROWN               9
 *        9          BLUE               10
 *       10          GREEN              A5
 *       11          YELLOW             11
 *       12          ORANGE             5V
 *       13          RED                12
 *       14          BROWN              A0
 *       15          BLACK             GRD
 *
 * ============================
 * Atari 5200 Controller Pinout
 * ============================ 
 *       1            Right  Column (keypad)
 *       2            Middle Column (keypad)
 *       3            Left   Column (keypad)
 *       4            Start, Pause, and Reset "Column"
 *       5            Third  Row (keypad) and Reset
 *       6            Second Row (keypad) and Pause
 *       7            Top    Row (keypad) and Start
 *       8            Bottom Row (keypad)
 *       9            Pot common
 *      10            Horizontal pot
 *      11            Vertical pot
 *      12            5V (DC)
 *      13            Bottom Fire Buttons (found on side of controller)
 *      14            Top    Fire Buttons (found on side of controller)
 *      15            0V (Ground)
 */

#include <common.h>

//#define WAKE_UP_TIME_IN_MS   2500 
#define WAKE_UP_TIME_IN_MS   25

#define JITTER_THRESHOLD     18
#define MOVEMENT_MULTIPLIER   3

#define MOUSE_MAX_COORD      1024
#define TOP_FIRE_KEYPRESS    KEY_RIGHT_SHIFT
#define BOTTOM_FIRE_KEYPRESS 'b'

#define NUM_COLS 4
#define NUM_ROWS 4

#define POT_COMMON_PIN     10
#define HORIZONTAL_POT_PIN A5
#define VERTICAL_POT_PIN   11

#define BOTTOM_FIRE 12
#define TOP_FIRE    A0

int columnPin[NUM_COLS] = { 0, 1, 4, 5 };
int rowPin[NUM_ROWS]    = { 6, 7, 8, 9 };


int charToSend[NUM_ROWS][NUM_COLS]
= { { '9', '8', '7', KEY_F5 },
    { '6', '5', '4', '+'    },
    { '3', '2', '1', KEY_F4 },
    { '#', '0', '*', 0      }
};

bool buttonPosition[NUM_ROWS][NUM_COLS]
= { { false, false, false, false },
    { false, false, false, false },
    { false, false, false, false },
    { false, false, false, false }
};

bool topFireButtonPosition = false;
bool bottomFireButtonPosition = false;
int  lastXPosition;
int  lastYPosition;

int minX = 725;
int maxX = 925;
int minY = 725;
int maxY = 925;

void logKeypress( char key, bool pressed ) {
    serialLog( "    " );
    if ( KEY_RIGHT_SHIFT == key ) 
       serialLog( "<R-Shift>" );
    else
       serialLog( key );
    serialLogLn( pressed ? " pressed." : " released." );
}

void logPinSetup( int pinNum ) {
    serialLog( "   Pin " );
    serialLog( pinNum );
    serialLogLn( " set." );
}

void logJoystickLoc( int x, int y) {
    serialLog( "    Joystick Loc: (" );
    serialLog( x );
    serialLog( ", " );
    serialLog( y );
    serialLogLn( ")" );
}

/**
 * Setup the device
 */
void setup() 
{
    setupDebugMode( true );
    delay(100);

    // Open a connection to the display
    waitForSerial();

    // init pins for keypad
    for ( int i=0; i<NUM_COLS; i++ ) {
        pinMode( columnPin[i], OUTPUT );
        digitalWrite( columnPin[i], HIGH ); // true );
        logPinSetup( columnPin[i] );
    }
    for ( int i=0; i<NUM_ROWS; i++ ) {
        pinMode( rowPin[i], INPUT_PULLUP );
        logPinSetup( rowPin[i] );
    }

    // turn on the pins for the analog joystick
//    pinMode( POT_COMMON_PIN, OUTPUT );
//    digitalWrite( POT_COMMON_PIN, true );
//    logPinSetup( POT_COMMON_PIN );

//    pinMode( HORIZONTAL_POT_PIN, INPUT );
//    logPinSetup( HORIZONTAL_POT_PIN );
//    pinMode( VERTICAL_POT_PIN, INPUT );
//    logPinSetup( VERTICAL_POT_PIN );

    // turn on pins for fire buttons
    pinMode( TOP_FIRE, INPUT_PULLUP );
    logPinSetup( TOP_FIRE );
    pinMode( BOTTOM_FIRE, INPUT_PULLUP );
    logPinSetup( BOTTOM_FIRE );

    // Wait a second! He he
    delay(100);

    lastXPosition = analogRead( HORIZONTAL_POT_PIN );
    lastYPosition = analogRead( VERTICAL_POT_PIN );

    // Prepare to press buttons and move joystick x & y axis
    Keyboard.begin();
    Mouse.begin();
} // setup

void checkJoystick() {
    int x = analogRead( HORIZONTAL_POT_PIN );
    int y = analogRead( VERTICAL_POT_PIN );
    int c = analogRead( POT_COMMON_PIN );
   
    serialLog( "  x=" );
    serialLog( x );
    serialLog( ", y=" );
    serialLog( y );
    serialLog( ", common=" );
    serialLogLn( c );
}

/**
 * Do this every so often!
 */
void loop() 
{
    bool pressed  = false;
    char keypress = 0;

    // Keypad
    for ( int col=0; col<NUM_COLS; col++ ) {
        digitalWrite( columnPin[col], false );

        for ( int row=0; row<NUM_ROWS; row++ ) {
            pressed = !digitalRead( rowPin[row] );
            keypress = charToSend[row][col];
            if ( pressed != buttonPosition[row][col] ) {
                if ( pressed ) {
                    logKeypress( keypress, true );
                    Keyboard.press( keypress );
                } 
                else 
                {
                    logKeypress( keypress, false );
                    Keyboard.release( keypress );
                }
                buttonPosition[row][col] = !buttonPosition[row][col];
            } // if button state changed
        }
        digitalWrite( columnPin[col], true );
    }

    // Joystick
//    checkJoystick();
/*
    bool movement = false;
    int rawX = analogRead( HORIZONTAL_POT_PIN );
    int rawY = analogRead( VERTICAL_POT_PIN );

//    if ( rawX < minX ) minX = rawX;
//    if ( rawX > maxX ) maxX = rawX;
//    if ( rawY < minY ) minY = rawY;
//    if ( rawY > maxY ) maxY = rawY;

    // Try to ignore super small movements (jittering)
    if ( abs( rawX - lastXPosition ) < JITTER_THRESHOLD ) {
        rawX = lastXPosition;
    } 
    else {
        movement = true;
    }
    if ( abs( rawY - lastYPosition ) < JITTER_THRESHOLD ) {
        rawY = lastYPosition;
    } 
    else {
        movement = true;
    }

      if ( movement ) {
        logJoystickLoc( rawX, rawY );
        Mouse.move( MOVEMENT_MULTIPLIER*(lastXPosition-rawX), MOVEMENT_MULTIPLIER*(lastYPosition-rawY) );
    }

    lastXPosition = rawX;
    lastYPosition = rawY;
 */
    // Fire Buttons
    pressed = !digitalRead( TOP_FIRE );
    if ( pressed != topFireButtonPosition ) {
        if ( pressed ) {
            Keyboard.press( TOP_FIRE_KEYPRESS );
        } 
        else 
        {
            Keyboard.release( TOP_FIRE_KEYPRESS );
        }
        logKeypress( TOP_FIRE_KEYPRESS, pressed );
        topFireButtonPosition = !topFireButtonPosition;
    }

    pressed = !digitalRead( BOTTOM_FIRE );

    if ( pressed != bottomFireButtonPosition ) {
        if ( pressed ) {
            Keyboard.press( BOTTOM_FIRE_KEYPRESS );
        } 
        else 
        {
            Keyboard.release( BOTTOM_FIRE_KEYPRESS );
        }
        logKeypress( BOTTOM_FIRE_KEYPRESS, pressed );
        bottomFireButtonPosition = !bottomFireButtonPosition;
    }

    delay( WAKE_UP_TIME_IN_MS );
} // loop

