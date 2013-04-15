/*SandStorm 1 Software v0.1 by Guillermo Vargas et alia, El Paso, TX

Creative Commons License - Share Alike

If you alter, transform, or build upon this work, you may distribute 
the resulting work only under the same or similar license to this one.

*/

#include <SoftwareSerial.h>
#include <string.h>
#include <ctype.h>


// Declaration of variables.

SoftwareSerial goopsSerial (2, 3);
const int cameraPin = 13;  // LED test pin
const int cutoffPin = 12;  //Cutoff Pin
const int rxPin = 0;  // RX PIN 
const int txPin = 1; //TX PIN
int byteGPS = -1;
char line[300] = "";
char commandGPR[7] = "$GPRMC";
int count = 0;
int bien = 0;
int conta = 0;
int indices[13];
int cameraInstruction = 0; //Camera is initially off.
int cutoffInstruction = 0; //Cutoff Mechanism is intially off.
int heartBeatInstruction = 0; //Heart does not beat initially.
int cameraState = LOW;
int instructionCode = 0;

//Interval declarations for millis() function

long timeSinceShutterTrigger = 0;
long intervalOfShutterTrigger = 5000; //The interval will be in the order of x10^3
long timeSinceLastHeartBeat = 0;
long intervalOfHeartbeats = 5000;
//------------------------------

void setup() {
  pinMode(cameraPin, OUTPUT);       // Initialize camera pin
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  Serial.begin(57600);
  goopsSerial.begin(57600);
  
  for (int i=0;i<300;i++){       // Initialize a buffer for received data
    line[i]=' ';
  }  
}

// HeartBeat Function

void HeartBeats() {
  unsigned long currentHeartBeatMillis = millis();
  if(currentHeartBeatMillis - timeSinceLastHeartBeat > intervalOfHeartbeats) {
    timeSinceLastHeartBeat = currentHeartBeatMillis;
      Serial.println("Heartbeat");
      Serial.println();
  }
}
//Function for automatic picture acquisition.

void startCamera() {
 unsigned long currentCameraMillis = millis();
 if(currentCameraMillis - timeSinceShutterTrigger > intervalOfShutterTrigger ) {
  timeSinceShutterTrigger = currentCameraMillis;
  if(cameraState == LOW)
    cameraState = HIGH;
  else
    cameraState = LOW;
    digitalWrite(cameraPin, cameraState);
  }
}

//-------------------------------------------
 
 //Function for cutdown of payload from balloon line.

void cutoffPayload() {
  Serial.print("Instruction received, cutoff underway for 10 seconds");
  Serial.println("Check for altitude drop to confirm");
  Serial.println();
  digitalWrite(cutoffPin, HIGH);
  delay(5000);
  digitalWrite(cutoffPin, LOW);
  cutoffInstruction = 0;
  
}

//---------------------------------------------

void loop() {
 
/* Setup of the camera commands, the cutoff mechanism, the heartbeat indicator. 
The flight will last around 4 hours therefore use the formula on Github to 
calculate the interval of camera shutter.*/

if(Serial.available()>0) {
  instructionCode = Serial.read();
  switch (instructionCode) {
    case 30: //Case of the Auto-Camera instruction to "ON"
      Serial.println("Activate Auto Camera Shutter");
      cameraInstruction = 1;
      break;
      
    case 40: //Case of the Auto-Camera instruction "OFF"
      Serial.println("Shutdown Auto Camera Shutter");
      cameraInstruction = 0;
      break;
    
    case 50: //Cut the line of the balloon.
      cutoffInstruction = 1;
      break;
      
    case 60: //Start Heartbeats.
      heartBeatInstruction = 1;
      break;
      
    case 70: //Stop Heartbeats.
      heartBeatInstruction = 0;
  }
}
  
/*Loop of the camera action, and cutoff while the value of cameraInstruction is 1;
continue executing the function triggerPictures()*/
  
  while(cameraInstruction == 1) {
    startCamera();
  }
  
  if(cutoffInstruction == 1) {
    cutoffPayload();
  }
  
  if(heartBeatInstruction == 1) {
    HeartBeats();
  }

/*------------------------------------*/

// Setup of the GPS code.
   //Serial.print("Overflow is ");
   //Serial.println(goopsSerial.overflow());
   byteGPS = goopsSerial.read();         // Read a byte of the serial port
   if (byteGPS == -1) {           // See if the port is empty yet
     delay(100); 
   } else {
     line[conta]=byteGPS;        // If there is serial port data, it is put in the buffer
     conta++;                      
     Serial.write(byteGPS); 
     if (byteGPS==13){            // If the received byte is = to 13, end of transmission
       count=0;
       bien=0;
       for (int i=1;i<7;i++){     // Verifies if the received command starts with $GPR
         if (line[i]==commandGPR[i-1]){
           bien++;
         }
       }
       if(bien==6){               // If yes, continue and process the data
         for (int i=0;i<300;i++){
           if (line[i]==','){    // check for the position of the  "," separator
             indices[count]=i;
             count++;
           }
           if (line[i]=='*'){    // ... and the "*"
             indices[12]=i;
             count++;
           }
         }
         for (int i=0;i<12;i++){
           switch(i){
           }
           for (int j=indices[i];j<(indices[i+1]-1);j++){
             Serial.print(line[j+1]); 
           }
         }
       }
       conta=0;                    // Reset the buffer
       for (int i=0;i<300;i++){    //  
         line[i]=' ';             
       }                 
     }
   }
 }

