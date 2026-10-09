#include <QTRSensors.h>
#include <Servo.h>

QTRSensors qtr;
Servo esc;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

// PID
float Kp = 0.1;
float Ki = 0.00002;
float Kd = 1;

long integral = 0;
int lastError = 0;

// viteza de bază
int baseSpeed = 160;

// motoare
const int PWMA = 3;
const int PWMB = 11;

const int AIN1 = 10;
const int AIN2 = 9;

const int BIN1 = 13;
const int BIN2 = 12;

const int STBY = A0;
// ESC turbina
const int ESC_PIN = A5;

// valoare turbina
const int TURBINE_SPEED = 1300; // ajustează între 1000-2000 µs

void setup()
{
  Serial.begin(115200);

  // ESC
  esc.attach(ESC_PIN);

  // turbina oprită
  esc.writeMicroseconds(1000);

  qtr.setTypeRC();

  qtr.setSensorPins(
    (const uint8_t[]){0,1,2,4,5,6,A3,A4},
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

  // turbina rămâne oprită în timpul calibrării
  for (int i = 0; i < 500; i++)
  {
    qtr.calibrate();
    delay(5);
  }

  Serial.println("Start");

  // pornește turbina după calibrare
  esc.writeMicroseconds(TURBINE_SPEED);

  delay(2000); // timp pentru armarea ESC-ului dacă este necesar
}

void loop()
{
  uint16_t position = qtr.readLineBlack(sensorValues);

  int error = position - 3500;

  // PID
  integral += error;
  integral = constrain(integral, -100000, 100000);

  int derivative = error - lastError;

  float correction =
      (Kp * error) +
      (Ki * integral) +
      (Kd * derivative);

  lastError = error;

  int leftMotor = baseSpeed + correction;
  int rightMotor = baseSpeed - correction;

  leftMotor = constrain(leftMotor, 0, 200);
  rightMotor = constrain(rightMotor, 0, 200);

  analogWrite(PWMA, leftMotor);
  analogWrite(PWMB, rightMotor);

  // menține turbina pornită
  esc.writeMicroseconds(TURBINE_SPEED);
}