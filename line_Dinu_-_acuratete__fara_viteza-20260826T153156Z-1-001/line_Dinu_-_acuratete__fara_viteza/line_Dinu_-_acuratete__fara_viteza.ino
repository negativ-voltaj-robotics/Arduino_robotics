#include <PololuQTRSensors.h>
#include <OrangutanMotors.h>
#include <OrangutanLEDs.h>
#include <Servo.h> 

Servo myservo; 
  
unsigned char qtr_rc_pins[] = {IO_D4,IO_C0, IO_C1, IO_C2, IO_C3, IO_C4, IO_C5, IO_D7};
unsigned char qtr_rc_count  = 8;       
unsigned int  qtr_rc_values[8] = {0};
int center = (( (qtr_rc_count) * 1000) / 2);

float KP = 5 , KI = 100 , KD = 25/1;

int fan = 0;

int   integral  = 0;
int   last_proportional = 0;

void followPID()
{
    int position = qtr_read_line(qtr_rc_values, QTR_EMITTERS_ON);

    

    int proportional = position - center;

    int derivative = proportional - last_proportional;

    int power_difference = proportional / KP + integral / KI + derivative * KD;
    last_proportional    = proportional;
    integral  += proportional;


    const int max = 200;
    const int max_diffrence = 60;
    const int factor_diffrence = 2;

    if(power_difference > max)
        power_difference = max;
    if(power_difference < -max)
        power_difference = -max;

    int leftMotorSpeed  = max;
    int rightMotorSpeed = max-power_difference;

    if(power_difference < 0)
    {
        leftMotorSpeed  = max+power_difference;
        rightMotorSpeed = max;
    }


    if(leftMotorSpeed - rightMotorSpeed > max_diffrence)
    {
        leftMotorSpeed -= (leftMotorSpeed - rightMotorSpeed)/factor_diffrence;
    } 
    else if(rightMotorSpeed - leftMotorSpeed > max_diffrence)
    {
        rightMotorSpeed -= (rightMotorSpeed - leftMotorSpeed)/factor_diffrence;
    }

    set_motors(leftMotorSpeed,rightMotorSpeed);

}
void loop(){}


void setup()
{
    myservo.attach(9);  

    myservo.write(20);
    delay(100);

    set_digital_input(IO_D4, PULL_UP_ENABLED);

    while(is_digital_input_high(IO_B0)) {} 
    
    qtr_rc_init(qtr_rc_pins,qtr_rc_count, 2000, 255); 

    red_led(1);
    delay(1000);

    int i;
      for (i = 0; i < 100; i++)  
      {
        if(i%25==0)
        {
            if((i/5)%2==0) set_motors(30,-30); 
                else set_motors(-30,30); 
        }

        if(i%5)
        {
            if((i/5)%2==0) red_led(0);
                else red_led(1); 
        }
        qtr_calibrate(QTR_EMITTERS_ON);
        delay(20);
      }
 
    set_motors(0,0); 
    red_led(1);

    while(is_digital_input_high(IO_B0)) {} 
    delay(1000);
    

    while(1)
    {
     followPID();
    }

}
