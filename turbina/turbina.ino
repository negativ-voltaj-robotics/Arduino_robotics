#include <QTRSensors.h>
#include <Servo.h>

QTRSensors qtr;
Servo esc;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

// PID
float Kp = 0.185;
float Ki = 0.0002;
float Kd = 1.3489;

long integral = 0;
int lastError = 0; 

// viteza de bază
int baseSpeed = 255;

// motoare
const int PWMA = 9;
const int PWMB = 11;

const int AIN1 = 12;
const int AIN2 = A4;

const int BIN1 = A1;
const int BIN2 = A2;

const int STBY = A3;

// ESC turbina
const int ESC_PIN = 13;

const int ESC_STOP = 1000;
const int ESC_RUN  = 1450;

void setup()
{
  Serial.begin(115200);

  // ESC
  esc.attach(ESC_PIN);

  // armare ESC
  for (int i = 0; i < 300; i++)
  {
    esc.writeMicroseconds(ESC_STOP);
    delay(20);
  }

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

  // calibrare cu turbina oprită
  for (int i = 0; i < 700; i++)
  {
    qtr.calibrate();
    esc.writeMicroseconds(ESC_STOP);
    delay(5);
  }

  Serial.println("Start");

  // pornește turbina
  esc.writeMicroseconds(ESC_RUN);

  delay(1000);
}

void loop()
{
  // menține turbina pornită
  esc.writeMicroseconds(ESC_RUN);

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

  // limitează corecția
  correction = constrain(correction, -255, 255);

  lastError = error;

  int leftMotor = baseSpeed + correction;
  int rightMotor = baseSpeed - correction;

  leftMotor = constrain(leftMotor, 0, 255);
  rightMotor = constrain(rightMotor, 0, 255);

  analogWrite(PWMA, leftMotor);
  analogWrite(PWMB, rightMotor);
}