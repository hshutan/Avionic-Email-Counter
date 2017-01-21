// This program will drive a modified Turbin Gas Temp dial from a Boeing aircraft
// The part number of the avionics/dial is 65003-005
// The indicator dial and built in display are driven by serial commands.
// All electronics are 5 volt. <500mA total max current, USB safe.

// The original servo is used (5 wire unipolar) -- driven by a ULN2803 chip
// The original three 7-segment Minitron-style incandescent displays are used -- driven by using the original shift registers
// Nov 2016 - Harrison Shutan

const int latchPin = 5; // Pulse this high to parallel push out shift register bits
const int clockPin = 6; // Shift register clock
const int dataPin = 12; // Shift register binary data pin
const int pwmPin = A0; // LOW, display off but retained in memory, HIGH, display on

#include <Stepper.h>
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 4, 8, 7, 9);

int currentLoc = 0;
int RequestedLoc = 0;

String inputString = "";         // a string to hold incoming serial data
boolean stringComplete = false;  // whether the string is complete

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  digitalWrite(pwmPin, HIGH); // PWM idea didn't work, just leave this high.
  Serial.begin(9600);

  blankDisplay();
  delay(500);
  writeNumbertoDisplay(888);
  resetIndicatorHome();
  delay(250);
  blankDisplay();
  //demoIndicatorHome();
  myStepper.setSpeed(170); // Nice, quiet, accurate speed.
}

void resetIndicatorHome() {
  // Reliable reset method.
  myStepper.setSpeed(170);
  myStepper.step(-130);
  delay(250);
  myStepper.step(-3);
  delay(1000);
  currentLoc = 0;
}

void demoIndicatorHome() {
  myStepper.setSpeed(170);
  delay(350);
  requestSpecificIndicatorPosition(1);
  delay(250);
  requestSpecificIndicatorPosition(2);
  delay(250);
  requestSpecificIndicatorPosition(3);
  delay(250);
  requestSpecificIndicatorPosition(4);
  delay(250);
  requestSpecificIndicatorPosition(5);
  delay(250);
  requestSpecificIndicatorPosition(6);
  delay(250);
  requestSpecificIndicatorPosition(7);
  delay(250);
  requestSpecificIndicatorPosition(8);
  delay(250);
  requestSpecificIndicatorPosition(9);
  delay(250);
  requestSpecificIndicatorPosition(10);
  delay(350);
  requestSpecificIndicatorPosition(0);
  delay(250);
}

void moveIndicatorBackward() {
  myStepper.step(-1);
}

void moveIndicatorForward() {
  myStepper.step(1);
}

void requestSpecificIndicatorPosition(int input) {
  if (input > 10) {
    return;
  }
  if (input < 0) {
    return;
  }
  if (input == currentLoc) {
    return;
  }

  if (currentLoc > input) {
    // We need to move backwards

    int clicks = getStepperNumber(currentLoc) - getStepperNumber(input);

    for (int i = 0; i < clicks; i++) {
      moveIndicatorBackward();
    }
    currentLoc = input;


  } else {
    // We need to move forwards
    int clicks = getStepperNumber(input) - getStepperNumber(currentLoc);

    for (int i = 0; i < clicks; i++) {
      moveIndicatorForward();
    }
    currentLoc = input;

  }
}

void blankDisplay() {
  shiftOut(dataPin, clockPin, LSBFIRST, B11111111);
  shiftOut(dataPin, clockPin, LSBFIRST, B11111111);
  shiftOut(dataPin, clockPin, LSBFIRST, B11111111);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
}

