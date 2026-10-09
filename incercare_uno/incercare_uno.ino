#include <QTRSensors.h>


//================ MOTOARE ================

#define RightMotorDir 12
#define RightMotorSpeedPin 3

#define LeftMotorDir 13
#define LeftMotorSpeedPin 11



#define MAX_SPEED 110
#define MIN_SPEED 15



//================ SENZORI =================

QTRSensors qtr;


const uint8_t SensorCount = 8;

uint16_t sensorValues[SensorCount];


const uint8_t qtrPins[SensorCount] =
{
  2,4,5,6,7,8,9,10
};




//================ PID =====================

float Kp = 0.55;
float Kd = 6.0;


float filteredError = 0;

int LastError = 0;



unsigned long lastDebug = 0;




//============== CONTROL MOTOARE ===========

void MotorControl(int leftSpeed,int rightSpeed)
{


  digitalWrite(LeftMotorDir,HIGH);
  digitalWrite(RightMotorDir,LOW);



  leftSpeed = constrain(leftSpeed,0,255);
  rightSpeed = constrain(rightSpeed,0,255);



  analogWrite(LeftMotorSpeedPin,leftSpeed);
  analogWrite(RightMotorSpeedPin,rightSpeed);

}






//================ SETUP ===================

void setup()
{

  Serial.begin(115200);


  pinMode(LeftMotorDir,OUTPUT);
  pinMode(LeftMotorSpeedPin,OUTPUT);


  pinMode(RightMotorDir,OUTPUT);
  pinMode(RightMotorSpeedPin,OUTPUT);



  qtr.setTypeRC();

  qtr.setSensorPins(qtrPins,SensorCount);



  delay(1000);



  Serial.println("Calibrare...");



  for(int i=0;i<500;i++)
  {
    qtr.calibrate();
    delay(5);
  }



  Serial.println("Gata");

}





//================ LOOP ====================

void loop()
{


  qtr.readCalibrated(sensorValues);



  long sum=0;
  long weightedSum=0;


  bool lineFound=false;



  for(int i=0;i<SensorCount;i++)
  {

    int value=sensorValues[i];

    value=constrain(value,0,1000);



    if(value>120)
      lineFound=true;



    sum+=value;


    weightedSum+=(long)value*(i*1000);

  }






  if(!lineFound || sum==0)
  {

    MotorControl(45,45);

    return;

  }







  int position = weightedSum/sum;


  int error = 3500-position;





  filteredError =
  filteredError*0.65 + error*0.35;





  int derivative =
  filteredError-LastError;



  int correction =
  Kp*filteredError+
  Kd*derivative;



  LastError=filteredError;




  correction =
  constrain(correction,-180,180);







  //=========== VITEZA =====================


  int absError=abs(error);



  int baseSpeed;



  if(absError < 250)
  {

    // drept

    baseSpeed=MAX_SPEED;

  }

  else if(absError < 700)
  {

    // curba usoara

    baseSpeed=85;

  }

  else if(absError < 1200)
  {

    // curba serioasa

    baseSpeed=65;

  }

  else
  {

    // curba foarte stransa

    baseSpeed=45;

  }








  int leftSpeed =
  baseSpeed-correction;


  int rightSpeed =
  baseSpeed+correction;







  //=========== AJUTOR CURBE ================


  if(error > 600)
  {

    // curba stanga

    leftSpeed = baseSpeed-45;

    rightSpeed = baseSpeed+25;

  }



  if(error < -600)
  {

    // curba dreapta

    rightSpeed = baseSpeed-45;

    leftSpeed = baseSpeed+25;

  }








  leftSpeed =
  constrain(leftSpeed,MIN_SPEED,MAX_SPEED);


  rightSpeed =
  constrain(rightSpeed,MIN_SPEED,MAX_SPEED);






  MotorControl(leftSpeed,rightSpeed);






  //============== DEBUG ====================


  if(millis()-lastDebug>100)
  {

    lastDebug=millis();


    Serial.print("POS=");
    Serial.print(position);


    Serial.print(" ERR=");
    Serial.print(error);


    Serial.print(" L=");
    Serial.print(leftSpeed);


    Serial.print(" R=");
    Serial.println(rightSpeed);

  }


}