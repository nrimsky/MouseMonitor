#include <SoftwareSerial.h> 

SoftwareSerial BlueTooth(10, 11); // RX | TX 
const int PIEZO_PIN = A0; // Piezo output
int piezoADC;

void setup(){  
  Serial.begin(9600); 
  BlueTooth.begin(9600); 
} 

void loop(){ 
  piezoADC = analogRead(PIEZO_PIN);
  if (piezoADC > 50) {
      BlueTooth.print(piezoADC, DEC);
      BlueTooth.print(",");
      Serial.println(piezoADC, DEC);
  }
  delay(10);
}  
