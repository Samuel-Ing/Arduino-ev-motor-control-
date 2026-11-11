
/*=========================================================
          ARDUINO UNO - EV MOTOR CONTROL SYSTEM
=========================================================*/



// ---------------- PIN DEFINITIONS ----------------

const byte ACCELERATOR_PIN = A0;

const byte START_BUTTON = 2;
const byte BRAKE_BUTTON = 3;

const byte RPWM = 9;
const byte LPWM = 10;

const byte R_EN = 7;
const byte L_EN = 8;


// ---------------- MOTOR SETTINGS ----------------

const int MAX_PWM = 255;

const int DEAD_ZONE = 5;

const int RAMP_STEP = 2;

const unsigned long RAMP_INTERVAL = 10;

const unsigned long DEBOUNCE_DELAY = 50;

// Five-second brake reset
const unsigned long BRAKE_RESET_TIME = 5000;


// ---------------- SYSTEM VARIABLES ----------------

bool systemEnabled = false;

int acceleratorValue = 0;

int targetCommand = 0;

int motorCommand = 0;

// Software accelerator zero point
int acceleratorZeroPoint = 0;


// ---------------- TIMERS ----------------

unsigned long lastRampTime = 0;

unsigned long lastPrintTime = 0;


// ---------------- BUTTON VARIABLES ----------------

bool lastStartReading = HIGH;

bool stableStartState = HIGH;

unsigned long startDebounceTime = 0;


// ---------------- SETUP ----------------

void setup() {

  Serial.begin(9600);

  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(BRAKE_BUTTON, INPUT_PULLUP);

  pinMode(ACCELERATOR_PIN, INPUT);

  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);

  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);

  // Disable motor driver at startup
  digitalWrite(R_EN, LOW);
  digitalWrite(L_EN, LOW);

  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);

  // Initial potentiometer zero reference
  acceleratorZeroPoint = analogRead(ACCELERATOR_PIN);

  Serial.println("====================================");
  Serial.println("       EV CONTROL SYSTEM");
  Serial.println("====================================");

  Serial.println("SYSTEM: OFF");
  Serial.println("Press START to activate.");

}


// ---------------- START BUTTON ----------------

bool readStartButton() {

  bool reading = digitalRead(START_BUTTON);

  if (reading != lastStartReading) {

    startDebounceTime = millis();

    lastStartReading = reading;

  }

  if (millis() - startDebounceTime >= DEBOUNCE_DELAY) {

    if (reading != stableStartState) {

      stableStartState = reading;

      if (stableStartState == LOW) {
        return true;
      }

    }

  }

  return false;

}


// ---------------- MOTOR DRIVER ----------------

void enableMotorDriver() {

  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);

}


void disableMotorDriver() {

  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);

  digitalWrite(R_EN, LOW);
  digitalWrite(L_EN, LOW);

}


// ---------------- ACCELERATOR CONTROL ----------------

void readAccelerator() {

  acceleratorValue = analogRead(ACCELERATOR_PIN);

  // Calculate position relative to software zero
  int relativeValue =
      acceleratorValue - acceleratorZeroPoint;

  // Prevent negative acceleration command
  relativeValue = constrain(relativeValue, 0, 1023);

  // Convert relative position to PWM
  targetCommand = map(
    relativeValue,
    0,
    1023,
    0,
    MAX_PWM
  );

  // Apply dead zone
  if (targetCommand <= DEAD_ZONE) {

    targetCommand = 0;

  }

}


// ---------------- GRADUAL SPEED CONTROL ----------------

void updateMotorSpeed() {

  if (millis() - lastRampTime >= RAMP_INTERVAL) {

    lastRampTime = millis();

    // Gradual acceleration
    if (motorCommand < targetCommand) {

      motorCommand += RAMP_STEP;

      if (motorCommand > targetCommand) {

        motorCommand = targetCommand;

      }

    }

    // Gradual deceleration
    else if (motorCommand > targetCommand) {

      motorCommand -= RAMP_STEP;

      if (motorCommand < targetCommand) {

        motorCommand = targetCommand;

      }

    }

    // Apply PWM
    analogWrite(RPWM, motorCommand);

    analogWrite(LPWM, 0);

  }

}


