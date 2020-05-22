#include <QTRSensors.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

String path="";
//String pathToSolve="LBLLBFBLLLRBLLB";   //LBLLBFBLLLRBLLB
String optimizer[6]={"LBR","LBF","RBL","FBL","FBF","LBL"};
String optimizeResult[6]={"B","R","B","R","B","F"};

String mainQueue="";
int learn=0; //set this true to enter into learned mode, false to enter into learning mode
byte bedNumber[40]={};
int bedNumberArrayIndex=0; //this variable tracks the index number of the bednumber array

String pathAllNodes[30]={};

struct mappedPath{
  byte from;
  byte to;
  String shortPath;
};

struct mappedPath pathMap[50];

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);


#define Kp 0.013 //  0.013 works okay(best)   0.01 works good  try D and I gain 0 with p=0.01
#define Kd 0.009 //  0.009
#define Ki 0.004 // 0.004

#define MaxSpeed 40 // max speed of the robot (40)
#define BaseSpeed 35 // this is the speed at which the motors should spin when the robot is perfectly on the line  (35)
#define NUM_SENSORS 10 // number of sensors used

#define speedturn 180

void forward(void);
void backward(void);
void hardleft(void);
void hardright(void);

void backleft(void);
void backright(void);
void wait(void);
//void move(int motor, int speed, int direction);

String str;
char data = 0;
const byte interruptPin = 2;  //interrupt pin

//button variables
const int decreButton = 7;  
const int increButton = 6;
const int backButton = 5;
const int okButton = 4;  //okay button
int decreButtonState = 0; 
int increButtonState = 0; 
int backButtonState = 0; 
int okButtonState = 0; 

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

  pinMode(decreButton, INPUT);
  pinMode(increButton, INPUT);
  pinMode(backButton, INPUT);
  pinMode(okButton, INPUT);

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

  for(int i=0;i<40;i++)
        bedNumber[i]=255;  //by default we are inputing 255 to all array elements to denote that they are empty

  //temporary
    /*bedNumber[0]=1;//setting the hospital bed number
    bedNumber[1]=2;
    bedNumber[2]=3;
    bedNumber[3]=5;
    bedNumber[4]=4;*/
}

volatile long int counter = 0;
int lastError = 0;



void loop() { 
  uint16_t position = qtr.readLineBlack(sensorValues); 
  int flag=0;
  followLine();
  leftHand();

  if(learn){
    String opPath=pathProvider(2, 5);//load the path to solve  set from and to 0 is home always

    lcd.clear();
    lcd.setCursor(0,0); //we start writing from the first row first column
    lcd.print(opPath); //16 characters per line

    insertInQueue(opPath);//load the path to queue
    delay(4000);
    flag=1;
  }
     
  while(flag){   //execute this portion when entered into learned mode
    position = qtr.readLineBlack(sensorValues); 
    followLine();
    solvedRun();
  }

}



