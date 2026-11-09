// TIMER BOT — Arduino Uno + L293D
// Same pin assignments

const int ENA = 5;
const int IN1 = 8;
const int IN2 = 9;

const int ENB = 6;
const int IN3 = 10;
const int IN4 = 11;

const int SPEED = 255;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void moveBot(int left1, int left2, int right1, int right2) {
  digitalWrite(IN1, left1);
  digitalWrite(IN2, left2);
  digitalWrite(IN3, right1);
  digitalWrite(IN4, right2);

  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
}

void stopBot() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void loop() {
  // 1. Backward — 5 seconds
  moveBot(LOW, HIGH, LOW, HIGH);
  delay(5000);

  // 2. Forward — 5 seconds
  moveBot(HIGH, LOW, HIGH, LOW);
  delay(5000);

  // 3. Stop — 5 seconds
  stopBot();
  delay(5000);

  // 4. Left turn — 5 seconds
  moveBot(LOW, HIGH, HIGH, LOW);
  delay(5000);

  // 5. Right turn — 5 seconds
  moveBot(HIGH, LOW, LOW, HIGH);
  delay(5000);
}