// ---------------- GRADUAL BRAKING ----------------

void handleBrake() {

  unsigned long brakeStartTime = millis();

  bool fiveSecondReset = false;

  Serial.println();
  Serial.println("BRAKE ENGAGED");
  Serial.println("Gradual braking initiated...");

  // Set target speed to zero
  targetCommand = 0;


  // Continue while brake button is held
  while (digitalRead(BRAKE_BUTTON) == LOW) {

    // Gradually reduce motor speed
    updateMotorSpeed();

    // Detect five-second brake hold
    if (millis() - brakeStartTime >= BRAKE_RESET_TIME) {

      if (!fiveSecondReset) {

        fiveSecondReset = true;

        Serial.println();
        Serial.println("5 SECOND BRAKE HOLD DETECTED");

      }

    }

    delay(1);

  }


  Serial.println("BRAKE RELEASED");


  // Complete gradual deceleration
  // even if brake is released early

  while (motorCommand > 0) {

    targetCommand = 0;

    updateMotorSpeed();

    delay(1);

  }


  // Ensure motor PWM reaches zero
  motorCommand = 0;
  targetCommand = 0;

  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);


  // Five-second software zero calibration
  if (fiveSecondReset) {

    // Capture current potentiometer position
    acceleratorZeroPoint =
        analogRead(ACCELERATOR_PIN);

    Serial.println("------------------------------------");

    Serial.println("ACCELERATION RESET COMPLETE");

    Serial.print("NEW ZERO POINT: ");

    Serial.println(acceleratorZeroPoint);

    Serial.println("Motor PWM: 0");

    Serial.println("System remains ON.");

    Serial.println("Accelerator ready from new zero.");

    Serial.println("------------------------------------");

  }

  else {

    Serial.println("Motor stopped.");
    Serial.println("System remains ON.");

  }

  lastRampTime = millis();

}


// ---------------- MAIN PROGRAM ----------------

void loop() {

  // Read START button
  bool startPressed = readStartButton();


  // ---------------------------------------
  // START CONTROL
  // ---------------------------------------

  if (startPressed && !systemEnabled &&
      digitalRead(BRAKE_BUTTON) == HIGH) {

    systemEnabled = true;

    motorCommand = 0;
    targetCommand = 0;

    enableMotorDriver();

    lastRampTime = millis();

    // Establish initial zero reference
    acceleratorZeroPoint =
        analogRead(ACCELERATOR_PIN);

    Serial.println();
    Serial.println("START PRESSED");

    Serial.println("SYSTEM: ON");

    Serial.println("Waiting for accelerator...");

  }


  // ---------------------------------------
  // BRAKE CONTROL
  // ---------------------------------------

  if (systemEnabled &&
      digitalRead(BRAKE_BUTTON) == LOW) {

    handleBrake();

  }


  // ---------------------------------------
  // ACCELERATOR CONTROL
  // ---------------------------------------

  if (systemEnabled &&
      digitalRead(BRAKE_BUTTON) == HIGH) {

    readAccelerator();

    updateMotorSpeed();

  }


  // ---------------------------------------
  // SERIAL MONITOR
  // ---------------------------------------

  if (millis() - lastPrintTime >= 200) {

    lastPrintTime = millis();

    Serial.print("Accelerator: ");

    Serial.print(acceleratorValue);

    Serial.print(" | Zero Point: ");

    Serial.print(acceleratorZeroPoint);

    Serial.print(" | Target PWM: ");

    Serial.print(targetCommand);

    Serial.print(" | Motor PWM: ");

    Serial.print(motorCommand);

    Serial.print(" | System: ");

    Serial.print(systemEnabled ? "ON" : "OFF");

    Serial.print(" | Brake: ");

    Serial.println(
      digitalRead(BRAKE_BUTTON) == LOW
      ? "ACTIVE"
      : "RELEASED"
    );

  }

}
