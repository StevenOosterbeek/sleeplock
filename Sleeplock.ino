/*/////////////////////

    Sleeplock - v.0.0.1
    Steven Oosterbeek
    Rotterdam University of Applied Sciences

    -------------
    Sleeplock is a system which automatically locks a door when his user lies down on bed.
    As a result, the user does not need to worry about whether or not he has locked the door - especially useful for single older people.

    -------------
    Arduino Mega 2560, Step motor 28BYJ-48, Force sensitive resistor (2), LED (2), button

/////////////////////*/

int personWeight = 100;
boolean personOnBed = false;
boolean doorMayClose = true;
boolean doorShouldBeClosing = false;
boolean doorIsClosed = false;

//
// LEDS
const int ledBlue = 4; // On when the door may be closed
const int ledRed = 5; // On when door is closed, off when door is open

//
// Step motor
int stepCounter = 0;
const int motorPinOne = 8; // Blue
const int motorPinTwo = 9; // Pink
const int motorPinThree = 10; // Yellow
const int motorPinFour = 11; // Orange
const int motorSpeed = 1200;
const int motorSteps = 512;
const int pinLookup[8] = { B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001 };

//
// Force sensitive resistors (analog in)
const int fsrPinOne = 0;
const int fsrPinTwo = 1;
int fsrLeftValue;
int fsrRightValue;

//
// Button
const int buttonPin = 34;

void setup() {

    // Leds pins
    pinMode(ledBlue, OUTPUT);
    pinMode(ledRed, OUTPUT);

    // Default: door is open and may be closed
    digitalWrite(ledBlue, HIGH);
    digitalWrite(ledRed, LOW);

    // Step motor pins
    pinMode(motorPinOne, OUTPUT);
    pinMode(motorPinTwo, OUTPUT);
    pinMode(motorPinThree, OUTPUT);
    pinMode(motorPinFour, OUTPUT);

    // Button pins
    pinMode(buttonPin, INPUT);

}

void loop() {

    fsrLeftValue = analogRead(fsrPinTwo);
    fsrRightValue = analogRead(fsrPinOne);

    // Check if weight is more than the calibrated weight
    if ((fsrRightValue - 50) >= personWeight && (fsrLeftValue - 50) >= personWeight) personOnBed = true;

    // Close door?
    if (personOnBed) {

        stepCounter++;
        doorShouldBeClosing = (stepCounter <= motorSteps);

        if (doorShouldBeClosing && doorMayClose && !doorIsClosed) {

            for (int i = 7; i >= 0; i--) {
                stepMotorOutput(i);
                delayMicroseconds(motorSpeed);
            }

            if (stepCounter == motorSteps) {
                // Door is now closed
                doorIsClosed = true;
                doorMayClose = false;
            }

        } else {

            // Keep checking if the person has gone out of bed
            if (fsrRightValue < personWeight && fsrLeftValue < personWeight) {
                personOnBed = false;
                doorMayClose = true;
                stepCounter = 0; // Reset step motor
            }

        }

    }

    // Door has been unlocked manually when the button is pressed
    if (digitalRead(buttonPin) == HIGH) doorIsClosed = false;

    // LED logic
    if (doorIsClosed) digitalWrite(ledRed, HIGH);
    else digitalWrite(ledRed, LOW);

    if (doorMayClose) digitalWrite(ledBlue, HIGH);
    else digitalWrite(ledBlue, LOW);

}

void stepMotorOutput(int out) {
  digitalWrite(motorPinOne, bitRead(pinLookup[out], 0));
  digitalWrite(motorPinTwo, bitRead(pinLookup[out], 1));
  digitalWrite(motorPinThree, bitRead(pinLookup[out], 2));
  digitalWrite(motorPinFour, bitRead(pinLookup[out], 3));
}