
#define CHANNEL A0

// Sample Frequency in Hz
const float sample_freq = 20;
const int num_samples = 100;

unsigned int sampling_period_us;
unsigned long microseconds;

void setup() {
    sampling_period_us = round(1000000*(1.0/sample_freq));
    Serial.begin(9600);
}

float get_main_freq(int *rawData, int len) {
  int i,k;
  int sum, sum_old;
  int thresh = 0;
  byte pd_state = 0;
  sum = 0;
  pd_state = 0;
  int period = 0;
  // Autocorrelation
  for(i=0; i < len; i++){
    sum_old = sum;
    sum = 0;
    for(k=0; k < len-i; k++) sum += rawData[k]*rawData[k+i];
    // Peak Detect State Machine
    if (pd_state == 2 && (sum-sum_old) <=0){
      period = i;
      pd_state = 3;
    }
    if (pd_state == 1 && (sum > thresh) && (sum-sum_old) > 0) pd_state = 2;
    if (!i) {
      thresh = sum * 0.5;
      pd_state = 1;
    }
  }
  // Frequency identified in Hz
  return sample_freq/period;
  
}

int piezo_data[num_samples];

void loop() {
  microseconds = micros();
  for(int i=0; i<num_samples; i++){
    piezo_data[i] = analogRead(CHANNEL);
    while(micros() - microseconds < sampling_period_us){}
    microseconds += sampling_period_us;
  }
  float mf = get_main_freq(piezo_data, num_samples);
  // Print in BPM
  Serial.println(mf*60,3);
} 
