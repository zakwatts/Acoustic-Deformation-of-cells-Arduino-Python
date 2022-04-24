/* 
 * A simple single freq AD9850 Arduino test script
 * Original AD9851 DDS sketch by Andrew Smallbone at www.rocketnumbernine.com
 * Modified for testing the inexpensive AD9850 ebay DDS modules
 * Pictures and pinouts at nr8o.dhlpilotcentral.com
 * 9850 datasheet at http://www.analog.com/static/imported-files/data_sheets/AD9850.pdf
 * Use freely
 */

#include "HCuOLED.h"
#include "SPI.h"

/* Digital pin number for the displays chip select pin */
#define CS_DI 4
/* Digital pin number for the displays data/command pin */
#define DC_DI 3
/* Digital pin number for the displays reset pin */
#define RST_DI 0

/* Create an instance of the library (uncomment one of the lines below) */
HCuOLED HCuOLED(SSD1307, CS_DI, DC_DI, RST_DI); // For SSD1307 displays (HCMODU0050 & HCMODU0052)
//HCuOLED HCuOLED(SH1106, CS_DI, DC_DI, RST_DI); // For SH1106 displays (HCMODU0058 & HCMODU0059)

 #define W_CLK 8       // Pin 8 - connect to AD9850 module word load clock pin (CLK)
 #define FQ_UD 9       // Pin 9 - connect to freq update pin (FQ)
 #define DATA 11       // Pin 10 - connect to serial data load pin (DATA)
 #define RESET 12      // Pin 11 - connect to reset pin (RST).

//AD8302 output connections
int COM_Pin = A2;
int VREF_Pin = A3;  
int VM_Pin = A4;
int VP_Pin = A5;

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

#define pulseHigh(pin) {digitalWrite(pin, HIGH); delay(1); digitalWrite(pin, LOW); }

//Used to try to track the data and store it into python
int FreqPin = A0; // Pin A0 is the data pin
float FreqValue = 0;

//Create the starting freq value
float StartFreq = (660.e4);
float CurrentFreq = 0;
float IntervalFreq = (5.e3);
float FinalFreq = (69.e5);


// transfers a byte, a bit at a time, LSB first to the 9850 via serial DATA line
void tfr_byte(byte data)
{
  for (int i=0; i<8; i++, data>>=1) {
    digitalWrite(DATA, data & 0x01);
    pulseHigh(W_CLK);   //after each bit sent, CLK is pulsed high
  }
}

 // frequency calc from datasheet page 8 = <sys clock> * <frequency tuning word>/2^32
 // Dont change this freq, this is for the calibration in regards to the clock, Change freq at bottom
void sendFrequency(double frequency) {
  int32_t freq = frequency * 4294967295/125000000;  // note 125 MHz clock on 9850
  for (int b=0; b<4; b++, freq>>=8) {
    tfr_byte(freq & 0xFF);
  }
  tfr_byte(0x000);   // Final control byte, all 0 for 9850 chip
  pulseHigh(FQ_UD);  // Done!  Should see output
}



void setup() {

  /* Reset the display */
  HCuOLED.Reset();
  
 // configure arduino data pins for output
  pinMode(FQ_UD, OUTPUT);
  pinMode(W_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(RESET, OUTPUT);
   
  pulseHigh(RESET);
  pulseHigh(W_CLK);
  pulseHigh(FQ_UD);  // this pulse enables serial mode - Datasheet page 12 figure 10

  //configure the output of the data
  Serial.begin(9600);

  sendFrequency(StartFreq);
  CurrentFreq = StartFreq;
  // freq

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

//Change freq here
void loop() {
  //if (Serial.available() && Serial.read() == 'r') {
  //ans = 1;
  //}
  //if (Serial.available() && Serial.read() == 's') {
  //ans = 2;
  //}

  //if (ans == 1) {
    while(CurrentFreq < FinalFreq) {
     
      delay(100);
  
      //Reading the analog pins on the Arduinio    
      FreqValue = analogRead(FreqPin);  // read pin A0
      VREFValue = analogRead(VREF_Pin);
      VMValue = analogRead(VM_Pin);
      VPValue = analogRead(VP_Pin);
      COMValue = analogRead(COM_Pin);
      //Serial.println(FreqValue);         // send data to serial
  
      sendFrequency(CurrentFreq);
      //Serial.println(CurrentFreq);
  
      String p1=",";
      Serial.println(CurrentFreq +  p1 + VREFValue + p1 + VMValue + p1 + VPValue + p1 + COMValue);
      CurrentFreq = CurrentFreq + IntervalFreq;
    //Serial.println("Data Collection Complete");
    //ans == 0;
    //break;
      }
  

    //if (ans == 2) {
    if (Serial.available() > 0) {

      Serial.write("Connection");
      input = Serial.readStringUntil('\n');
      maxfreq = input.toFloat();
      
      Serial.print(input);
      Serial.print(maxfreq);
  
      //SHOW ON LED SCREEN
      /* Display a number using a large 4 line LCD style font */
      HCuOLED.Cursor(28,40);
      HCuOLED.SetFont(LCDLarge_24pt);
      HCuOLED.Print(maxfreq, 2);
    
      /* Output the display buffer to the screen */
      HCuOLED.Refresh();
      
      sendFrequency(maxfreq);
  
      Serial.print("freq sent");
      //ans == 0;
 
      }
  
 }
   
