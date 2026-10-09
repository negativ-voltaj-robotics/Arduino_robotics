// ============================================================
// 16 QTR + MUX + ARDUINO NANO + TB6612
// LINE FOLLOWER - CONTROL DIFERIT: WEIGHTED + FILTERED PD
// ============================================================

// ===================== MUX =====================

const byte S0 = 2;
const byte S1 = 3;
const byte S2 = 4;
const byte S3 = 5;

const byte SENSOR_PIN = A0;


// ===================== TB6612 =====================

const byte STBY = 6;

const byte AIN1 = 7;
const byte AIN2 = 8;
const byte PWMA = 9;

const byte PWMB = 10;
const byte BIN1 = 11;
const byte BIN2 = 12;


// ===================== CONTROL =====================

// Nu mai folosim Ki
float Kp = 0.42;
float Kd = 2.8;

// Viteza normală
int baseSpeed = 95;

// Limită absolută
int maxSpeed = 210;

// Viteză minimă când robotul corectează
int minSpeed = 35;


// ===================== SENZORI =====================

// Stânga -> dreapta
const int position[16] = {
  -750, -650, -550, -450,
  -350, -250, -150,  -50,
    50,  150,  250,  350,
   450,  550,  650,  750
};


// Valorile tale aproximative
const int sensorMin[16] = {
  250, 280, 280, 300,
  260, 550, 600, 600,
   40,  50, 100,  80,
  220, 250, 300, 400
};

const int sensorMax[16] = {
  520, 550, 550, 570,
  540, 850, 850, 850,
  450, 450, 500, 500,
  550, 580, 600, 700
};


// ===================== VARIABILE =====================

float filteredPosition = 0;
float previousPosition = 0;

unsigned long previousTime = 0;

int lastDirection = 1;


// ============================================================
// SELECTARE MUX
// ============================================================

void selectMux(byte channel) {

  digitalWrite(S0, channel & 0x01);
  digitalWrite(S1, channel & 0x02);
  digitalWrite(S2, channel & 0x04);
  digitalWrite(S3, channel & 0x08);

  delayMicroseconds(15);
}


// ============================================================
// CITIRE SENZOR
// ============================================================

int readSensor(byte channel) {

  selectMux(channel);

  return analogRead(SENSOR_PIN);
}


// ============================================================
// NORMALIZARE
// ============================================================

int normalizeSensor(byte i, int value) {

  int low = sensorMin[i];
  int high = sensorMax[i];

  // Valorile mici reprezintă linia
  int strength = map(value, low, high, 1000, 0);

  return constrain(strength, 0, 1000);
}


// ============================================================
// CALCUL POZIȚIE
// ============================================================

int calculatePosition() {

  long weighted = 0;
  long sum = 0;

  int strongestSensor = -1;
  int strongestValue = 0;


  for(byte i = 0; i < 16; i++) {

    int raw = readSensor(i);

    int strength = normalizeSensor(i, raw);

    if(strength > strongestValue) {
      strongestValue = strength;
      strongestSensor = i;
    }

    weighted += (long)strength * position[i];
    sum += strength;
  }


  // Nu avem linie
  if(sum < 700) {

    if(lastDirection < 0)
      return -850;
    else
      return 850;
  }


  int pos = weighted / sum;


  // Memorează direcția
  if(pos < -40)
    lastDirection = -1;

  if(pos > 40)
    lastDirection = 1;


  return pos;
}


// ============================================================
// CONTROL MOTOARE
// ============================================================

void motorLeft(int speed) {

  speed = constrain(speed, -255, 255);

  if(speed >= 0) {

    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);

    analogWrite(PWMA, speed);

  } else {

    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);

    analogWrite(PWMA, -speed);
  }
}


void motorRight(int speed) {

  speed = constrain(speed, -255, 255);

  if(speed >= 0) {

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);

    analogWrite(PWMB, speed);

  } else {

    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);

    analogWrite(PWMB, -speed);
  }
}


// ============================================================
// SETARE MOTOARE
// ============================================================

void setMotors(int left, int right) {

  motorLeft(left);
  motorRight(right);
}


// ============================================================
// STOP
// ============================================================

void stopMotors() {

  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}


// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(115200);


  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(STBY, OUTPUT);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);


  digitalWrite(STBY, HIGH);

  stopMotors();

  delay(1000);

  previousTime = micros();
}


// ============================================================
// LOOP
// ============================================================

void loop() {

  // ----------------------------------------------------------
  // 1. CITIM POZIȚIA
  // ----------------------------------------------------------

  int positionNow = calculatePosition();


  // ----------------------------------------------------------
  // 2. FILTRARE
  // ----------------------------------------------------------

  // Filtru simplu:
  // 70% poziția veche
  // 30% poziția nouă

  filteredPosition =
      filteredPosition * 0.70 +
      positionNow * 0.30;


  // ----------------------------------------------------------
  // 3. EROARE
  // ----------------------------------------------------------

  float error = filteredPosition;


  // ----------------------------------------------------------
  // 4. DERIVATIVĂ
  // ----------------------------------------------------------

  unsigned long now = micros();

  float dt = (now - previousTime) / 1000000.0;

  previousTime = now;

  if(dt < 0.001)
    dt = 0.001;

  if(dt > 0.05)
    dt = 0.05;


  float derivative =
      (filteredPosition - previousPosition) / dt;

  previousPosition = filteredPosition;


  // ----------------------------------------------------------
  // 5. PD
  // ----------------------------------------------------------

  float correction =
      Kp * error +
      Kd * derivative;


  // Limităm corecția
  correction = constrain(correction, -180, 180);


  // ----------------------------------------------------------
  // 6. VITEZĂ DINAMICĂ
  // ----------------------------------------------------------

  // Cu cât suntem mai departe de centru,
  // cu atât încetinim robotul.

  int absError = abs((int)error);

  int dynamicSpeed =
      baseSpeed - map(
        constrain(absError, 0, 750),
        0,
        750,
        0,
        55
      );


  dynamicSpeed =
      constrain(dynamicSpeed, minSpeed, baseSpeed);


  // ----------------------------------------------------------
  // 7. MOTOR MIXING
  // ----------------------------------------------------------

  int leftSpeed =
      dynamicSpeed + correction;

  int rightSpeed =
      dynamicSpeed - correction;


  // ----------------------------------------------------------
  // 8. LIMITARE
  // ----------------------------------------------------------

  leftSpeed =
      constrain(leftSpeed, -maxSpeed, maxSpeed);

  rightSpeed =
      constrain(rightSpeed, -maxSpeed, maxSpeed);


  // ----------------------------------------------------------
  // 9. MOTOARE
  // ----------------------------------------------------------

  setMotors(leftSpeed, rightSpeed);


  // ----------------------------------------------------------
  // DEBUG
  // ----------------------------------------------------------

  Serial.print("POS:");
  Serial.print(positionNow);

  Serial.print(" FIL:");
  Serial.print(filteredPosition);

  Serial.print(" ERR:");
  Serial.print(error);

  Serial.print(" DER:");
  Serial.print(derivative);

  Serial.print(" COR:");
  Serial.print(correction);

  Serial.print(" L:");
  Serial.print(leftSpeed);

  Serial.print(" R:");
  Serial.println(rightSpeed);
}