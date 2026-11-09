
#include <SoftwareSerial.h>

// HC-05 Bluetooth
// HC-05 TXD -> Arduino D2
// HC-05 RXD -> Arduino D3 through voltage divider
SoftwareSerial BT(2, 3);

// L293D wiring — unchanged
const int ENA = 5;
const int IN1 = 8;
const int IN2 = 9;

const int ENB = 6;
const int IN3 = 10;
const int IN4 = 11;

const int SPEED = 255;

char lastDirection = 'S';

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  BT.begin(9600);
  Serial.begin(9600);

  stopBot();
  Serial.println("Bluetooth Bot Ready");
}

void loop() {
  if (BT.available()) {
    char cmd = BT.read();

    switch (cmd) {
      case 'F':
      case 'f':
        lastDirection = 'F';
        forward();
        break;

      case 'B':
      case 'b':
        lastDirection = 'B';
        backward();
        break;

      case 'L':
      case 'l':
        turnLeft();
        break;

      case 'R':
      case 'r':
        turnRight();
        break;

      case 'S':
      case 's':
        lastDirection = 'S';
        stopBot();
        break;
    }
  }
}

void forward() {
  // Both motors forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
}

void backward() {
  // Both motors backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
}

void turnLeft() {
  // Stop left motor
  analogWrite(ENA, 0);

  // Right motor continues in the previous direction
  if (lastDirection == 'F') {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, SPEED);
  }
  else if (lastDirection == 'B') {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, SPEED);
  }
}

void turnRight() {
  // Stop right motor
  analogWrite(ENB, 0);

  // Left motor continues in the previous direction
  if (lastDirection == 'F') {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, SPEED);
  }
  else if (lastDirection == 'B') {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, SPEED);
  }
}

void stopBot() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
