#include <Wire.h>
#include <Servo.h>

// mmpu
#define MPU_ADDR 0x68
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;


#define S1 A0
#define S2 A1
#define S3 A2
#define S4 A3

// Variabile MPU
float accX, accY, accZ;
float gyroX, gyroY, gyroZ;

float pitch = 0;
float roll = 0;

unsigned long lastTime;
float dt;

// PID constante doamne ajuta!!
float Kp = 2.5;
float Ki = 0.01;
float Kd = 1.2;

float errorPitch, errorRoll;
float lastErrorPitch = 0;
float lastErrorRoll = 0;

float integralPitch = 0;
float integralRoll = 0;

// ținte (vertical = 0)
float targetPitch = 0;
float targetRoll = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // MPU init
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0); // wake up
  Wire.endTransmission(true);

  servo1.attach(3);
  servo2.attach(5);
  servo3.attach(6);
  servo4.attach(9);

  lastTime = millis();
}

void loop() {
  readMPU();
  computeAngles();
  readSensors();
  controlPID();
}

void readMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  accX = (Wire.read() << 8 | Wire.read()) / 16384.0;
  accY = (Wire.read() << 8 | Wire.read()) / 16384.0;
  accZ = (Wire.read() << 8 | Wire.read()) / 16384.0;

  Wire.read(); Wire.read(); // temp

  gyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
  gyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  gyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
}

void computeAngles() {
  unsigned long now = millis();
  dt = (now - lastTime) / 1000.0;
  lastTime = now;

  float accPitch = atan2(accY, accZ) * 180 / PI;
  float accRoll  = atan2(-accX, accZ) * 180 / PI;

  // filtru complementar
  pitch = 0.98 * (pitch + gyroX * dt) + 0.02 * accPitch;
  roll  = 0.98 * (roll  + gyroY * dt) + 0.02 * accRoll;
}

void readSensors() {
  int s1 = analogRead(S1);
  int s2 = analogRead(S2);
  int s3 = analogRead(S3);
  int s4 = analogRead(S4);

  // exemplu: corecție direcțională simplă
  float biasX = (s1 - s2) * 0.001;
  float biasY = (s3 - s4) * 0.001;

  targetRoll += biasX;
  targetPitch += biasY;
}

void controlPID() {
  errorPitch = targetPitch - pitch;
  errorRoll  = targetRoll  - roll;

  integralPitch += errorPitch * dt;
  integralRoll  += errorRoll * dt;

  float derivativePitch = (errorPitch - lastErrorPitch) / dt;
  float derivativeRoll  = (errorRoll  - lastErrorRoll) / dt;

  float outputPitch = Kp * errorPitch + Ki * integralPitch + Kd * derivativePitch;
  float outputRoll  = Kp * errorRoll  + Ki * integralRoll  + Kd * derivativeRoll;

  lastErrorPitch = errorPitch;
  lastErrorRoll  = errorRoll;

  // control servo 
  int s1_out = constrain(90 + outputPitch + outputRoll, 0, 180);
  int s2_out = constrain(90 + outputPitch - outputRoll, 0, 180);
  int s3_out = constrain(90 - outputPitch + outputRoll, 0, 180);
  int s4_out = constrain(90 - outputPitch - outputRoll, 0, 180);

  servo1.write(s1_out);
  servo2.write(s2_out);
  servo3.write(s3_out);
  servo4.write(s4_out);

  Serial.print("Pitch: "); Serial.print(pitch);
  Serial.print(" Roll: "); Serial.println(roll);
}