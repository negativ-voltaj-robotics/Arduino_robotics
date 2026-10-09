//================ ARDUMOTO ==================

#define DIRA 12
#define PWMA 3

#define DIRB 13
#define PWMB 11



//================ ULTRASONIC ================

#define trig1 A0
#define echo1 A1



const int attackDistance = 70;

int targetCount = 0;



//================ SETUP =====================

void setup()
{
  Serial.begin(9600);


  pinMode(DIRA, OUTPUT);
  pinMode(PWMA, OUTPUT);

  pinMode(DIRB, OUTPUT);
  pinMode(PWMB, OUTPUT);


  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);


  stopMotors();

  delay(3000);
}



//================ LOOP ======================

void loop()
{

  float distance = readDistance();


  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" cm");



  // confirmare tinta
  if(distance <= attackDistance)
  {

    targetCount++;


    if(targetCount >= 3)
    {
      attack();
    }

  }
  else
  {

    targetCount = 0;

    search();

  }


}



//================ CITIRE DISTANTA ============

float readDistance()
{

  long total = 0;
  int valid = 0;



  for(int i = 0; i < 5; i++)
  {

    digitalWrite(trig1, LOW);
    delayMicroseconds(2);


    digitalWrite(trig1, HIGH);
    delayMicroseconds(10);


    digitalWrite(trig1, LOW);



    long duration = pulseIn(echo1, HIGH, 25000);



    if(duration > 0)
    {

      total += duration * 0.0343 / 2;

      valid++;

    }


    delay(10);

  }



  if(valid == 0)
  {
    return 999;
  }



  return total / valid;

}



//================ CAUTARE ====================

void search()
{

  // rotire pe loc

  analogWrite(PWMA,150);
  analogWrite(PWMB,150);


  digitalWrite(DIRA,HIGH);

  digitalWrite(DIRB,LOW);

}



//================ ATAC =======================

void attack()
{

  // mers înainte

  analogWrite(PWMA,255);
  analogWrite(PWMB,255);


  digitalWrite(DIRA,HIGH);

  digitalWrite(DIRB,HIGH);

}



//================ STOP =======================

void stopMotors()
{

  analogWrite(PWMA,0);  	
  analogWrite(PWMB,0);

}