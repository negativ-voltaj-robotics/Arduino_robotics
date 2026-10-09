// ==========================
// Test motoare Raspberry Pi Pico + L298N
// Arduino IDE
// ==========================

// ---------- Driver L298N ----------
const int IN1 = 14;
const int IN2 = 15;

const int IN3 = 16;
const int IN4 = 17;

const int ENA = 19;
const int ENB = 20;

// viteza motoarelor (0-255)
int speedMotor = 150;

// ==========================

void setup()
{
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  Serial.println("Test motoare L298N");

  stopMotors();
  delay(1000);
}

// ==========================

void forward()
{
  // Motor stanga fata
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Motor dreapta fata
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, speedMotor);
  analogWrite(ENB, speedMotor);

  Serial.println("FATA");
}

// ==========================

void backward()
{
  // Motor stanga spate
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  // Motor dreapta spate
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, speedMotor);
  analogWrite(ENB, speedMotor);

  Serial.println("SPATE");
}

// ==========================

void stopMotors()
{
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  Serial.println("STOP");
}

// ==========================

void loop()
{
  forward();
  delay(3000);   // merge inainte 3 secunde

  stopMotors();
  delay(1000);

  backward();
  delay(3000);   // merge inapoi 3 secunde

  stopMotors();
  delay(2000);
}