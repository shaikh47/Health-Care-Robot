
String path="LBLLBFBLLLRBLLB";     //LBLLBFBLLB  //LBLLBFBLLLRBLLBFL
String optimizer[6]={"LBR","LBF","RBL","FBL","FBF","LBL"};
String optimizeResult[6]={"B","R","B","R","B","F"};

byte bedNumber[40]={};

String pathAllNodes[30]={};

struct mappedPath{
  byte from;
  byte to;
  String shortPath;
};

struct mappedPath pathMap[50];

//String pathToHome[5]={};

void setup() {
    Serial.begin(9600);
    for(int i=0;i<40;i++)
        bedNumber[i]=255;  //by default we are inputing 255 to all array elements to denote that they are empty
}

void loop() {
    for(int i=0;i<5;i++)
        bedNumber[i]=i+1;
    fromToAssign();
    for(int i=0;i<15;i++){
      Serial.print(pathMap[i].from);
      Serial.print(" ");
      Serial.println(pathMap[i].to);
    }
      delay(50000);  
  
    shortestPath(path);//call to calculate shortest path from home to all nodes
    pathSplitter(path);
    Serial.println("");
    Serial.println("_______________");
    for (int i=0; i<20; i++) {
         if(pathAllNodes[i].length() != 0){
            pathAllNodes[i]=shortestPathToAllPath(pathAllNodes[i]);
         } 
    }



    for (int i=0; i<20; i++) {
         Serial.print(i); 
         Serial.print(" ");
         Serial.println(pathAllNodes[i]);
    }
    delay(50000);
}

void fromToAssign(){
  int i,j;
  int count=0;
  byte endPoint=0;
  for(int i=0;i<40;i++){  //count the number of beds
    if(bedNumber[i] != 255){
      count++;
    }
  }
  
  for(int i=0;i<count;i++){ //assign for home to any path
    pathMap[i].from=0;  //set fixed number '0' because we are denoting 0 as home
    pathMap[i].to=bedNumber[i]; //set destination according to the bedArray
    endPoint++;
  }
  
  for(i=0;i<count-1;i++){      //now storing the path lables from one node to others
    for(j=0;j<count-i-1;j++){  //home is not here
       pathMap[j+endPoint].from=bedNumber[i];  //also reverse pathing is not here also   
       pathMap[j+endPoint].to=bedNumber[i+j+1];
    }
    endPoint=endPoint+j;
  }
}


void pathSplitter(String pathToSolve){
  int count=0;
    int j=0;
    int endPosition[10];
    
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
