#include <QTRSensors.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);


#define Kp 0.01 //  0.01 works okay   0.05 works good  
#define Kd 0.39 //  0.39
#define Ki 0.06 // 0.06

#define MaxSpeed 40 // max speed of the robot (40)
#define BaseSpeed 35 // this is the speed at which the motors should spin when the robot is perfectly on the line  (35)
#define NUM_SENSORS 10 // number of sensors used

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
char data = 0;
const byte interruptPin = 2;

String path="";

int x = 30;
int y = 255;
int flag = 0;

int motorleft_forward = 9;
int motorleft_backward = 8;
int motorright_forward = 12;
int motorright_backward = 13;

int pwmleft = 10;
int pwmright = 11;

int led = 25;

int Read = 0;
float distance = 0.0;
float elapsedTime, time, timePrev; //Variables for time control
float previous_error, error;
int period = 50; //Refresh rate period of the loop is 50ms
unsigned int sensors[10];

QTRSensors qtr;

const uint8_t SensorCount = 10;
uint16_t sensorValues[SensorCount];

void setup() {
  pinMode(motorleft_forward, OUTPUT);
  pinMode(motorleft_backward, OUTPUT);
  pinMode(motorright_forward, OUTPUT);
  pinMode(motorright_backward, OUTPUT);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(0, encoder, FALLING);
  //hardleft();
  lcd.begin(16,2);
  lcd.backlight();

  // configure the sensors
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]) {35,37,39,41,43,45,47,49,51,53}, SensorCount); //53,51,49,47,45,43,41,39,37,35
  //qtr.setEmitterPin(2);                                                             //35,37,39,41,43,45,47,49,51,53

  lcd.setCursor(0,0); //we start writing from the first row first column
  lcd.print("CALIBRATING!"); //16 characters poer line

  delay(500);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // turn on Arduino's LED to indicate we are in calibration mode
  analogWrite(11, 0);
  analogWrite(10, 0);

  // 2.5 ms RC read timeout (default) * 10 reads per calibrate() call
  // = ~25 ms per calibrate() call.
  // Call calibrate() 100 times to make calibration take about 10 seconds.
  for (uint16_t i = 0; i < 150; i++) {
    qtr.calibrate();
  }
  digitalWrite(LED_BUILTIN, LOW); // turn off Arduino's LED to indicate we are through with calibration

  // print the calibration minimum values measured when emitters were on
  Serial.begin(9600);
  for (uint8_t i = 0; i < SensorCount; i++) {
    Serial.print(qtr.calibrationOn.minimum[i]);
    Serial.print(' ');
  }
  Serial.println();

  // print the calibration maximum values measured when emitters were on
  for (uint8_t i = 0; i < SensorCount; i++) {
    Serial.print(qtr.calibrationOn.maximum[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();

  lcd.clear();
  lcd.print("DONE!");
  wait();
  delay(1000);

  time = millis();
  lcd.clear();
}

volatile long int count = 0;
int lastError = 0;



void loop() {
  uint16_t position = qtr.readLineBlack(sensorValues);
  int parity=0;
  for (uint8_t i = 0; i < SensorCount; i++) {
    if(sensorValues[i]>200)
        parity++;
  }
  
  followLine();
  leftHand();
}








void leftHand(){
  uint16_t position;
  position = qtr.readLineBlack(sensorValues);
  //foreign code
  //handling case 1 ('+' intersection  and  'T'  like intersection  and  dead end)
  //below if handles the situation if bot sees black on all the sensors
  if ((sensorValues[0] > 100 && sensorValues[9] > 100) || (sensorValues[1] > 100 && sensorValues[8] > 100)) 
    {
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("ALL BLACK!");  
    wait();
    delay(1000);
    steps_forward(22); //after seeing plus intersection go forward for 24 steps
    wait(); //wait a bit
    position = qtr.readLineBlack(sensorValues); //read the line again
    delay(100); //wait for 100 ms
        //below if handles the all black sensor situation which signifies the start position
    //paste
    if((sensorValues[0] > 100 && sensorValues[9] > 100) || (sensorValues[1] > 100 && sensorValues[8] > 100))
    {
        wait();  //just wait for now
        delay(1000);
        String pathTwo="";
          //display the path array
            lcd.clear();
            lcd.setCursor(0,0); 
            lcd.print(path);
            if(path.length()>16){
              pathTwo=path.substring(16);
              lcd.setCursor(1,1);
              lcd.print(pathTwo);
            }  
            while(1){
              
            }
    }
    //paste
    //handling the case where we see a T intersection
    //below if handles the situation where we see no line after a T intersection
    else if (sensorValues[0] < 200 && sensorValues[1] < 200 && sensorValues[2] < 200 && sensorValues[3] < 200 && sensorValues[4] < 200 && sensorValues[5] < 200 && sensorValues[6] < 200 && sensorValues[7] < 200 && sensorValues[8] < 200 && sensorValues[9] < 200) 
    { //0000000000
      path=path+"L";
      lcd.clear();
      lcd.setCursor(0,0); 
      lcd.print("T INTER");  
      wait();
      delay(1000);
    
      position = qtr.readLineBlack(sensorValues);
        while (sensorValues[3] < 150 || sensorValues[4] < 150) {   //move left while the bot finds the line
        position = qtr.readLineBlack(sensorValues);
        hardleft();
        }
      wait();
      delay(100);
    }
    //now check here to see if these is a straight line in order to verify if as a plus intersection
    else if (!(sensorValues[0]<200 && sensorValues[1]<200 && sensorValues[2] < 200 && sensorValues[3] < 200 && sensorValues[4] < 200 && sensorValues[5] < 200 && sensorValues[6] < 200 && sensorValues[7] < 200 && sensorValues[8]<200 && sensorValues[9]<200)){ //111000
      path=path+"L";
      lcd.clear();
      lcd.setCursor(0,0); 
      lcd.print("PLUS INTER");
      wait();
      delay(1000);
      
      while ((sensorValues[9] < 500 || sensorValues[8] < 500)) { //if the if is true which means it is '+' intersection
        position = qtr.readLineBlack(sensorValues);
        hardleft();
      }
      wait();
      delay(100);
      position = qtr.readLineBlack(sensorValues);
      while ((sensorValues[4] < 100 || sensorValues[5] < 100)) { //center the bot with the line
        position = qtr.readLineBlack(sensorValues);
        hardleft();
      }
      wait();
      delay(500);
    }
  }//end of black line condition

  //handling the case for no line. when the robot see's no line(case 0)
  else if (sensorValues[0] < 100 && sensorValues[1] < 100 && sensorValues[2] < 100 && sensorValues[3] < 100 && sensorValues[4] < 100 && sensorValues[5] < 100 && sensorValues[6] < 100 && sensorValues[7] < 100 && sensorValues[8] < 100 && sensorValues[9] < 100)
  {
    path=path+"B";
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("ROTATE NO LINE");  
    wait();
    delay(1000);
    
    count = 0; // reset the value of count
    while (count < 18) { //advance forward for 20 steps
      forward();
    }
    wait();
    delay(100);
    while (sensorValues[3] < 100 || sensorValues[4] < 100) {
      position = qtr.readLineBlack(sensorValues);
      hardleft();
    }
    wait();
    delay(500);
  }
  //handling case 4 (-|  like symbol  and  case 2 (⌝ like section)) no matter what we have to go left
   else if(sensorValues[9] > 900 && sensorValues[0] < 200 && (sensorValues[5] > 900 || sensorValues[6] > 900))   //400
        {
            path=path+"L";
            lcd.clear();
            lcd.setCursor(0,0); 
            lcd.print("-|  or left turn");  
            wait();
            delay(1000);
          
            steps_forward(19);
            wait();
            delay(500);
            
            position = qtr.readLineBlack(sensorValues);
            while (sensorValues[9] < 500 || sensorValues[8] < 500) {
              position = qtr.readLineBlack(sensorValues);
              hardleft();
            }
            wait();
            delay(100);
            position = qtr.readLineBlack(sensorValues);
            while (sensorValues[4] < 300 || sensorValues[5] < 300) {
              position = qtr.readLineBlack(sensorValues);
              hardleft();
            }
            wait();
            delay(500);
  }
  //handling case 5 (|-  like symbol) have to forward (have to go right if cant go forward)
  else if(sensorValues[0] > 900 && sensorValues[9] < 200 && (sensorValues[4] > 900 || sensorValues[3] > 900))
        {
            lcd.clear();
            lcd.setCursor(0,0); 
            lcd.print("|- or right turn");  
            wait();
            delay(1000);
          
            steps_forward(19);
            wait();
            delay(500);
            
            position = qtr.readLineBlack(sensorValues);
            //if there is no line we must go right
            if(sensorValues[0] < 100 && sensorValues[1] < 100 && sensorValues[2] < 100 && sensorValues[3] < 100 && sensorValues[4] < 100 && sensorValues[5] < 100 && sensorValues[6] < 100 && sensorValues[7] < 100 && sensorValues[8] < 100 && sensorValues[9] < 100){
                path=path+"R";
                while (sensorValues[0] < 500 || sensorValues[1] < 500) {
                    position = qtr.readLineBlack(sensorValues);
                    hardright();
                }
                wait();
                delay(100);
                position = qtr.readLineBlack(sensorValues);
                while (sensorValues[4] < 300 || sensorValues[5] < 300) {
                    position = qtr.readLineBlack(sensorValues);
                    hardright();
                }
            wait();
            delay(500);
            }
            //else, if there is line we must go forward
            else{  
                //do nothing here the bot will go forward anyway
                path=path+"F";
            }
  }
}


void followLine() {
  int prev_position = 0;
  //detachInterrupt(digitalPinToInterrupt(interruptPin));
  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  uint16_t position;
  position = qtr.readLineBlack(sensorValues);

  // print the sensor values as numbers from 0 to 1000, where 0 means maximum
  // reflectance and 1000 means minimum reflectance, followed by the line
  // position
  String senseData="";
  for (uint8_t i = 0; i < SensorCount; i++) {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
    int temp;
    if(sensorValues[i]>=1000)
        temp=9;
    else
        temp=(int)sensorValues[i]/100;
    senseData=senseData+temp;
  }
  senseData=senseData+"         ";
  lcd.setCursor(0,0); 
  lcd.print(senseData);
    
  Serial.println(position);
  int error;

  error = position - 4333; //4500
  int I = (I + error) * Ki;
  int motorSpeed = (Kp * error) + (Kd * (error - lastError)) + I;
  lastError = error;

  int rightMotorSpeed = BaseSpeed + motorSpeed;
  int leftMotorSpeed = BaseSpeed - motorSpeed;

  if (rightMotorSpeed > MaxSpeed) rightMotorSpeed = MaxSpeed; // prevent the motor from going beyond max speed
  if (leftMotorSpeed > MaxSpeed) leftMotorSpeed = MaxSpeed; // prevent the motor from going beyond max speed
  if (rightMotorSpeed < 0) rightMotorSpeed = 0;
  if (leftMotorSpeed < 0) leftMotorSpeed = 0;

  //call this when straight on the line
  if (sensorValues[0] < 100 && sensorValues[1] < 100 && sensorValues[2] < 100 && sensorValues[3] < 100 && sensorValues[4] < 100 && sensorValues[5] < 100 && sensorValues[6] < 100 && sensorValues[7] < 100 && sensorValues[8] < 100 && sensorValues[9] < 100){
    forward();
  } else {
      move(1, rightMotorSpeed, 1); //motor derecho hacia adelante
      move(0, leftMotorSpeed, 1); //motor izquierdo hacia adelante
  }

  prev_position = position;
}

void steps_hardleft(int steps) {
  count = 0;
  while (count <= steps) { //77steps for 180 degree
    hardleft();
  }
  wait();
  delay(500);
}

void steps_hardright(int steps) {
  count = 0;
  while (count < steps) { //77steps for 180 degree
    hardright();
  }
  wait();
  delay(500);
}

void steps_forward(int steps) {
  count = 0;
  while (count < steps) {
    followLine();
  }
  wait();
  delay(500);
}

void encoder() {
  count++;
  //Serial.println(_count);
}

void move(int motor, int speed, int direction) {

  boolean inPin1 = HIGH;
  boolean inPin2 = LOW;

  if (direction == 1) {
    inPin1 = HIGH;
    inPin2 = LOW;
  }
  if (direction == 0) {
    inPin1 = LOW;
    inPin2 = HIGH;
  }

  if (motor == 0) {
    digitalWrite(motorleft_forward, inPin1);
    digitalWrite(motorleft_backward, inPin2);
    analogWrite(pwmleft, speed);
  }
  if (motor == 1) {
    digitalWrite(motorright_forward, inPin1);
    digitalWrite(motorright_backward, inPin2);
    analogWrite(pwmright, speed);
  }
}

void forward() {
  analogWrite(pwmleft, x);
  analogWrite(pwmright, x);
  digitalWrite(motorleft_forward, HIGH);
  digitalWrite(motorleft_backward, LOW);
  digitalWrite(motorright_forward, HIGH);
  digitalWrite(motorright_backward, LOW);
}
void backward() {
  analogWrite(pwmleft, x);
  analogWrite(pwmright, x);
  digitalWrite(motorleft_forward, LOW);
  digitalWrite(motorleft_backward, HIGH);
  digitalWrite(motorright_forward, LOW);
  digitalWrite(motorright_backward, HIGH);
}
void hardleft() {
  analogWrite(pwmleft, x);
  analogWrite(pwmright, x);
  digitalWrite(motorleft_forward, LOW);
  digitalWrite(motorleft_backward, HIGH);
  digitalWrite(motorright_forward, HIGH);
  digitalWrite(motorright_backward, LOW);
}
void hardright() {
  analogWrite(pwmleft, x);
  analogWrite(pwmright, x);
  digitalWrite(motorleft_forward, HIGH);
  digitalWrite(motorleft_backward, LOW);
  digitalWrite(motorright_forward, LOW);
  digitalWrite(motorright_backward, HIGH);
}
void left() {
  analogWrite(pwmleft, x);
  analogWrite(pwmright, x);
  digitalWrite(motorleft_forward, LOW);
  digitalWrite(motorleft_backward, LOW);
  digitalWrite(motorright_forward, HIGH);
  digitalWrite(motorright_backward, LOW);
}
void right() {
  analogWrite(pwmleft, x);
  analogWrite(pwmright, x);
  digitalWrite(motorleft_forward, HIGH);
  digitalWrite(motorleft_backward, LOW);
  digitalWrite(motorright_forward, LOW);
  digitalWrite(motorright_backward, LOW);
}
void wait() {
  analogWrite(pwmleft, 255);
  analogWrite(pwmright, 255);
  digitalWrite(motorleft_forward, LOW);
  digitalWrite(motorleft_backward, LOW);
  digitalWrite(motorright_forward, LOW);
  digitalWrite(motorright_backward, LOW);
}