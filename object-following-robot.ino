// ===============================
// OBJECT FOLLOWING BOT
// Arduino UNO + L293D + HC-SR04
// ===============================

// Ultrasonic sensor
#define TRIG_PIN 2
#define ECHO_PIN 3

// L293D motor inputs
#define IN1 6
#define IN2 7
#define IN3 10
#define IN4 11

// Distance limits
#define FOLLOW_DISTANCE 20
#define STOP_DISTANCE 8

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.begin(9600);

  stopMotors();
}

void loop() {

  long duration;
  int distance;

  // Send ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  // Read echo
  duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // Convert to cm
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // -------------------------------
  // OBJECT FOLLOWING LOGIC
  // -------------------------------

  if (distance == 0) {
    // No valid object detected
    stopMotors();
  }

  else if (distance <= STOP_DISTANCE) {
    // Too close
    stopMotors();
  }

  else if (distance <= FOLLOW_DISTANCE) {
    // Object detected at suitable distance
    forward();
  }

  else {
    // Object too far / not detected
    stopMotors();
  }

  delay(50);
}


// ===============================
// MOTOR FUNCTIONS
// ===============================

void forward() {

  // Left motor
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Right motor
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void stopMotors() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
