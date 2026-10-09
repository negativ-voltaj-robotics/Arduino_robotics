#include <NewPing.h>

// ------------------- Senzor ultrasonic -------------------

#define TRIG_PIN 8
#define ECHO_PIN 9
#define MAX_DISTANCE 60

#define DETECT_DISTANCE 40
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);


// ------------------- Motoare -------------------

#define DIR_A 12
#define PWM_A 3

#define DIR_B 13
#define PWM_B 11


#define SEARCH_SPEED 200
#define ATTACK_SPEED 255


// timp cat continua atacul dupa ce pierde adversarul
#define ATTACK_MEMORY 300

unsigned long lastSeen = 0;



// ------------------- Functii motoare -------------------

void stopMotors()
{
  analogWrite(PWM_A,0);
  analogWrite(PWM_B,0);
}


void attack()
{
  digitalWrite(DIR_A,HIGH);
  digitalWrite(DIR_B,LOW);

  analogWrite(PWM_A,ATTACK_SPEED);
  analogWrite(PWM_B,ATTACK_SPEED);
}


void search()
{
  digitalWrite(DIR_A,HIGH);
  digitalWrite(DIR_B,HIGH);

  analogWrite(PWM_A,SEARCH_SPEED);
  analogWrite(PWM_B,SEARCH_SPEED);
}



// ------------------- Setup -------------------

void setup()
{
  Serial.begin(9600);


  pinMode(DIR_A,OUTPUT);
  pinMode(PWM_A,OUTPUT);

  pinMode(DIR_B,OUTPUT);
  pinMode(PWM_B,OUTPUT);


  stopMotors();

  Serial.println("START");

  delay(3000);
}



// ------------------- Loop -------------------

void loop()
{

  int distance = sonar.ping_cm();


  if(distance == 0)
  {
    distance = MAX_DISTANCE;
  }


  Serial.print("Distanta: ");
  Serial.println(distance);



  // adversarul este vazut
  if(distance <= DETECT_DISTANCE)
  {
    lastSeen = millis();

    Serial.println("ATAC");
    attack();
  }


  // nu il vede, dar tocmai l-a vazut
  else if(millis() - lastSeen < ATTACK_MEMORY)
  {
    Serial.println("CONTINUA ATAC");
    attack();
  }


  // nu mai exista tinta
  else
  {
    Serial.println("CAUTA");
    search();
  }


  delay(20);
}