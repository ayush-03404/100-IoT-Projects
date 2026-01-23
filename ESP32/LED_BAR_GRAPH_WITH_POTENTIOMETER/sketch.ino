// Biến trở nối vào chân GPIO 1 (Analog ADC1_CH0) 
const int potPin = 1;
int potValue = 0;
int Led[10]={11,10,9,46,3,8,18,17,16,15};
int ledVal[10]={400,800,1200,1600,2000,2400,2800,3200,3600,4000};
int i;
void setup(){
  Serial.begin(115200);
  //led la ngo ra
  for(i=0;i<10;i++){
    pinMode(Led[i], OUTPUT);
  }
  delay(1000);
}

void loop(){
  potValue = analogRead(potPin);
  for(i=0;i<10;i++){
    if(potValue>ledVal[i]){
      digitalWrite(Led[i], LOW);
    }
    else{
      digitalWrite(Led[i], HIGH);
    }
  }
  Serial.println(potValue);
  delay(500);
}