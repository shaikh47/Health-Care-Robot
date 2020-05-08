#include <QTRSensors.h>

//#include <Wire.h> 
//#include <LiquidCrystal_I2C.h>

//LiquidCrystal_I2C lcd(0x27, 52, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // 2, 1, 0, 4, 5, 6, 7, 3

#define Kp 0.1 //  0.1
#define Kd 0.65 //  0.65
#define Ki 0.2 // 0.17
 
#define MaxSpeed 50// max speed of the robot 220
#define BaseSpeed 50 // this is the speed at which the motors should spin when the robot is perfectly on the line
#define NUM_SENSORS  10     // number of sensors used

#define speedturn 180

void forward(void);
void backward(void);
void hardleft(void);
void hardright(void);
void left(void);
void right(void);
void backleft(void);
void backright(void);
void wait(void);
//void move(int motor, int speed, int direction);

String str;
char data=0;
const byte interruptPin = 2;

int x=30;
int y=255;
int flag=0;  

int motorleft_forward = 9;
int motorleft_backward = 8;
int motorright_forward = 12;
int motorright_backward = 13;


int pwmleft = 10;
int pwmright = 11;

int led = 25;


int Read = 0;
float distance = 0.0;
float elapsedTime, time, timePrev;        //Variables for time control
float previous_error, error;
int period = 50;  //Refresh rate period of the loop is 50ms
unsigned int sensors[10];

QTRSensors qtr;

const uint8_t SensorCount = 10;
uint16_t sensorValues[SensorCount];

void setup()
{
  pinMode(motorleft_forward,OUTPUT);
  pinMode(motorleft_backward,OUTPUT);
  pinMode(motorright_forward,OUTPUT);
  pinMode(motorright_backward,OUTPUT);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(0, encoder, FALLING);
  //hardleft();
  //lcd.begin(16,2);
  //lcd.backlight();
  
  // configure the sensors
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){35,37,39,41,43,45,47,49,51,53}, SensorCount); //53,51,49,47,45,43,41,39,37,35
  //qtr.setEmitterPin(2);                                                             //35,37,39,41,43,45,47,49,51,53

    

    //lcd.setCursor(0,0); //we start writing from the first row first column
    //lcd.print("CALIBRATING!"); //16 characters poer line
    

  delay(500);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // turn on Arduino's LED to indicate we are in calibration mode
  analogWrite(11, 0); 
  analogWrite(10, 0); 
  
  
  // 2.5 ms RC read timeout (default) * 10 reads per calibrate() call
  // = ~25 ms per calibrate() call.
  // Call calibrate() 400 times to make calibration take about 10 seconds.
  for (uint16_t i = 0; i < 150; i++)
  {
    qtr.calibrate();
  }
  digitalWrite(LED_BUILTIN, LOW); // turn off Arduino's LED to indicate we are through with calibration
 
  
  // print the calibration minimum values measured when emitters were on
  Serial.begin(9600);
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.minimum[i]);
    Serial.print(' ');
  }
  Serial.println();

  // print the calibration maximum values measured when emitters were on
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.maximum[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
 
 //lcd.clear();
 //lcd.print("DONE!");
 wait();
 delay(1000);

    time = millis();
    //lcd.clear();
}

volatile long int count=0;
int lastError = 0;

