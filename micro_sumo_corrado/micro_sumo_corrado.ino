/* JSumo Micro Sumo Robot */
//nr ore pierdute pt a face codu`sa mearga relativ bine = ~73 h :)
#define Right_Op_Sensor A0
#define Front_Op_Sensor A1
#define Left_Op_Sensor A2

#define Line_Sensor A3

#define Right_Button 8
#define Left_Button 12
#define Led1 4
#define Led2 6

#define Right_Motor_Direction 10
#define Right_Motor_Speed 9
#define Left_Motor_Direction 13
#define Left_Motor_Speed 5

int Speed2 = 200;
int Last_Value = 1;

int Line_Threshold = 700;


// Control motoare
void Motor(int LeftMotorValue, int RightMotorValue) {

  if (LeftMotorValue < 0) {
    LeftMotorValue = abs(LeftMotorValue);
    digitalWrite(Right_Motor_Direction, LOW);
    analogWrite(Right_Motor_Speed, LeftMotorValue);
  }
  else if (LeftMotorValue > 0) {
    digitalWrite(Right_Motor_Direction, HIGH);
    analogWrite(Right_Motor_Speed, 255 - LeftMotorValue);
  }
  else {
    digitalWrite(Right_Motor_Direction, HIGH);
    analogWrite(Right_Motor_Speed, 255);
  }

  if (RightMotorValue < 0) {
    RightMotorValue = abs(RightMotorValue);
    digitalWrite(Left_Motor_Direction, LOW);
    analogWrite(Left_Motor_Speed, RightMotorValue);
  }
  else if (RightMotorValue > 0) {
    digitalWrite(Left_Motor_Direction, HIGH);
    analogWrite(Left_Motor_Speed, 255 - RightMotorValue);
  }
  else {
    digitalWrite(Left_Motor_Direction, HIGH);
    analogWrite(Left_Motor_Speed, 255);
  }
}


// Opreste motoarele
void MotorStop() {

  digitalWrite(Left_Motor_Direction, HIGH);
  analogWrite(Left_Motor_Speed, 255);

  digitalWrite(Right_Motor_Direction, HIGH);
  analogWrite(Right_Motor_Speed, 255);
}


void setup() {

  pinMode(Left_Button, INPUT_PULLUP);
  pinMode(Right_Button, INPUT_PULLUP);

  pinMode(Left_Op_Sensor, INPUT_PULLUP);
  pinMode(Right_Op_Sensor, INPUT_PULLUP);
  pinMode(Front_Op_Sensor, INPUT_PULLUP);

  pinMode(Left_Motor_Direction, OUTPUT);
  pinMode(Right_Motor_Direction, OUTPUT);

  pinMode(Left_Motor_Speed, OUTPUT);
  pinMode(Right_Motor_Speed, OUTPUT);

  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
}


void loop() {

  // Asteapta startul
  while (digitalRead(Right_Button) == 1 &&
         digitalRead(Left_Button) == 1) {

    MotorStop();
  }

  // Alegem directia de cautare
  if (digitalRead(Right_Button) == 0)
    Last_Value = 2;

  if (digitalRead(Left_Button) == 0)
    Last_Value = 0;


  // Numara 5 secunde
  for (int x = 0; x < 10; x++) {

    if (x % 2 == 0) {
      digitalWrite(Led1, HIGH);
      digitalWrite(Led2, LOW);
    }
    else {
      digitalWrite(Led1, LOW);
      digitalWrite(Led2, HIGH);
    }

    delay(500);
  }


  Motor(Speed2, Speed2);


  while (digitalRead(Right_Button) == 1 &&
         digitalRead(Left_Button) == 1) {
//thanks chat cpt 
    // Verifica marginea neagra
    if (analogRead(Line_Sensor) > Line_Threshold) {

      delay(15);

      if (analogRead(Line_Sensor) > Line_Threshold) {

        // Da inapoi
        Motor(-Speed2, -Speed2);
        delay(150);

        // Se intoarce
        Motor(-Speed2, Speed2);
        delay(200);

        // Merge din nou inainte
        Motor(Speed2, Speed2);
      }
    }

    // Senzorul din fata vede adversarul
    else if (digitalRead(Front_Op_Sensor) == 1) {

      Motor(Speed2, Speed2);
      Last_Value = 1;
    }

    // Senzorul din stanga vede adversarul
    else if (digitalRead(Left_Op_Sensor) == 1 &&
             digitalRead(Right_Op_Sensor) == 0) {

      Motor(-Speed2, Speed2);
      Last_Value = 0;
    }

    // Senzorul din dreapta vede adversarul
    else if (digitalRead(Left_Op_Sensor) == 0 &&
             digitalRead(Right_Op_Sensor) == 1) {
          //nigger
      Motor(Speed2, -Speed2);
      Last_Value = 2;
    }

    // Daca nu vede nimic, continua in ultima directie sper ...
    else if (Last_Value == 0) {

      Motor(-Speed2, Speed2);
    }

    else if (Last_Value == 1) {

      Motor(Speed2, Speed2);
    }

    else if (Last_Value == 2) {

      Motor(Speed2, -Speed2);
    }
  }


  // Asteapta sa fie eliberat butonul
  delay(100);

  while (digitalRead(Right_Button) == 0 ||
         digitalRead(Left_Button) == 0);

  delay(100);
} 