//returns the pressed bednumber
int buttonPress(){
    int bedNumb=0;
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("ENTER BED NUMBER");
    lcd.setCursor(0,1);
    lcd.print(bedNumb);
     
    okButtonState = digitalRead(okButton);
    while(okButtonState==1){
      decreButtonState = digitalRead(decreButton); 
      increButtonState = digitalRead(increButton);
      backButtonState = digitalRead(backButton);
      okButtonState = digitalRead(okButton);
      
      if(decreButtonState==0){
        bedNumb--;
        if(bedNumb<0)
          bedNumb=0;
        lcd.setCursor(0,1);
        lcd.print(bedNumb);
        delay(500);
      }
      else if(increButtonState==0){
        bedNumb++;
        lcd.setCursor(0,1);
        lcd.print(bedNumb);
        delay(500);
      }
    }
    lcd.clear();
    lcd.setCursor(0,0);
    String tempMsg=String(bedNumb)+"  PRESSED"; 
    lcd.print(tempMsg);
    delay(1000);
    return bedNumb;
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
        delay(500);
        learn=1;
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
            //call all the path optimization methods here
            shortestPath(path);//call to calculate shortest path from home to all nodes
            pathSplitter(path);
            for (int i=0; i<20; i++) {
                 if(pathAllNodes[i].length() != 0){
                    pathAllNodes[i]=shortestPathToAllPath(pathAllNodes[i]);
                 } 
            }
            
            fromToAssign();  //this function seems to be malfunctioning
            
            steps_hardleft(77);//rotate 180 degrees
            wait();
            delay(5000);
           
             
            
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
    wait(); //stop the bot                        
                                                   //get the bednumber from the user
    bedNumber[bedNumberArrayIndex++]=buttonPress();  //store the bed number in the main bednumber array

    path=path+"B";
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("ROTATE NO LINE");  
    wait();
    delay(1000);
    
    counter = 0; // reset the value of count
    while (counter < 18) { //advance forward for 20 steps
      forward();
    }
    wait();
    delay(100);
    while (sensorValues[4] < 100) {
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
  //handling case 5 (|-  like symbol) have to forward (have to go forward if cant go forward go right)
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
  counter = 0;
  while (counter <= steps) { //77steps for 180 degree
    hardleft();
  }
  wait();
  delay(500);
}

void steps_hardright(int steps) {
  counter = 0;
  while (counter < steps) { //77steps for 180 degree
    hardright();
  }
  wait();
  delay(500);
}

void steps_forward(int steps) {
  counter = 0;
  while (counter < steps) {
    followLine();
  }
  wait();
  delay(500);
}

void encoder() {
  counter++;
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
void wait() {
  analogWrite(pwmleft, 255);
  analogWrite(pwmright, 255);
  digitalWrite(motorleft_forward, LOW);
  digitalWrite(motorleft_backward, LOW);
  digitalWrite(motorright_forward, LOW);
  digitalWrite(motorright_backward, LOW);
}




void fromToAssign(){
  //int i,j;
  int count=0;
  byte endPoint=0;
  for(int i1=0;i1<40;i1++){  //count the number of beds
    if(bedNumber[i1] != 255){
      count++;
    }
  }
  
  for(int i1=0;i1<count;i1++){ //assign for home to any path
    pathMap[i1].from=0;  //set fixed number '0' because we are denoting 0 as home
    pathMap[i1].to=bedNumber[i1]; //set destination according to the bedArray
    endPoint++;
  }
  int iLocal,jLocal;
  for(iLocal=0;iLocal<count-1;iLocal++){      //now storing the path lables from one node to others
    for(jLocal=0;jLocal<count-iLocal-1;jLocal++){  //home is not here
       pathMap[jLocal+endPoint].from=bedNumber[iLocal];  //also reverse pathing is not here also   
       pathMap[jLocal+endPoint].to=bedNumber[iLocal+jLocal+1];
    }
    endPoint=endPoint+jLocal;
  }

    for (int i1=0; i1<20; i1++) {  //now store the paths in the struct map
         if(pathAllNodes[i1].length() != 0){
            pathMap[i1].shortPath=pathAllNodes[i1];
         } 
    }
    count=0;
    
    //first count what is the filled size of the struct map
    for (int i1=0; i1<50; i1++) {  //now counting the mapped paths how many paths have been mapped
         if((pathMap[i1].from != 0)||(pathMap[i1].to != 0)){
            count++;
         } 
    }
    Serial.print("total entry: ");
    Serial.println(count);

    for (int i1=count,x1=0; i1<count*2; i1++,x1++) {  //and now reverse pathing
         byte tempFrom;                          //only the form and to is stored in this loop
         byte tempTo;                            //we will store the actual path in the next loop
         
         tempFrom=pathMap[x1].from;
         tempTo=pathMap[x1].to;

         pathMap[i1].from=tempTo;
         pathMap[i1].to=tempFrom;
    }

    for (int i1=count,x1=0; i1<count*2; i1++,x1++) {  //and now reverse pathing
         String tempPath;                          //in this loop we will actually set the shortest path                       
         String reversedTempPath;
         
         tempPath=pathMap[x1].shortPath;
         reversedTempPath=reverse(tempPath);//call this function to reverse the string

         pathMap[i1].shortPath=reversedTempPath;
    }    
}


//this function just provides the path by taking input a from and to destination
String pathProvider(byte from, byte to){
  int i;
  for(i=0;i<50;i++){
     if(pathMap[i].from==from && pathMap[i].to==to)
        return pathMap[i].shortPath;                   
  }
  return "error";
}

void pathSplitter(String pathToSolve){
  int count=0;
    int j=0;
    int endPosition[30];
    
    for(int i=0;i<pathToSolve.length();i++){
      if(pathToSolve.charAt(i)=='B'){
        count++;
        endPosition[j++]=i;
      }
    }

    int x=0;
    for(int i=0;i<40;i++){  //searching free index from where value will be stored
      if(pathAllNodes[i].length()==0){
        x=i;
        break;
      }
    }
    
    for (int i=0; i<count; i++) {
       for (int k=i+1; k<count; k++) {
         pathAllNodes[x]=pathToSolve.substring(endPosition[i]+1,endPosition[k]);
         Serial.println(pathAllNodes[x]);  
         Serial.print("   ");
         Serial.print(x);
         Serial.println(" ");
         x++;
      } 
    }
}



void shortestPath(String pathToSolve){
  int j=0;
    for (int i=0; i<pathToSolve.length(); i++) {
        if(pathToSolve.charAt(i) == 'B'){
            pathAllNodes[j++]=pathToSolve.substring(0,i);     
        }
    }

    int len=0;
    for (int i=0; i<pathToSolve.length(); i++) {
        if(pathAllNodes[i].length()!=0)
            len++;
    }
    for (int i=0; i<len; i++) {
        //Serial.println(optimizedPaths[i]);
    }
    //main work starts from here
    for (int i=0; i<len; i++) {
           
            for(int x=0; x<pathAllNodes[i].length(); x++){
                
                for(int k=0; k<6; k++){
                    pathAllNodes[i].replace(optimizer[k], optimizeResult[k]);
                }
        } 
    }
    
    for (int i=0; i<len; i++) {
        Serial.println(pathAllNodes[i]);
    }
}


String shortestPathToAllPath(String pathToSolve){
  int j=0;
   //main work starts from here 
    for(int x=0; x<pathToSolve.length(); x++){        
        for(int k=0; k<6; k++){
            pathToSolve.replace(optimizer[k], optimizeResult[k]);
        }
    }
    return pathToSolve; 
}



//FUNCTION TO REVERSE A STRING
String reverse(String strToRev){
  int i,j;
  char arraChar[100];
  char temp[100];
  String strResult="";
  int len=strToRev.length();
  strToRev.toCharArray(arraChar, len+1);

  for(i=len-1,j=0; i>=0; i--,j++){
    temp[j]=arraChar[i];
  }

  for(i=0;i<len;i++){  //here we will flip R,L if R then L and if L then R
    if(temp[i] == 'L')
      temp[i]='R';
    else if(temp[i] == 'R'){
      temp[i]='L';
    }
  }
  
  for(i=0;i<len; i++){
    strResult=strResult+temp[i];
  }
  return strResult;
}


//call this function to insert a string into queue
void insertInQueue(String stringToQueue){
  mainQueue=stringToQueue;
}

char deQueue(){
  char deqeuedElement=mainQueue.charAt(0);   //dequesd element to return
  if(mainQueue.length()==0)
    return 'E';
  mainQueue.remove(0, 1);  //remove the element from the queue
  return deqeuedElement;
}



void solvedRun(){
  uint16_t position;
  position = qtr.readLineBlack(sensorValues);
  //foreign code
  //handling case 1 ('+' intersection  and  'T'  like intersection  and  dead end)
  //below if handles the situation if bot sees black on all the sensors
  if ((sensorValues[0] > 100 && sensorValues[9] > 100) || (sensorValues[1] > 100 && sensorValues[8] > 100)) 
    {
    steps_forward(22); //after seeing plus intersection go forward for 24 steps
    wait(); //wait a bit
    position = qtr.readLineBlack(sensorValues); //read the line again
    delay(100); //wait for 100 ms
    
  //now dequeue to see where we have to go
  char pathWay=deQueue();
  whereToGo(pathWay);  //call this function, it will rotate the bot to the required location
  
  }//end of black line condition

  //handling the case for no line. when the robot see's no line(case 0)
  else if (sensorValues[0] < 100 && sensorValues[1] < 100 && sensorValues[2] < 100 && sensorValues[3] < 100 && sensorValues[4] < 100 && sensorValues[5] < 100 && sensorValues[6] < 100 && sensorValues[7] < 100 && sensorValues[8] < 100 && sensorValues[9] < 100)
  { 
    steps_forward(20);
    wait();
    delay(100);
  //now dequeue to see where we have to go
  char pathWay=deQueue();
  whereToGo(pathWay);  //call this function, it will rotate the bot to the required location
    
  }
  //handling case 4 (-|  like symbol  and  case 2 (⌝ like section)) no matter what we have to go left
   else if(sensorValues[9] > 900 && sensorValues[0] < 200 && (sensorValues[5] > 900 || sensorValues[6] > 900))   //400
        {
            steps_forward(20);
            wait();
            delay(500);
      //now dequeue to see where we have to go
      char pathWay=deQueue();
      whereToGo(pathWay);  //call this function, it will rotate the bot to the required location

  }
  //handling case 5 (|-  like symbol) have to forward (have to go right if cant go forward)
  else if(sensorValues[0] > 900 && sensorValues[9] < 200 && (sensorValues[4] > 900 || sensorValues[3] > 900))
        {
            steps_forward(20);
            wait();
            delay(500);
            //now dequeue to see where we have to go
      char pathWay=deQueue();
      whereToGo(pathWay);  //call this function, it will rotate the bot to the required location
            
        }

}            

void whereToGo(char pathWay){
    uint16_t position = qtr.readLineBlack(sensorValues);
  if(pathWay == 'F'){
    //do nothing here the bot will go forward anyway
  }
  else if(pathWay == 'R'){
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
  else if(pathWay == 'L'){
    position = qtr.readLineBlack(sensorValues);
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
  else if(pathWay == 'E'){
    //here it means that the bot has reached its desired destination
    //for now just keep it empty with an infinte loop
    while(1){
      wait();
      lcd.clear();
      lcd.setCursor(0,0); 
      lcd.print("END OF THE RUN");
      delay(100);
    }
  }
}
