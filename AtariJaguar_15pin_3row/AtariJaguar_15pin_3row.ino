#include <Keyboard.h>
#define WAKE_UP_TIME_IN_MS 5

#define NUM_COLS 4
#define NUM_ROWS 6

int columnPin[NUM_COLS] = { 1, 2, 3, 4 };
int rowPin[NUM_ROWS]    = { 5, 6, 7, 8, 9, 10 };

int keypress[NUM_ROWS][NUM_COLS] 
                  = { { 0, 0, 0, 'i' },
                      { 'o', 'c', 'v', 'a' },
                      { '3', '2', '1', KEY_RIGHT_ARROW },
                      { '6', '5', '4', KEY_LEFT_ARROW },
                      { '9', '8', '7', KEY_DOWN_ARROW },
                      { 'l', '0', 'k', KEY_UP_ARROW }
                     };

bool buttonPosition[NUM_ROWS][NUM_COLS];

/**
 * Setup the device
 */
void setup() 
{
  // Wait before startup
  delay(100);

  // init pins for keypad
  for ( int i=0; i<NUM_COLS; i++ ) {
    pinMode( columnPin[i], OUTPUT );
    digitalWrite( columnPin[i], true );
  }
  for ( int i=0; i<NUM_ROWS; i++ ) {
    pinMode( rowPin[i], INPUT );
  }

  // Wait
  delay(100);

  // Prepare to press keys (as trenslations of the joystick state)
  Keyboard.begin();

  // initialize button up/down array
  for ( int i=0; i<NUM_COLS; i++ ) {
    digitalWrite( columnPin[i], false );
      
    for ( int j=0; j<NUM_ROWS; j++ ) {
      buttonPosition[j][i] = digitalRead( rowPin[j] );
    }
  } // for NUM_COLS
} // setup

/**
 * Do this every so often!
 */
void loop() 
{

  // Keypad
  for ( int i=0; i<NUM_COLS; i++ ) {
    digitalWrite( columnPin[i], false );
      
    for ( int j=0; j<NUM_ROWS; j++ ) {
      bool pressed = !digitalRead( rowPin[j] );
      
      if ( pressed != buttonPosition[j][i]  ) {
        if ( pressed ) 
        {
          Keyboard.press( keypress[j][i] );
        } else 
        {
          Keyboard.release( keypress[j][i] );
        }

        buttonPosition[j][i] = !buttonPosition[j][i];
      }
    }
    digitalWrite( columnPin[i], true );
  }

  delay( WAKE_UP_TIME_IN_MS );
} // loop
