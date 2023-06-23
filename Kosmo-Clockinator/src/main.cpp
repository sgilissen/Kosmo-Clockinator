#include <Arduino.h>
#include <TM1651.h> // GOTEK LEDC68 library. https://github.com/coopzone-dc/GotekLEDC68/
// #include <RotaryEncoder.h>
// #include <uClock.h>
#include <NewEncoder.h>

#define CLK 17 //A2 //pins definitions for TM1651 and can be changed to other ports       
#define DIO 16 //A3

// Clock outputs
long count = -1; // Input clock counter, -1 in order to go to 0 no the first pulse
const int DIVISIONS[] {2, 3, 4, 5, 6, 8, 16, 32}; // Divisions of the outputs
//const int OUTPUTS[] {12, 11, 10, 9, 8, 7, 6, 5}; // Output pins
const int OUTPUTS[] {5, 6, 7, 8, 9, 10, 11, 12}; // Output pins


//Rotary encoder inputs
#define encoderA   18 //A4
#define encoderB   19 //A5
#define encoderBTN 2  

float MaxBPM = 300.0;
float BPM = 120.0;
// float msPerMin = 1000 * 60;

// Millis
unsigned long currentMillis = 0;
unsigned long prevMillis = 0;
unsigned long O2PrevMillis = 0;
// unsigned long countMillis = 0;

// Coarse/granular mode
// 0 = Coarse mode (change BPM by 5), 1 = granular mode (change BPM by 1)
int bpmStepSize = 5;  // Initial step size

// Divisions
int n = 0; // Number of divisions

// put function declarations here:
TM1651 Display(CLK,DIO);
// RotaryEncoder Encoder(encoderA, encoderB, RotaryEncoder::LatchMode::TWO03);
// NewEncoder Encoder(encoderA, encoderB, -20, 20, 0, )
NewEncoder encoder;



void setup() {
  // DEBUG - Serial printout
  encoder.begin(encoderA, encoderB, encoderBTN, 20);
  Serial.begin(9600);
  // Setup the LED display
  Display.displaySet(2);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  Display.displayDP(0); // Turn off decimal point. This is buggy, so we should turn it off.

  // Set BPM to 120 as a default setting. 
  BPM = 120.0; 

  // Number of divisions
	n = sizeof(DIVISIONS) / sizeof(DIVISIONS[0]);

  // set pin modes
  pinMode(LED_BUILTIN, OUTPUT);
  // pinMode(OUT1, OUTPUT);
  // pinMode(OUT2, OUTPUT);

  for (int i = 0; i < n; i++) {
		pinMode(OUTPUTS[i], OUTPUT);
		digitalWrite(OUTPUTS[i], LOW);
	}
}


unsigned long lastMillis = 0;
void loop() {
  // Show BPM on display
  Display.displayInteger(BPM); 

  // Get latest status of the encoder
  encoder.Update(); 
  static int encPos = 0;
  int encNewPos  = encoder.GetSteps();
  int encNewDir  = encoder.GetDirection();
  int encBtState = encoder.ButtonPressed();

  // // Calculate intervals. 60000 ms = 1 minute.
  unsigned long bpmInterval = (60000 / BPM) / 2;
  // // BPM / n = slower. BPM * n = faster. Maybe we can create outputs that are both negative and positive, relative to BPM?
  // unsigned long IVal1 = 60000 / BPM;
  // unsigned long IVal2 = 60000 / (BPM / 2);

  // Rotary encoder changes BPM, between 0 and MaxBPM (defined above)
  if (encPos != encNewPos) {
    if (encNewDir == 1) {
      BPM+=bpmStepSize;
      if (BPM>MaxBPM){
        BPM = MaxBPM;
      }
    } else if (encNewDir == 2) {
      BPM-=bpmStepSize;
      if (BPM<0) {
        BPM=0;
      }
    }

    // Reset the encoder
    encoder.Reset();
  }

  // Coarse/fine mode via encoder button
  // TODO: Set indicator light? Or maybe use a toggle switch?
  if (encBtState) {
    if (bpmStepSize == 5) {
      bpmStepSize = 1;
      digitalWrite(LED_BUILTIN, HIGH);
      
    } else {
      bpmStepSize = 5;
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  // Actual clock stuff
  unsigned long currentMillis = millis(); // Set current millis

  // // Output 1
  // if (currentMillis - O1PrevMillis >= IVal1) {
  //   digitalWrite(OUT1, HIGH);
  //   O1PrevMillis = currentMillis;
    
  // } else {
  //   digitalWrite(OUT1, LOW);
  // }

  // // Output2
  // if (currentMillis - O2PrevMillis >= IVal2) {
  //   digitalWrite(OUT2, HIGH);
  //   O2PrevMillis = currentMillis;
  // } else {
  //   digitalWrite(OUT2, LOW);
  // }

  if (currentMillis - prevMillis >= bpmInterval) {
    count++;
    prevMillis = currentMillis;
    // Serial.println(bpmInterval);
  }

  for (int i = 0; i < n; i++) {
      bool v;
      v = (count % DIVISIONS[i] == 0);
      digitalWrite(OUTPUTS[i], v ? HIGH : LOW);
      // Serial.print("output ");
      // Serial.print(OUTPUTS[i]);
      // Serial.print(":");
      // Serial.println(v);
      // Serial.println(bpmInterval);
  }

  
  
}

