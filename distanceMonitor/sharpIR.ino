#include<Servo.h>
#define pin A0

uint16_t get_gp2d12 (uint16_t value);

Servo servo;

void setup () {
    Serial.begin (9600);
    pinMode (pin, INPUT);
    servo.attach(11);
    pinMode(13,OUTPUT);
}

void loop () {
    int pos;
    uint16_t value;
    uint16_t range;

    for (pos = 40; pos <= 100; pos += 1) {
      servo.write(pos); 
      value = analogRead (pin);
      range = get_gp2d12(value)/10.0;
      
      //Serial.print("Actual range: ");
      //Serial.print(range);
      //Serial.print("Predicted range: ");
      //Serial.print(distanceCalc(pos));
      //Serial.println();
      checkCollision(range,distanceCalc(pos));
           
      delay(10);                     
    }
  for (pos = 100; pos >= 40; pos -= 1) { 
      servo.write(pos); 
      value = analogRead (pin);
      range = get_gp2d12(value)/10.0;
      
      //Serial.print("Actual range: ");
      //Serial.print(range);
      //Serial.print("Predicted range: ");
      //Serial.print(distanceCalc(pos));
      //Serial.println();
      checkCollision(range,distanceCalc(pos));
      
      delay(10);                       
    }
}

uint16_t get_gp2d12 (uint16_t value) {
    if (value < 10) value = 10;
    return ((67870.0 / (value - 3.0)) - 40.0);
}


void checkCollision(double senseDict,double predictDist){
  int var=analogRead(pin);
  int dist=get_gp2d12(var)/10.0;

  if(senseDict<predictDist){
    while(dist<predictDist){
      var=analogRead(pin);
      dist=get_gp2d12(var)/10.0;
      digitalWrite(13,HIGH);
      
      Serial.print(dist);
      Serial.print("   ");
      Serial.print(predictDist);
      Serial.println("");
    }
  }
  digitalWrite(13,LOW);
}


double distanceCalc(double angleServo){
  int definedDist=12;//defined distance value at 0 degree angle
  
  double ang=map(angleServo,40,100,20,155);
 
  ang=abs(90-ang);
  ang=ang*(PI/180.0);
 
  double predictDist=(double)definedDist/sin(90-ang);
  return predictDist;
}
