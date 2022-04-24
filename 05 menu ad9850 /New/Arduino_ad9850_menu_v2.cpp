/*This is the arduino code to be used with the python code (ACS) to operate an acoustic deformation chamber */
/*For notes and use please vist - GITHUB "all rights reserved" */

/*By Zak Watts and Mimi Houlihan */


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   3
#define OLED_CLK   5
#define OLED_DC    6
#define OLED_CS    4
#define OLED_RESET 2


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

//below is probs not needed XXXXXXXXXXXXXXXXXXXXXXXx
#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

// Above not needed XXXXXXXXXXXXXXXXXXXXXX


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

      // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Welcome");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 17);
  display.println("This is");
  display.println("the ACS");
  display.println("by Mimi + Zak");

  display.display();

  delay(2000);


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

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Waiting for python signal");

  display.display();

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
    
        sendFrequency(CurrentFreq);
        //Serial.println(CurrentFreq);
    
        String p1=",";
        Serial.println(CurrentFreq +  p1 + VREFValue + p1 + VMValue + p1 + VPValue + p1 + COMValue);
        CurrentFreq = CurrentFreq + IntervalFreq;

        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println(CurrentFreq);
        display.println("Frequency in Hz");
        display.display();
    }
    }


    // if it's an S, get ready to recieve and send the driving frequency:
    if (incomingByte == 'S') {
      
        input = Serial.readStringUntil('\n');
        maxfreq = input.toInt();
        
        Serial.println(maxfreq);

        sendFrequency(maxfreq);

        Serial.println("freq sent");

        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println(maxfreq);
        display.println("Driven frequency");
        display.display();

    }


}







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