void writeNumbertoDisplay(int in) {
  if (in < 0) {
    return;
  }
  if (in > 999) {
    shiftOut(dataPin, clockPin, LSBFIRST, B11011111);
    shiftOut(dataPin, clockPin, LSBFIRST, B11011111);
    shiftOut(dataPin, clockPin, LSBFIRST, B11011111);
  } else if (in > 99) {
    int ones = (in % 10);
    int tens = ((in / 10) % 10);
    int hundreds = ((in / 100) % 10);
    shiftOut(dataPin, clockPin, LSBFIRST, getSRNumber(hundreds));
    shiftOut(dataPin, clockPin, LSBFIRST, getSRNumber(tens));
    shiftOut(dataPin, clockPin, LSBFIRST, getSRNumber(ones));
  } else if (in > 9) {
    int ones = (in % 10);
    int tens = ((in / 10) % 10);
    shiftOut(dataPin, clockPin, LSBFIRST, B11111111);
    shiftOut(dataPin, clockPin, LSBFIRST, getSRNumber(tens));
    shiftOut(dataPin, clockPin, LSBFIRST, getSRNumber(ones));


  } else {
    shiftOut(dataPin, clockPin, LSBFIRST, B11111111);
    shiftOut(dataPin, clockPin, LSBFIRST, getSRNumber(in));

    shiftOut(dataPin, clockPin, LSBFIRST, B11111111);

  }
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
}

byte getSRNumber(int in) {

  /*
    //All off: B11111111
    // 9: B00001011
    // 8: B00000000
    // 7: B00101111
    // 6: B10000000
    // 5: B10000001
    // 4: B01001011
    // 3: B00001101
    // 2: B00011100
    // 1: B01101111
    // 0: B00100000
     -a
    |b|c
     -d
    |e|f
     -g

    e: B11111110
    g: B11111101
    b: B11111011
    f: B11101111
    d: B11011111
    a: B10111111
    c: B01111111

  */


  switch (in) {
    case 0:
      return B00100000;
    case 1:
      return B01101111;
    case 2:
      return B00011100;
    case 3:
      return B00001101;
    case 4:
      return B01001011;
    case 5:
      return B10000001;
    case 6:
      return B10000000;
    case 7:
      return B00101111;
    case 8:
      return B00000000;
    case 9:
      return B00001011;
  }
}

int getStepperNumber(int in) {
  switch (in) {
    case 0:
      return 0;
    case 1:
      return 7;
    case 2:
      return 15;
    case 3:
      return 22;
    case 4:
      return 29;
    case 5:
      return 37;
    case 6:
      return 45;
    case 7:
      return 65;
    case 8:
      return 85;
    case 9:
      return 105;
    case 10:
      return 125;
  }
}

void loop() {

  // This section of if statements prevent the stepper from being driven at 100%
  // holding force when it is not moving (which is almost all of the time).
  // A very short off-delay of <20 Microseconds saves a ton
  // of power (100mA).
  
  if (digitalRead(4)) {
  digitalWrite(4, LOW);
    delayMicroseconds(5);
    digitalWrite(4, HIGH);
  }

  if (digitalRead(8)) {
  digitalWrite(8, LOW);
    delayMicroseconds(5);
    digitalWrite(8, HIGH);
  }

  if (digitalRead(7)) {
  digitalWrite(7, LOW);
    delayMicroseconds(5);
    digitalWrite(7, HIGH);
  }

  if (digitalRead(9)) {
  digitalWrite(9, LOW);
    delayMicroseconds(5);
    digitalWrite(9, HIGH);
  }

  if (stringComplete) {
  /*
     Menu system:
     b <enter> will blank the 7segment displays
     r <enter> will reset the servo motor to 0, as well as blank the 7seg display
     X,Y <enter> will set dial to X (0 thru 10 only), and 7seg displays to Y (0 thru 999 only).
  */
  if (inputString.indexOf('b') > -1) {
      blankDisplay();
    } else if (inputString.indexOf('r') > -1) {
      blankDisplay();
      resetIndicatorHome();
    } else if (inputString.indexOf('q') > -1) {
      moveIndicatorBackward();
    } else if (inputString.indexOf('a') > -1) {
      moveIndicatorForward();
    } else {
      // Traditional handling
      int commaIndex = inputString.indexOf(',');
      int endIndex = inputString.indexOf('.');
      String firstValue = inputString.substring(0, commaIndex);
      String secondValue = inputString.substring(commaIndex + 1, endIndex);
      int indicatorPosition = firstValue.toInt();
      int segmentNumber = secondValue.toInt();
      writeNumbertoDisplay(segmentNumber);
      requestSpecificIndicatorPosition(indicatorPosition);
    }
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

