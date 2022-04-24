
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

const int B_ledPin = 8; // the pin that the LED is attached to
const int R_ledPin = 9;
const int G_ledPin = 10;

int incomingByte;      // a variable to read incoming serial data into
String incomingByte2;
float timeon = 1000;
long randNumber;



void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  // initialize the LED pin as an output:
  pinMode(B_ledPin, OUTPUT);
  pinMode(R_ledPin, OUTPUT);
  pinMode(G_ledPin, OUTPUT);

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


    if (incomingByte == 'B') {
        
        
        incomingByte2 = Serial.read();
        timeon = incomingByte2.toInt();
        Serial.println(timeon);
        digitalWrite(B_ledPin, HIGH);

 
    }

    if (incomingByte == 'X') {
        
        
        incomingByte2 = Serial.read();
        timeon = incomingByte2.toInt();

        digitalWrite(B_ledPin, LOW);

 
    }

    if (incomingByte == 'R') {

        display.clearDisplay();
        
        
        incomingByte2 = Serial.readStringUntil('\n');
        timeon = incomingByte2.toInt();
        digitalWrite(R_ledPin, HIGH);
        
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println(timeon);
        display.println("Time in seconds");
        display.display();
  
        delay(timeon);
        Serial.println(timeon);
        digitalWrite(R_ledPin, LOW);
    }

    if (incomingByte == 'G') {

        
        incomingByte2 = Serial.read();
        
        digitalWrite(G_ledPin, HIGH);
        delay(1000);
        digitalWrite(G_ledPin, LOW);

    }
  }
}
