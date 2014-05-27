/*/////////////////////

    Sleeplock - v.0.0.2
    Steven Oosterbeek
    Rotterdam University of Applied Sciences

    -------------
    Sleeplock is a system which automatically locks a door when his user lies down on bed.
    As a result, the user does not need to worry about whether or not he has locked the door - especially useful for single older people.

    -------------
    Arduino Mega 2560, Step motor 28BYJ-48, Force sensitive resistor (2), LED (2), button

/////////////////////*/

boolean testMode = false; // Set to true for testing purposes
boolean sensorDebugging = false; // Set to true for outputting FSR values in monitor

int personWeight;
boolean isCalibrating = true;
boolean personOnBed = false;
boolean doorMayClose = true;
boolean doorShouldBeClosing = false;
boolean doorIsClosed = false;

//
// LEDS
const int ledYellow = 3; // Calibration indicator
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
    pinMode(ledYellow, OUTPUT);
    pinMode(ledBlue, OUTPUT);
    pinMode(ledRed, OUTPUT);

    // Step motor pins
    pinMode(motorPinOne, OUTPUT);
    pinMode(motorPinTwo, OUTPUT);
    pinMode(motorPinThree, OUTPUT);
    pinMode(motorPinFour, OUTPUT);

    // Button pins
    pinMode(buttonPin, INPUT);

    if (testMode || sensorDebugging) Serial.begin(9600);

}

void loop() {

    fsrLeftValue = analogRead(fsrPinTwo);
    fsrRightValue = analogRead(fsrPinOne);

    if (sensorDebugging) {
        Serial.println("--------- FSR VALUES ---------");
        Serial.print("Sensor right: ");
        Serial.println(fsrRightValue);
        Serial.print("Sensor left: ");
        Serial.println(fsrLeftValue);
        Serial.println("------- END FSR VALUES -------");
        Serial.println("");
        delay(1000);
    }

    if (isCalibrating) {

        // Someone has layed down on the bed for calibration
        if (fsrLeftValue > 20 && fsrRightValue > 20) {

            digitalWrite(ledYellow, HIGH);
            delay(5000);

            // Measure sensors again and only set weight when person did not get out of bed
            int secondFsrLeftValue = analogRead(fsrPinTwo);
            int secondFsrRightValue = analogRead(fsrPinOne);

            if (secondFsrLeftValue > 20 && secondFsrRightValue > 20) {

                // Always set lowest measured weight
                if (secondFsrLeftValue < secondFsrRightValue) personWeight = secondFsrLeftValue;
                else if (secondFsrLeftValue > secondFsrRightValue) personWeight = secondFsrRightValue;
                else personWeight = secondFsrLeftValue;

                digitalWrite(ledYellow, LOW);
                digitalWrite(ledBlue, HIGH);
                isCalibrating = false;

                if (testMode) {
                    Serial.print("Sleeplock has been calibrated > ");
                    Serial.print(personWeight);
                }

            }

        } else {

            if (testMode) Serial.println("Sleeplock is waiting for calibration..");

            digitalWrite(ledBlue, LOW);
            digitalWrite(ledRed, LOW);

            digitalWrite(ledYellow, LOW);
            delay(350);
            digitalWrite(ledYellow, HIGH);
            delay(350);

        }

    } else {

        // Check if weight is more than the calibrated weight
        if ((fsrLeftValue - 50) >= personWeight && (fsrRightValue - 50) >= personWeight) personOnBed = true;

        // Close door?
        if (personOnBed) {

            stepCounter++;
            doorShouldBeClosing = (stepCounter <= motorSteps);

            if (doorShouldBeClosing && doorMayClose && !doorIsClosed) {

                for (int i = 7; i >= 0; i--) {
                    stepMotorOutput(i);
                    delayMicroseconds(motorSpeed);
                }

                // Door is now closed
                if (stepCounter == motorSteps) {
                    doorIsClosed = true;
                    doorMayClose = false;
                    if (testMode) Serial.println("The door has been closed.");
                }

            } else {

                // Keep checking if the person has gone out of bed
                if (fsrLeftValue < personWeight && fsrRightValue < personWeight) {
                    personOnBed = false;
                    doorMayClose = true;
                    stepCounter = 0; // Reset step motor
                    if (testMode) Serial.println("The person has left the bed, the door may be closed once again.");
                }

            }

        }

        // Clicking the button will unlock the door, keeping the button pressed will restart the calibration
        if (digitalRead(buttonPin) == HIGH) {

            doorIsClosed = false;
            digitalWrite(ledRed, LOW);

            delay(1500); // Keep button pressed for 1,5 seconds for restarting calibration
            if (digitalRead(buttonPin) == HIGH) isCalibrating = true;
            else if (testMode) Serial.println("Someone has unlocked the door manualy.");

        }

        // LED logic
        if (doorIsClosed) digitalWrite(ledRed, HIGH);
        if (doorMayClose) digitalWrite(ledBlue, HIGH);
        else digitalWrite(ledBlue, LOW);

    }

}

void stepMotorOutput(int out) {
  digitalWrite(motorPinOne, bitRead(pinLookup[out], 0));
  digitalWrite(motorPinTwo, bitRead(pinLookup[out], 1));
  digitalWrite(motorPinThree, bitRead(pinLookup[out], 2));
  digitalWrite(motorPinFour, bitRead(pinLookup[out], 3));
}