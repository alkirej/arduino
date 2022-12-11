// Define pin constants for each color
#define BLUE_PIN 5
#define GREEN_PIN 3
#define RED_PIN 6

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
}

// Define power amounts to each color pin variables
int redPower, greenPower, bluePower;

// the loop function runs over and over again forever
void loop() {
  #define delayTime 10000
  #define OFF 0
  
  /*
  // constant for color changing delay
  redPower = 255;
  greenPower = 0;
  bluePower = 0;

    for(int i = 0; i < 255; i += 1) { // fades out red bring green full when i=255
    redPower--;
    greenPower++;
    analogWrite(RED_PIN, redPower);
    analogWrite(GREEN_PIN, greenPower);
    delay(delayTime);
  } // for loop

    for(int i = 0; i < 255; i += 1) { // fades out red bring green full when i=255
      redPower++;
      greenPower--;
      analogWrite(RED_PIN, redPower);
      analogWrite(GREEN_PIN, greenPower);
      delay(delayTime*10);
  } // for loop
  */

  redPower   = 255;
  greenPower = 20;
  bluePower  = 147;
  analogWrite(RED_PIN,   redPower   );
  analogWrite(GREEN_PIN, greenPower );
  analogWrite(BLUE_PIN,  bluePower  );
  delay( delayTime );

  analogWrite(RED_PIN,   OFF   );
  analogWrite(GREEN_PIN, OFF );
  analogWrite(BLUE_PIN,  OFF  );
  delay( delayTime/10 );

  redPower   =  0;
  greenPower =  255;
  bluePower  =  0;
  analogWrite(RED_PIN,   redPower   );
  analogWrite(GREEN_PIN, greenPower );
  analogWrite(BLUE_PIN,  bluePower  );
  delay( delayTime/4 );

  analogWrite(RED_PIN,   OFF   );
  analogWrite(GREEN_PIN, OFF );
  analogWrite(BLUE_PIN,  OFF  );
  delay( delayTime/10 );

  redPower   =  0;
  greenPower =  0;
  bluePower  =  255;
  analogWrite(RED_PIN,   redPower   );
  analogWrite(GREEN_PIN, greenPower );
  analogWrite(BLUE_PIN,  bluePower  );
  delay( delayTime/4 );

  analogWrite(RED_PIN,   OFF   );
  analogWrite(GREEN_PIN, OFF );
  analogWrite(BLUE_PIN,  OFF  );
  delay( delayTime/4 );

} // loop()
