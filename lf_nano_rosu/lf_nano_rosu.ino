#include <QTRSensors.h>

QTRSensors qtr;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];


// ================= PID =================
float Kp = 0.08;
float Ki = 0.0;
float Kd = 0.35;

long integral = 0;
int lastError = 0;


// viteza
int baseSpeed = 140;


// ================= L298N =================

const int ENA = 10;
const int IN1 = A3;
const int IN2 = 13;

const int ENB = 11;
const int IN3 = A0;
const int IN4 = A1;


// Buzzer

const int BUZZER = A7;



// ================= MOTOARE =================


void motorLeft(int speed)
{
  speed = constrain(speed,-255,255);

  if(speed >= 0)
  {
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
  }
  else
  {
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
    speed = -speed;
  }

  analogWrite(ENA,speed);
}



void motorRight(int speed)
{
  speed = constrain(speed,-255,255);

  if(speed >= 0)
  {
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);
  }
  else
  {
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,HIGH);
    speed = -speed;
  }

  analogWrite(ENB,speed);
}



// ================= SETUP =================


void setup()
{
  Serial.begin(115200);


  pinMode(BUZZER,OUTPUT);


  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);

  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);

  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);



  qtr.setTypeRC();

  qtr.setSensorPins(
    (const uint8_t[]){2,3,4,5,6,7,8,9},
    SensorCount
  );



  tone(BUZZER,2000);
  delay(150);
  noTone(BUZZER);



  Serial.println("Calibrare...");


  // misca robotul stanga-dreapta peste linie
  for(int i=0;i<500;i++)
  {
    qtr.calibrate();
    delay(5);
  }



  Serial.println("START");


  tone(BUZZER,2000);
  delay(500);
  noTone(BUZZER);


  delay(1000);
}



// ================= LOOP =================


void loop()
{

  uint16_t position = qtr.readLineBlack(sensorValues);



  int error = position - 3500;



  // elimina micile vibratii pe centru

  if(abs(error)<80)
  {
    error = 0;
  }



  // ================= PID =================


  integral += error;

  integral = constrain(integral,-4000,4000);



  if(abs(error)>2000)
  {
    integral = 0;
  }



  // filtru derivativ

  int derivative = (error - lastError) * 0.5;



  int correction =
      (Kp * error) +
      (Kd * derivative) +
      (Ki * integral);



  lastError = error;



  // ================= CONTROL VITEZA =================


  int speed = baseSpeed - abs(error)/50;


  speed = constrain(speed,90,baseSpeed);



  int leftSpeed =
      speed + correction;


  int rightSpeed =
      speed - correction;



  leftSpeed = constrain(leftSpeed,-200,200);
  rightSpeed = constrain(rightSpeed,-200,200);



  motorLeft(leftSpeed);
  motorRight(rightSpeed);



  // ================= DEBUG =================


  Serial.print("Pos:");
  Serial.print(position);

  Serial.print(" Err:");
  Serial.print(error);

  Serial.print(" Corr:");
  Serial.print(correction);

  Serial.print(" L:");
  Serial.print(leftSpeed);

  Serial.print(" R:");
  Serial.println(rightSpeed);



  delay(5);

}