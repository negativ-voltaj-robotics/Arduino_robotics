#include <QTRSensors.h>

#define RightBaseSpeed 60
#define LeftBaseSpeed 60

// Motoare
#define RightMotorDir 12
#define RightMotorSpeedPin 3
#define LeftMotorDir 13
#define LeftMotorSpeedPin 11

QTRSensors qtr;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

const uint8_t qtrPins[SensorCount] = {A0, A1, 5, 6, 7, 8, 9, 10};

float Kp = 0.3;
float Kd = 5.0;
int LastError = 0;

unsigned long lastDebug = 0;

//========================================

void MotorControl(int LeftMotorSpeed, int RightMotorSpeed)
{
  // Motor stânga
  if (LeftMotorSpeed >= 0)
    digitalWrite(LeftMotorDir, HIGH);
  else
  {
    digitalWrite(LeftMotorDir, LOW);
    LeftMotorSpeed = -LeftMotorSpeed;
  }

  // Motor dreapta
  if (RightMotorSpeed >= 0)
    digitalWrite(RightMotorDir, LOW);
  else
  {
    digitalWrite(RightMotorDir, HIGH);
    RightMotorSpeed = -RightMotorSpeed;
  }

  LeftMotorSpeed = constrain(LeftMotorSpeed, 0, 255);
  RightMotorSpeed = constrain(RightMotorSpeed, 0, 255);

  analogWrite(LeftMotorSpeedPin, LeftMotorSpeed);
  analogWrite(RightMotorSpeedPin, RightMotorSpeed);
}

void setup()
{
  Serial.begin(9600);

  pinMode(RightMotorDir, OUTPUT);
  pinMode(RightMotorSpeedPin, OUTPUT);
  pinMode(LeftMotorDir, OUTPUT);
  pinMode(LeftMotorSpeedPin, OUTPUT);

  qtr.setTypeRC();
  qtr.setSensorPins(qtrPins, SensorCount);

  delay(1500);

  Serial.println("Calibrare...");

  for (int i = 0; i < 300; i++)
  {
    qtr.calibrate();
    delay(10);
  }

  Serial.println("Calibrare terminata!");
  delay(1000);
}

void loop()
{
  uint16_t position = qtr.readLineBlack(sensorValues);

  // eroare normalizată
  int Error = (2500 - (int)position) / 10;

  int P = Error;
  int D = Error - LastError;

  int SpeedUpdate = Kp * P + Kd * D;

  LastError = Error;

  int RightMotorSpeed = RightBaseSpeed + SpeedUpdate;
  int LeftMotorSpeed = LeftBaseSpeed - SpeedUpdate;

  RightMotorSpeed = constrain(RightMotorSpeed, -100, 200);
  LeftMotorSpeed = constrain(LeftMotorSpeed, -100, 200);

  MotorControl(LeftMotorSpeed, RightMotorSpeed);

  if (millis() - lastDebug > 150)
  {
    lastDebug = millis();

    Serial.print("Pos: ");
    Serial.print(position);

    Serial.print(" | Err: ");
    Serial.print(Error);

    Serial.print(" | L: ");
    Serial.print(LeftMotorSpeed);

    Serial.print(" | R: ");
    Serial.println(RightMotorSpeed);
  }
}