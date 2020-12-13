#include <Servo.h>
Servo jaw;
Servo jawTwist;
Servo secondaryArm;
Servo mainRotation;
Servo primaryArm;

int posA;
int posB;
int posC;
int posD;
int posE;

byte jawVal[30];
byte jawTwistVal[30];
byte secondaryArmVal[30];
byte mainRotationVal[30];
byte primaryArmVal[30];

void setup() {
  Serial.begin(9600);

  jaw.attach(11);
  jawTwist.attach(6);
  secondaryArm.attach(10);
  mainRotation.attach(5);
  primaryArm.attach(9);

  jaw.write(124);
  jawTwist.write(80);
  secondaryArm.write(160);
  mainRotation.write(60);
  primaryArm.write(0);

  posA=90;
  posB=90;
  posC=70;
  posD=40;
  posE=90;
  
}

void loop() {
  String x[5];
  int from=0;
  int i1=0;
    
    while(Serial.available()>0) {
    String a = Serial.readStringUntil('&');
    int len=a.length();
    for(int i=0;i<len;i++){
      if(a.charAt(i)=='$'){
        x[i1]=a.substring(from,i);
        from=++i;
        i1++;
      }
    }

    int varTwist=x[0].toInt();
    int varPitchPalm=x[1].toInt();
    int varYaw=x[2].toInt();
    int varPitchArm=x[3].toInt();
    int varJaw=x[4].toInt();

    varTwist=map(varTwist,-80,60,20,160);//roll
    varPitchPalm=map(varPitchPalm,-40,40,160,25);//pitch palm
    varYaw=map(varYaw,-15,15,30,110);//yaw
    varPitchArm=map(varPitchArm,-40,40,110,0);//pitch arm
    varJaw=map(varJaw,125,470,124,60);//jaw
    
    jaw.write(varJaw);
    jawTwist.write(varTwist);
    secondaryArm.write(varPitchPalm);
    mainRotation.write(varYaw);
    primaryArm.write(varPitchArm);
  }
}
