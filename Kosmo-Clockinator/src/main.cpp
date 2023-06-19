#include <Arduino.h>
#include <TM1651.h> // GOTEK LEDC68 library. https://github.com/coopzone-dc/GotekLEDC68/
#include <RotaryEncoder.h>
// #include <uClock.h>

#define CLK 2 //pins definitions for TM1651 and can be changed to other ports       
#define DIO 3

// Clock outputs
#define OUT1 12  // /1
#define OUT2 11  // /2 
#define OUT3 10  // /4 
#define OUT4 9   // /8
#define OUT5 8   // /16 
//Rotary encoder inputs
#define encoderA   7
#define encoderB   6
#define encoderBTN 5

float MaxBPM = 300.0;
float BPM = 120.0;
// float msPerMin = 1000 * 60;

// Millis
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long countMillis = 0;

// Coarse/granular mode
// 0 = Coarse mode (change BPM by 10), 1 = granular mode (change BPM by 1)
int bpmStepSize = 10;

// put function declarations here:
TM1651 Display(2,3);
RotaryEncoder Encoder(encoderA, encoderB, RotaryEncoder::LatchMode::TWO03);

void setup() {
  // DEBUG - Serial printout
  Serial.begin(9600);
  // Setup the LED display
  Display.displaySet(2);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  Display.displayDP(0); // Turn off decimal point. This is buggy, so we should turn it off.
}

void loop() {
  // Show BPM on display
  Display.displayInteger(BPM); 

  // Encoder position + tick
  static int encPos = 0;
  Encoder.tick();  
  int encNewPos = Encoder.getPosition();
  int encDirection = (int)Encoder.getDirection();
  if (encPos != encNewPos) {
    if (BPM >= bpmStepSize && BPM <= (MaxBPM+bpmStepSize)) {
      if (encDirection == 1) {
        if (BPM > bpmStepSize) {
          BPM-=bpmStepSize;
        }
      } else if (encDirection == -1) {
        if (BPM < (MaxBPM+bpmStepSize)) {
          BPM+=bpmStepSize;
        }
      }
    }
    

    // Serial.print("pos: ");
    //Serial.print(encNewPos);
    //Serial.print(" dir: ");
    //Serial.println((int)(Encoder.getDirection()));
    Serial.print("bpm: ");
    Serial.println(BPM);
  }
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}