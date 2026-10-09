
int m1a = 6;
int m1b = 7;
int m2a = 8;
int m2b = 9;


int s1 = A1;
int s2 = A2;
int s3 = A3;
int s4 = A4;
int s5 = A5;

void setup() {
  pinMode(m1a, OUTPUT);
  pinMode(m1b, OUTPUT);
  pinMode(m2a, OUTPUT);
  pinMode(m2b, OUTPUT);

  pinMode(s1, INPUT);
  pinMode(s2, INPUT);
  pinMode(s3, INPUT);
  pinMode(s4, INPUT);
  pinMode(s5, INPUT);
}

void loop() {
  int v1 = digitalRead(s1);
  int v2 = digitalRead(s2);
  int v3 = digitalRead(s3);
  int v4 = digitalRead(s4);
  int v5 = digitalRead(s5);




  if (v3 == LOW && v2 == HIGH && v4 == HIGH) {
    inainte();
  }


  else if (v2 == LOW) {
    stangaUsor();
  }


  else if (v1 == LOW) {
    stangaPuternic();
  }


  else if (v4 == LOW) {
    dreaptaUsor();
  }


  else if (v5 == LOW) {
    dreaptaPuternic();
  }

  else {
    stopMotoare();
  }
}



void inainte() {
  digitalWrite(m1a, HIGH);
  digitalWrite(m1b, LOW);
  digitalWrite(m2a, HIGH);
  digitalWrite(m2b, LOW);
}

void stangaUsor() {
  digitalWrite(m1a, LOW);
  digitalWrite(m1b, LOW);
  digitalWrite(m2a, HIGH);
  digitalWrite(m2b, LOW);
}

void stangaPuternic() {
  digitalWrite(m1a, LOW);
  digitalWrite(m1b, HIGH);
  digitalWrite(m2a, HIGH);
  digitalWrite(m2b, LOW);
}

void dreaptaUsor() {
  digitalWrite(m1a, HIGH);
  digitalWrite(m1b, LOW);
  digitalWrite(m2a, LOW);
  digitalWrite(m2b, LOW);
}

void dreaptaPuternic() {
  digitalWrite(m1a, HIGH);
  digitalWrite(m1b, LOW);
  digitalWrite(m2a, LOW);
  digitalWrite(m2b, HIGH);
}

void stopMotoare() {
  digitalWrite(m1a, LOW);
  digitalWrite(m1b, LOW);
  digitalWrite(m2a, LOW);
  digitalWrite(m2b, LOW);
}