
String path="LBLLBFBLLLRBLLB";     //LBLLBFBLLB  //LBLLBFBLLLRBLLBFL
String optimizedPaths[20]={};
String optimizer[6]={"LBR","LBF","RBL","FBL","FBF","LBL"};
String optimizeResult[6]={"B","R","B","R","B","F"};

void setup() {
    Serial.begin(9600);
}

void loop() {
    Serial.println(optimizedPaths[0].length());
    shortestPath();
    Serial.println(optimizedPaths[0].length());
    delay(5000);
}

void shortestPath(){
  int j=0;
    for (int i=0; i<20; i++) {
        if(path.charAt(i) == 'B'){
            optimizedPaths[j++]=path.substring(0,i);     
        }
    }

    int len=0;
    for (int i=0; i<20; i++) {
        if(optimizedPaths[i].length()!=0)
            len++;
    }
    for (int i=0; i<len; i++) {
        //Serial.println(optimizedPaths[i]);
    }
    //main work starts from here
    for (int i=0; i<len; i++) {
           
            for(int x=0; x<optimizedPaths[i].length(); x++){
                
                for(int k=0; k<6; k++){
                    optimizedPaths[i].replace(optimizer[k], optimizeResult[k]);
                }
        } 
        Serial.println(""); 
    }
    
    for (int i=0; i<len; i++) {
        Serial.println(optimizedPaths[i]);
    }
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
  for(i=0;i<len; i++){
    strResult=strResult+temp[i];
  }
  return strResult;
}
