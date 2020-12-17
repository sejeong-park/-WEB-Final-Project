#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h> //시리얼통신 라이브러리 호출
SoftwareSerial btSerial(5,4);  //PX,TX시리얼 통신을 위한 객체선언
LiquidCrystal lcd(7,8,9,10,11,12);
String myString="";

#define DHT11_PIN 2
int Sensor_pin = 2;
DHT my_sensor(Sensor_pin,DHT11);

float value=0;
float voltage=0;
float dustDensity=0;

int Echo_Pin=15;
int Trig_Pin=16;
int ALARM=13;
int cnt=0;

int relay=6;
byte buffer[3];

void setup(){
  lcd.begin(16,2); //lcd
  Serial.begin(9600);
  btSerial.begin(9600); //블루투스 시리얼
  my_sensor.begin();
  
  pinMode(3,OUTPUT); //미세먼지 센서
  pinMode(A0,INPUT);

  pinMode(Echo_Pin,INPUT); //초음파센서
  pinMode(Trig_Pin,OUTPUT);

  pinMode(relay,OUTPUT); //릴레이모듈
 
}

void loop(){
  char data=btSerial.read();
    if (Serial.available()) {       
    btSerial.write(Serial.read());  
  }
  if (btSerial.available()) {         
    char myChar=(char)btSerial.read();  
    myString+=myChar;    
    delay(5); 
  }


  int temp= my_sensor.readTemperature();
  int humi=my_sensor.readHumidity();
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" ℃");
  Serial.print("  Humidity: ");
  Serial.print(humi);
  Serial.println(" %");

  lcd.setCursor(0,0);
  lcd.print("Temperature: ");
  lcd.print(temp);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(humi);
  lcd.print("%");
  
  digitalWrite(3,LOW);
  delayMicroseconds(10);
  value=analogRead(A0);
  delayMicroseconds(10);
  digitalWrite(3,HIGH);
  delayMicroseconds(10);

  voltage=value*(5.0/1024.0);
  dustDensity=voltage/0.005;
  Serial.print("Dust Density : ");
  Serial.print(dustDensity);
  Serial.println(" ㎍/㎥");
  
  long duration,distance;
  digitalWrite(Trig_Pin,HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig_Pin,LOW);
  duration=pulseIn(Echo_Pin,HIGH);
      
  distance=((float)(340*duration)/10000)/2;
      
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
 
  if(distance<=10){
    cnt++;
    if(cnt>=2){
    tone(ALARM,1000);
  }
  }
  else{
    noTone(ALARM);
    cnt=0;
  }
  
  if(data=='1'){ //앱인벤터에서 1을 받아오면 전원 ON
    digitalWrite(relay,HIGH);
    delayMicroseconds(10);
  }
   if(data=='0'){ //앱인벤터에서 0을 받아오면 전원 OFF
    digitalWrite(relay,LOW);
    delayMicroseconds(10);
  }

 buffer[0]=(byte)temp;
 buffer[1]=(byte)humi;
 buffer[2]=(byte)dustDensity;
 btSerial.write(buffer,3);
 delay(2000);
}
