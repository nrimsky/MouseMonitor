#include <SoftwareSerial.h> 

SoftwareSerial BlueTooth(10, 11); // RX | TX 

const double samplingFrequency = 20; // Hz, must be less than 10000 due to ADC

unsigned int sampling_period_us;
unsigned long microseconds;
int reading;
int mock_data_length = 80;
float mock_data[80] = {0.17,0.06,0.16,0.1,0.15,0.14,0.15,0.18,0.16,4.46,5.0,1.16,0.0,0.1,0.0,0.15,0.0,0.17,0.0,0.17,0.0,0.18,0.03,0.17,0.08,0.17,4.68,2.82,0.38,0.02,0.06,0.03,0.1,0.01,0.17,0.0,0.22,0.0,0.25,0.02,0.25,1.56,5.0,0.0,0.12,0.02,0.14,0.06,0.12,0.1,0.11,0.12,0.1,0.14,0.09,0.19,0.04,0.23,4.78,5.0,0.33,0.32,0.02,0.22,0.04,0.22,0.07,0.2,0.1,0.17,0.13,0.16,0.15,0.15,0.19,5.0,4.83,0.0,0.23,0.13};
int i = 0;

void setup() {
  sampling_period_us = round(1000000*(1.0/samplingFrequency));
  BlueTooth.begin(9600); 
  Serial.begin(9600);
}

void loop() {
  microseconds = micros();
  while (true) {
    if (i>= mock_data_length) {
      i = 0;
    }
    reading = mock_data[i]*100;
    i++;
    Serial.println(reading);
    BlueTooth.print(reading, DEC);
    BlueTooth.print(",");
    while(micros() - microseconds < sampling_period_us){}
    microseconds += sampling_period_us;
  }; 
}
