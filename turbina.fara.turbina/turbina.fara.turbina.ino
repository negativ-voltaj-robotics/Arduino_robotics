// ===== SENZORI =====
#define S1 A0
#define S2 A1
#define S3 A2
#define S4 A3
#define S5 A4

// ===== MOTOARE =====
#define M1_PWM 5
#define M1_DIR 7

#define M2_DIR 8
#define M2_PWM 9

// Viteza de bază (0-255)
int viteza = 100;

void setup() {

  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(S5, INPUT);

  pinMode(M1_PWM, OUTPUT);
  pinMode(M1_DIR, OUTPUT);

  pinMode(M2_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);

  stopMotoare();
}

void loop() {

  int s1 = digitalRead(S1);
  int s2 = digitalRead(S2);
  int s3 = digitalRead(S3);
  int s4 = digitalRead(S4);
  int s5 = digitalRead(S5);

  // Linie perfect pe centru
  if (s3 == 0 && s2 == 1 && s4 == 1) {
    vitezaMotoare(viteza, viteza);
  }

  // Ușor stânga
  else if (s2 == 0) {
    vitezaMotoare(40, viteza + 30);
  }

  // Ușor dreapta
  else if (s4 == 0) {
    vitezaMotoare(viteza + 30, 40);
  }

  // Curba puternică stânga
  else if (s1 == 0) {
    vitezaMotoare(0, 180);
  }

  // Curba puternică dreapta
  else if (s5 == 0) {
    vitezaMotoare(180, 0);
  }

  // Intersecție sau mai mulți senzori pe negru
  else if (s2 == 0 && s3 == 0 && s4 == 0) {
    vitezaMotoare(viteza, viteza);
  }

  // Linie pierdută
  else {
    stopMotoare();
  }
}

// =========================

void vitezaMotoare(int stanga, int dreapta) {

  digitalWrite(M1_DIR, LOW);
  digitalWrite(M2_DIR, LOW);

  analogWrite(M1_PWM, constrain(stanga, 0, 255));
  analogWrite(M2_PWM, constrain(dreapta, 0, 255));
}

void stopMotoare() {
  analogWrite(M1_PWM, 0);
  analogWrite(M2_PWM, 0);
}