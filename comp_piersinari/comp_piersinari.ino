// =====================================================
// 8 QTR ANALOG + ARDUINO NANO + TB6612FNG
// LINE FOLLOWER PID + TURBINA ESC
// =====================================================

#include <Servo.h>

// ---------------- Senzori QTR analog ----------------

const int SensorCount = 8;

const int sensorPins[SensorCount] = {
  A0, A1, A2, A3, A4, A5, A6, A7
};

int sensorValues[SensorCount];

// ---------------- PID ----------------

float Kp = 1.5;
float Ki = 0.0002;
float Kd = 1.0;

long integral = 0;
int lastError = 0;

// ---------------- Viteza ----------------

int baseSpeed = 150;


// =====================================================
// TURBINĂ / ESC
// =====================================================

const int TURBINE_PIN = 11;

Servo turbine;

// 1000 = minim
// 2000 = maxim
//
// Începe cu 1000-1100 pentru test.
// Poți crește ulterior.
int turbineSpeed = 1600;


// =====================================================
// TB6612FNG
// =====================================================

// Motor A
const int PWMA = 5;
const int AIN1 = 7;
const int AIN2 = 8;

// Motor B
const int PWMB = 6;
const int BIN1 = 9;
const int BIN2 = 10;

// Standby
const int STBY = 4;


// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  // ---------------- TB6612 ----------------

  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);

  // Direcția înainte
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);


  // ---------------- TURBINĂ / ESC ----------------

  turbine.attach(TURBINE_PIN);

  // Semnal minim pentru armarea ESC-ului
  turbine.writeMicroseconds(1000);

  delay(3000);

  // Pornește turbina la valoarea setată
  turbine.writeMicroseconds(turbineSpeed);


  // ---------------- Senzori ----------------

  for (int i = 0; i < SensorCount; i++)
  {
    pinMode(sensorPins[i], INPUT);
  }

  delay(1000);

  Serial.println("START");
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
  // ---------------- CITIRE SENZORI ----------------

  long weightedSum = 0;
  long sum = 0;

  for (int i = 0; i < SensorCount; i++)
  {
    sensorValues[i] = analogRead(sensorPins[i]);

    // Negru = valoare mai mare
    int value = sensorValues[i];

    // Elimină zgomotul foarte mic
    if (value < 50)
      value = 0;

    weightedSum += (long)value * (i * 1000);
    sum += value;
  }


  // ---------------- POZIȚIA LINIEI ----------------

  int position;

  if (sum > 0)
  {
    position = weightedSum / sum;
  }
  else
  {
    // Linia a fost pierdută
    // continuă în direcția ultimei erori

    if (lastError < 0)
      position = 0;
    else
      position = 7000;
  }


  // ---------------- PID ----------------

  // Centrul celor 8 senzori = 3500
  int error = position - 3500;

  integral += error;

  integral = constrain(
    integral,
    -50000,
    50000
  );

  int derivative = error - lastError;

  float correction =
      (Kp * error) +
      (Ki * integral) +
      (Kd * derivative);

  // Limitare corecție
  correction = constrain(
    correction,
    -255,
    255
  );

  lastError = error;


  // ---------------- MOTOARE ----------------

  int leftMotor =
      baseSpeed + correction;

  int rightMotor =
      baseSpeed - correction;

  leftMotor = constrain(
    leftMotor,
    0,
    255
  );

  rightMotor = constrain(
    rightMotor,
    0,
    255
  );

  analogWrite(PWMA, leftMotor);
  analogWrite(PWMB, rightMotor);


  // ---------------- TURBINĂ ----------------

  turbine.writeMicroseconds(turbineSpeed);


  // ---------------- DEBUG ----------------

  Serial.print("Senzori: ");

  for (int i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print(" ");
  }

  Serial.print(" | Pos: ");
  Serial.print(position);

  Serial.print(" | Error: ");
  Serial.print(error);

  Serial.print(" | L: ");
  Serial.print(leftMotor);

  Serial.print(" | R: ");
  Serial.print(rightMotor);

  Serial.print(" | Turbina: ");
  Serial.println(turbineSpeed);
}