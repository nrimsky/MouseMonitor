#include <SoftwareSerial.h> 

SoftwareSerial BlueTooth(10, 11); // RX | TX 
const int PIEZO_PIN = A0; // Piezo output
const int moving_avg_size = 5;
int piezoADC;

int buf[moving_avg_size];
int read_index = 0;
int tot = 
0;
double avg = 0;

void setup(){  
  Serial.begin(9600); 
  BlueTooth.begin(9600); 
  for (int z = 0; z < moving_avg_size; z++) {
    buf[z] = 0;
  }
} 

void loop(){ 
  piezoADC = analogRead(PIEZO_PIN);
//  BlueTooth.print(piezoADC, DEC);
//  BlueTooth.print(",");
//  tot-=buf[read_index];
//  buf[read_index] = piezoADC;
//  tot+= piezoADC;
//  read_index+=1;
//  if (read_index >= moving_avg_size) {
//    read_index = 0;
//  }
//  avg = tot/moving_avg_size;
  Serial.println(piezoADC);
  delay(1);
}  
