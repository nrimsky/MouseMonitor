#include <SoftwareSerial.h> 
#define CHANNEL A0

SoftwareSerial BlueTooth(10, 11); // RX | TX 

const double samplingFrequency = 10; // Hz, must be less than 10000 due to ADC

unsigned int sampling_period_us;
unsigned long microseconds;
int reading;

void setup() {
  sampling_period_us = round(1000000*(1.0/samplingFrequency));
  BlueTooth.begin(9600); 
  Serial.begin(9600);
}

void loop() {
  microseconds = micros();
  while (true) {
    reading = analogRead(CHANNEL);
    Serial.println(reading);
    BlueTooth.print(reading, DEC);
    BlueTooth.print(",");
    while(micros() - microseconds < sampling_period_us){}
    microseconds += sampling_period_us;
  }; 
}
