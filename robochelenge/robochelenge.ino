#include <QTRSensors.h>

QTRSensors qtr;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

// PID
float Kp = 0.18;
float Ki = 0.0002;
float Kd = 1.38;

long integral = 0;
int lastError = 0;

// viteza de bază
int baseSpeed = 150;

// motoare
const int PWMA = 9;
const int PWMB = 11;

const int AIN1 = 12;
const int AIN2 = A4;

const int BIN1 = A1;
const int BIN2 = A2;

const int STBY = A3;

// Buzzer
const int BUZZER = 13;

void beep(int durata = 150)
{
  tone(BUZZER, 2000);   // 2 kHz
  delay(durata);
  noTone(BUZZER);
}

void setup()
{
  Serial.begin(115200);

  pinMode(BUZZER, OUTPUT);

  // Semnal la pornire
  beep(200);
  delay(150);
  beep(200);

  qtr.setTypeRC();

  qtr.setSensorPins(
    (const uint8_t[]){2,3,4,5,6,7,8,A5},
    SensorCount);

  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);

  // înainte
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  delay(1000);

  Serial.println("Calibrare...");

  // calibrare
  for (int i = 0; i < 1000; i++)
  {
    qtr.calibrate();
    delay(5);
  }

  Serial.println("Start");

  // Semnal că începe traseul
  beep(500);

  delay(1000);
}

void loop()
{
  uint16_t position = qtr.readLineBlack(sensorValues);

  int error = position - 3500;

  // PID
  integral += error;
  integral = constrain(integral, -50000, 50000);

  int derivative = error - lastError;

  float correction =
      (Kp * error) +
      (Ki * integral) +
      (Kd * derivative);

  correction = constrain(correction, -255, 255);

  lastError = error;

  int leftMotor = baseSpeed + correction;
  int rightMotor = baseSpeed - correction;

  leftMotor = constrain(leftMotor, 0, 255);
  rightMotor = constrain(rightMotor, 0, 255);

  analogWrite(PWMA, leftMotor);
  analogWrite(PWMB, rightMotor);
}