void loop()
{
  int prev_position=0;
  //detachInterrupt(digitalPinToInterrupt(interruptPin));
  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  uint16_t position;
  position = qtr.readLineBlack(sensorValues);

  // print the sensor values as numbers from 0 to 1000, where 0 means maximum
  // reflectance and 1000 means minimum reflectance, followed by the line
  // position
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
   Serial.println(position);
   //Serial.print(" ");
   int error;
   //Serial.print(error);
//Serial.println("");

    int character[20];
    String myString = String(position);
    myString.concat("         ");
    //lcd.setCursor(0,0); //we start writing from the first row first column
    //lcd.print(myString); //16 characters per line
 
  //foreign code  
  
  error = position - 4500;   //3500
  int I = (I + error) * Ki;
  int motorSpeed = (Kp * error) + (Kd * (error - lastError)) + I;
  lastError = error;

  int rightMotorSpeed = BaseSpeed + motorSpeed;
  int leftMotorSpeed = BaseSpeed - motorSpeed;
  
  if (rightMotorSpeed > MaxSpeed ) rightMotorSpeed = MaxSpeed; // prevent the motor from going beyond max speed
  if (leftMotorSpeed > MaxSpeed ) leftMotorSpeed = MaxSpeed; // prevent the motor from going beyond max speed
  if (rightMotorSpeed < 0)rightMotorSpeed = 0;    
  if (leftMotorSpeed < 0)leftMotorSpeed = 0;


  //foreign code
  //handling case 1 ('+' intersection  and  'T'  like intersection  and  dead end)
  if((sensorValues[0]>100 && sensorValues[1]>100 &&sensorValues[2]>100 &&sensorValues[3]>100 &&sensorValues[4]>100 &&sensorValues[5]>100 &&sensorValues[6]>100 &&sensorValues[7]>100 &&sensorValues[8]>100 && sensorValues[9]>100)
      || (sensorValues[0]<100 && sensorValues[1]>100 &&sensorValues[2]>100 &&sensorValues[3]>100 &&sensorValues[4]>100 &&sensorValues[5]>100 &&sensorValues[6]>100 &&sensorValues[7]>100 &&sensorValues[8]>100 && sensorValues[9]>100)
      || (sensorValues[0]>100 && sensorValues[1]>100 &&sensorValues[2]>100 &&sensorValues[3]>100 &&sensorValues[4]>100 &&sensorValues[5]>100 &&sensorValues[6]>100 &&sensorValues[7]>100 &&sensorValues[8]>100 && sensorValues[9]<100)){
      steps_forward(24);   //after seeing plus intersection go forward for 15 steps
      wait();   //wait a bit
      position = qtr.readLineBlack(sensorValues);  //read the line again
      delay(100);  //wait for 100 ms
        //next to do list, not only make the bot go forward for certain steps but also add line following to it
      if((sensorValues[2]<100 && sensorValues[3]>100 && sensorValues[4]>100 && sensorValues[5]>100 && sensorValues[6]>100 && sensorValues[7]<100)
        ||(sensorValues[2]>100 && sensorValues[3]>100 && sensorValues[4]>100 && sensorValues[5]>100 && sensorValues[6]<100 && sensorValues[7]<100)
        ||(sensorValues[2]<100 && sensorValues[3]<100 && sensorValues[4]>100 && sensorValues[5]>100 && sensorValues[6]>100 && sensorValues[7]>100)
        ||(sensorValues[2]<100 && sensorValues[3]<100 && sensorValues[4]<100 && sensorValues[5]>100 && sensorValues[6]>100 && sensorValues[7]>100)
        ||(sensorValues[2]<100 && sensorValues[3]<100 && sensorValues[4]>100 && sensorValues[5]>100 && sensorValues[6]>100 && sensorValues[7]<100)
        ||(sensorValues[2]<100 && sensorValues[3]>100 && sensorValues[4]>100 && sensorValues[5]>100 && sensorValues[6]<100 && sensorValues[7]<100)
        ||(sensorValues[2]>100 && sensorValues[3]>100 && sensorValues[4]>100 && sensorValues[5]<100 && sensorValues[6]<100 && sensorValues[7]<100)){
        
        while((sensorValues[9]<500 || sensorValues[8]<500)){  //if the if is true which means it is '+' intersection
            position = qtr.readLineBlack(sensorValues);
            hardleft();
        }
        wait();
        delay(500);
      }
    }
  //handling case 2 (⌝ like section)
  if(sensorValues[0]<100 && sensorValues[1]<100 &&sensorValues[2]<100 &&sensorValues[3]<100 &&sensorValues[4]<100 &&sensorValues[5]<100 &&sensorValues[6]<100 &&sensorValues[7]<100 &&sensorValues[8]<100 && sensorValues[9]<100){
      steps_forward(15);
      wait();
      delay(100);
      while(sensorValues[3]<100 || sensorValues[4]<100){
          position = qtr.readLineBlack(sensorValues);
          hardleft();
      }
      wait();
      delay(500);
    }
  //handling case 3 (⌜ like symbol)
  if(sensorValues[0]<100 && sensorValues[1]<100 &&sensorValues[2]<100 &&sensorValues[3]<100 &&sensorValues[4]<100 &&sensorValues[5]<100 &&sensorValues[6]<100 &&sensorValues[7]<100 &&sensorValues[8]<100 && sensorValues[9]<100){
      steps_forward(15);
      wait();
      delay(100);
      while(sensorValues[3]<100 || sensorValues[4]<100){
          position = qtr.readLineBlack(sensorValues);
          hardleft();
      }
      wait();
      delay(500);
    }
  //handling case 4 (-|  like symbol)
  if(sensorValues[0]<100 && sensorValues[1]<100 &&sensorValues[2]<100 &&sensorValues[3]<100 &&sensorValues[4]<100 &&sensorValues[5]<100 &&sensorValues[6]<100 &&sensorValues[7]<100 &&sensorValues[8]<100 && sensorValues[9]<100){
      steps_forward(15);
      wait();
      delay(100);
      while(sensorValues[3]<100 || sensorValues[4]<100){
          position = qtr.readLineBlack(sensorValues);
          hardleft();
      }
      wait();
      delay(500);
    }
  //handling case 5 (|-  like symbol)
  if(sensorValues[0]<100 && sensorValues[1]<100 &&sensorValues[2]<100 &&sensorValues[3]<100 &&sensorValues[4]<100 &&sensorValues[5]<100 &&sensorValues[6]<100 &&sensorValues[7]<100 &&sensorValues[8]<100 && sensorValues[9]<100){
      steps_forward(15);
      wait();
      delay(100);
      while(sensorValues[3]<100 || sensorValues[4]<100){
          position = qtr.readLineBlack(sensorValues);
          hardleft();
      }
      wait();
      delay(500);
    }


  //handling the case for no line. when the robot see's no line(case 0)
    if(sensorValues[0]<100 && sensorValues[1]<100 &&sensorValues[2]<100 &&sensorValues[3]<100 &&sensorValues[4]<100 &&sensorValues[5]<100 &&sensorValues[6]<100 &&sensorValues[7]<100 &&sensorValues[8]<100 && sensorValues[9]<100){
      steps_forward(15);
      wait();
      delay(100);
      while(sensorValues[3]<100 || sensorValues[4]<100){
          position = qtr.readLineBlack(sensorValues);
          hardleft();
      }
      wait();
      delay(500);
    }
    
  

  

    //call this when straight on the line
  move(1, rightMotorSpeed, 1);//motor derecho hacia adelante
  move(0, leftMotorSpeed, 1);//motor izquierdo hacia adelante

  
 // Serial.print(leftMotorSpeed);
  //Serial.print("     ");
  //Serial.println(rightMotorSpeed);

  prev_position=position; 
}

