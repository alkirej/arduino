void setup() {
  // put your setup code here, to run once:
  delay( 100 );
  
  Serial.begin( 9600 );

  pinMode( 13, OUTPUT );
  digitalWrite( 13, LOW );
  
  // Wait for the serial port to be opened
  while (!Serial) {
    // flash led
    digitalWrite( 13, HIGH );
    delay(500);
    digitalWrite( 13, LOW );
    delay(500);
  }
  for ( int i=2; i<=4; i++ )
  {
    pinMode( i, INPUT_PULLUP );
  }

  Serial.println( "Time to start:" );
  Serial.flush();
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println( "Scanning pins:" );
  Serial.flush();

  for ( int pinNum=2; pinNum<=4; pinNum++ ) {
    int pinValue = digitalRead(pinNum);
    Serial.print( "Pin: " );

    if ( pinNum < 10 ) { Serial.print( " " ); }
    Serial.print( pinNum, DEC);
    Serial.print( "=" );
    Serial.print( pinValue, DEC );
    Serial.print( "  " );
    if ( pinNum%5 == 4 ) {
      Serial.println();
    }
    Serial.flush();
  }
 
  Serial.println();
  Serial.flush();
/*
  // digital reads on analog pins
  for ( int pinNum=A0; pinNum<=A5; pinNum++ ) {
    int pinValue = digitalRead(pinNum);
    Serial.print( "Pin: " );

    if ( pinNum < 10 ) { Serial.print( " " ); }
    Serial.print( pinNum, DEC);
    Serial.print( "=" );
    Serial.print( pinValue, DEC );
    Serial.print( "  " );
    if ( (pinNum-A0)%5 == 4 ) {
      Serial.println();
    }
    Serial.flush();
  }

  Serial.println();
  Serial.println();
  Serial.flush();
 */
  // ANALOG READS
  // A0-A5 PINS
  // "DIGITAL" PINS 3/5/6/9/10/11/13
  delay( 1000 );
}
