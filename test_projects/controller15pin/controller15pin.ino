// TIME BETWEEN CONTROLLER CHECKS  - 5 ms when rolled out!
#define WAKE_UP_TIME_IN_MS 5179

// # of pins in the controller interface
#define PIN_COUNT 15

int PIN_NUM[PIN_COUNT] = { 2,  3,  4,  5,  6, 
                           7,  8,  9,  10, 11,
                           12, 13, A1, A2, A3
                          };



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
 * Setup the device
 */
void setup() {
  // Open a connection to the display
  Serial.begin( 9600 );

  for ( int i=0; i<PIN_COUNT; i++ ) {
    pinMode( i, INPUT_PULLUP );
  }

  Serial.println( "Setup and ready to go ..." );
  Serial.println();

  Serial.print( " " );
  for ( int i=1; i<=PIN_COUNT; i++ ) {
    if ( i < 10 ) 
    {
      Serial.print( "0" );
    } 
    Serial.print( i, DEC );
    Serial.print( " " );
  } // for
  Serial.println();
  
  // Wait a second! He he
  delay(1000);

  // Prepare to press keys (as trenslations of the joystick state)
  // Keyboard.begin();
} // setup

/**
 * Do this every so often!
 */
void loop() 
{

  Serial.print( "  " );

  for ( int i=0; i<PIN_COUNT; i++ ) {
    Serial.print( analogRead( PIN_NUM[i] ), DEC );
    Serial.print( "   " );
  } // for
  Serial.println();

  for ( int i=0; i<PIN_COUNT; i++ ) {
    if ( readPin( PIN_NUM[i] ) ) {
      Serial.print( "1  " );
    } else {
      Serial.print( "0  " );
    }
  } // for

  Serial.println();
  Serial.println();

  delay( WAKE_UP_TIME_IN_MS );
} // loop
