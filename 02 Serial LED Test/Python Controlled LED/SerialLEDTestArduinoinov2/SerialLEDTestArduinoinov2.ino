



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
}

void loop() {
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

        
        incomingByte2 = Serial.readStringUntil('\n');
        timeon = incomingByte2.toInt();
        digitalWrite(R_ledPin, HIGH);
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
