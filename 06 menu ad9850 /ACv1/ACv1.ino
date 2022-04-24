/*This is the arduino code to be used with the python code (ACS) to operate an acoustic deformation chamber */
/*For notes and use please vist - GITHUB "all rights reserved" */

/*By Zak Watts and Mimi Houlihan */


#include <AD9833.h>
#define FNC_PIN 10   // Can be any digital IO pin
AD9833 gen(FNC_PIN);    // Defaults to 25MHz internal reference frequency

//AD8302 output connections
//not sure if COM_pin is needded XXXXXXXXX 

int VREF_Pin = A0;
int VP_Pin = A1;  
int VM_Pin = A2;

//com_pin not needed 
int COM_Pin = A5;


float COMValue = 0;
float VREFValue = 0;
float VMValue = 0;
float VPValue = 0;

//Set the header names for the outputted data
String dataLabel1 = "FREQ";
String dataLabel2 = "VREF";
String dataLabel3 = "VM";
String dataLabel4 = "VP";
String dataLabel5 = "COM";

bool label = true;
float maxfreq;
String input;
String userInput;
int ans;

String incomingByte;

#define pulseHigh(pin) {digitalWrite(pin, HIGH); delay(1); digitalWrite(pin, LOW); }

//Used to try to track the data and store it into python
float FreqValue = 0;

//Create the starting freq value
float StartFreq = (60.e5);
float CurrentFreq = 0;
float IntervalFreq = (1.e4);
float FinalFreq = (70.e5);





void setup() {

  //configure the output of the data
  Serial.begin(9600);


  gen.ApplySignal(SINE_WAVE,REG0,StartFreq);
  gen.EnableOutput(true);

  CurrentFreq = StartFreq;
  

  //Creating the labels for headers in the data, which will be in CSV format
  while(label){ //runs once
    Serial.print(dataLabel1);
    Serial.print(",");
    Serial.print(dataLabel2);
    Serial.print(",");
    Serial.print(dataLabel3);
    Serial.print(",");
    Serial.print(dataLabel4);
    Serial.print(",");
    Serial.println(dataLabel5);
    label=false;
  }
}


void loop() {

  // see if there's incoming serial data:
  if (Serial.available() > 0) {

    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();

    // if it's a capital F, scan for the frequencies:
    if (incomingByte == 'F') {

        while(CurrentFreq < FinalFreq) {
        
        delay(100);
    
        //Reading the analog pins on the Arduinio    
        VREFValue = analogRead(VREF_Pin);
        VMValue = analogRead(VM_Pin);
        VPValue = analogRead(VP_Pin);
        COMValue = analogRead(COM_Pin);
        //Serial.println(FreqValue);         // send data to serial
    
        gen.ApplySignal(SINE_WAVE,REG0,CurrentFreq);
        //Serial.println(CurrentFreq);
    
        String p1=",";
        Serial.println(CurrentFreq +  p1 + VREFValue + p1 + VMValue + p1 + VPValue + p1 + COMValue);
        CurrentFreq = CurrentFreq + IntervalFreq;
    }
    }


    // if it's an S, get ready to recieve and send the driving frequency:
    if (incomingByte == 'S') {
      
        
        input = Serial.readStringUntil('\n');
        maxfreq = input.toInt();
        
        Serial.println(maxfreq);

        gen.ApplySignal(SINE_WAVE,REG0,maxfreq);

        Serial.println("freq sent");

    }


}
