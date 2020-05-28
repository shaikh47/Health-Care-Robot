void setup() {
Serial.begin(9600);
}

void loop() {
  postBedData("1$2$3$4$5$6$7$");
  delay(300000);
}


int requestData(){
  Serial.println("SEND DATA");
  
  if(Serial.available()>0) {
    String a = Serial.readStringUntil('$');
    a=a+" length is: "+(a.length()-1);
    Serial.println(a);
    delay(100);
    return 1;
  }

  return 0;
}


void postBedData(String dataToPost){
  Serial.println("BED DATA");
  while(1){
    if(Serial.available()>0) {
      String a = Serial.readStringUntil('$');
      if(a=="SEND DATA"){
        delay(100);
        Serial.println(dataToPost);  //send a dummy bed data
        break;
      }
    }
  }
}