void steps_hardleft(int steps){
  count=0;
      while(count<=steps){   //77steps for 180 degree
        hardleft();
      }
      wait();
      delay(500);
}

void steps_hardright(int steps){
  count=0;
      while(count<steps){   //77steps for 180 degree
        hardright();
      }
      wait();
      delay(500); 
}

void steps_forward(int steps){
  count=0;
      while(count<steps){
        forward();
      }
      wait();
      delay(500);
}

void encoder() {
  count++;
  //Serial.println(_count);
}

    


void move(int motor, int speed, int direction){
 

  boolean inPin1=HIGH;
  boolean inPin2=LOW;
  
  if(direction == 1){
    inPin1 = HIGH;
    inPin2 = LOW;
  }  
  if(direction == 0){
    inPin1 = LOW;
    inPin2 = HIGH;
  }

  if(motor == 0){
    digitalWrite(motorleft_forward, inPin1);
    digitalWrite(motorleft_backward, inPin2);
    analogWrite(pwmleft, speed);
  }
  if(motor == 1){
    digitalWrite(motorright_forward, inPin1);
    digitalWrite(motorright_backward, inPin2);
    analogWrite(pwmright, speed);
  }  
}


void forward()
{
  analogWrite(pwmleft,x);
  analogWrite(pwmright,x);
  digitalWrite(motorleft_forward,HIGH); 
  digitalWrite(motorleft_backward,LOW);
  digitalWrite(motorright_forward,HIGH);
  digitalWrite(motorright_backward,LOW);
  }
  void backward()
  {
    analogWrite(pwmleft,x);
  analogWrite(pwmright,x);
  digitalWrite(motorleft_forward,LOW); 
  digitalWrite(motorleft_backward,HIGH);
  digitalWrite(motorright_forward,LOW);
  digitalWrite(motorright_backward,HIGH);
    }
      void hardleft()
  {
    analogWrite(pwmleft,x);
  analogWrite(pwmright,x);
  digitalWrite(motorleft_forward,LOW); 
  digitalWrite(motorleft_backward,HIGH);
  digitalWrite(motorright_forward,HIGH);
  digitalWrite(motorright_backward,LOW);
    }
       void hardright()
  {
    analogWrite(pwmleft,x);
  analogWrite(pwmright,x);
  digitalWrite(motorleft_forward,HIGH); 
  digitalWrite(motorleft_backward,LOW);
  digitalWrite(motorright_forward,LOW);
  digitalWrite(motorright_backward,HIGH);
    }
     void left()
  {
    analogWrite(pwmleft,x);
  analogWrite(pwmright,x);
  digitalWrite(motorleft_forward,LOW); 
  digitalWrite(motorleft_backward,LOW);
  digitalWrite(motorright_forward,HIGH);
  digitalWrite(motorright_backward,LOW);
    }
    void right()
  {
    analogWrite(pwmleft,x);
  analogWrite(pwmright,x);
  digitalWrite(motorleft_forward,HIGH); 
  digitalWrite(motorleft_backward,LOW);
  digitalWrite(motorright_forward,LOW);
  digitalWrite(motorright_backward,LOW);
    }
    void wait()
      {
    analogWrite(pwmleft,255);
  analogWrite(pwmright,255);
  digitalWrite(motorleft_forward,LOW); 
  digitalWrite(motorleft_backward,LOW);
  digitalWrite(motorright_forward,LOW);
  digitalWrite(motorright_backward,LOW);
    }



  
  
