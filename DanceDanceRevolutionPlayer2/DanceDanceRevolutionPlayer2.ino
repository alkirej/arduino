#include <Keyboard.h>
#define WAKE_UP_TIME_IN_MS 2

#define UP_ARROW_PIN    9
#define CORNER_A_PIN    2
#define RIGHT_ARROW_PIN 3
#define DOWN_ARROW_PIN  4
#define LEFT_ARROW_PIN  5
#define CORNER_B_PIN    6

#define UP_ARROW_KEY    'r'
#define CORNER_A_KEY    'a'
#define RIGHT_ARROW_KEY 'g'
#define DOWN_ARROW_KEY  'f'
#define LEFT_ARROW_KEY  'd'
#define CORNER_B_KEY    's'

bool upButton      = false;
bool cornerAButton = false;
bool rightButton   = false;
bool downButton    = false;
bool leftButton    = false;
bool cornerBButton = false;

bool handleButtonChange( int pinNumber, int charToPress, bool previousButtonState ) {
  bool returnVal = previousButtonState;
  bool pressed = !digitalRead( pinNumber );

  if ( pressed != previousButtonState ) 
  {
    returnVal = !previousButtonState;
    if ( pressed ) 
    {
      Keyboard.press( charToPress );
      Serial.print( "PRESS   " );
    } else 
    {
      Keyboard.release( charToPress );
      Serial.print( "RELEASE " );
    }

    Serial.println( pinNumber, DEC );
    Serial.flush();

  }
  
  return returnVal;
}
void setup() {
  // put your setup code here, to run once:

  delay(100);

  Serial.begin(9600);
  Serial.println( "Begin ..." );
  
  pinMode( UP_ARROW_PIN,    INPUT_PULLUP );
  pinMode( CORNER_A_PIN,    INPUT_PULLUP );
  pinMode( RIGHT_ARROW_PIN, INPUT_PULLUP );
  pinMode( DOWN_ARROW_PIN,  INPUT_PULLUP );
  pinMode( LEFT_ARROW_PIN,  INPUT_PULLUP );
  pinMode( CORNER_B_PIN,    INPUT_PULLUP );

  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  upButton      = handleButtonChange( UP_ARROW_PIN,    UP_ARROW_KEY,    upButton      );
  cornerAButton = handleButtonChange( CORNER_A_PIN,    CORNER_A_KEY,    cornerAButton );
  rightButton   = handleButtonChange( RIGHT_ARROW_PIN, RIGHT_ARROW_KEY, rightButton   );
  downButton    = handleButtonChange( DOWN_ARROW_PIN,  DOWN_ARROW_KEY,  downButton    );
  leftButton    = handleButtonChange( LEFT_ARROW_PIN,  LEFT_ARROW_KEY,  leftButton    );
  cornerBButton = handleButtonChange( CORNER_B_PIN,    CORNER_B_KEY,    cornerBButton );

  delay( WAKE_UP_TIME_IN_MS );
}
