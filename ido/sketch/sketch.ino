#include <MPU6050.h>
#include <SoftwareWire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>


const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

const int trigPin = 10;
const int echoPin = 12;

SoftwareWire gyro(2, 3);


const int button = 5;
hd44780_I2Cexp lcd;

void setup() {
  /* for bluetooth */
  Serial.begin(9600); 

  /* button */
  pinMode(button, INPUT);

  /* lcd */
  lcd.begin(16,2); 

  /* gyro setup */
  gyro.begin();
  gyro.beginTransmission(MPU_addr);
  gyro.write(byte(0x6B));  // PWR_MGMT_1 register
  gyro.write(byte(0));     // set to zero (wakes up the MPU-6050)
  gyro.endTransmission(true);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  lcd.setCursor(0, 1);
  lcd.print("GO FOR 10 HOURS!");
  delay(1000);
}

int hour = 0;
int minute = 0;
int second = -1;

void print_hour() {
  lcd.setCursor(4, 0);
  if (hour < 10) {
    lcd.print(0);
  }
  lcd.print(hour);
  lcd.print(":");
  if (minute < 10) {
    lcd.print(0);
  }
  lcd.print(minute);
  lcd.print(":");
  if (second < 10) {
    lcd.print(0);
  }
  lcd.print(second);
}


int beforeMovX = 0;
int beforeMovY = 0;
int idotime = 0;

char input = '0';

void loop() {

  // if bluetooth input available
  if (Serial.available() > 0) {
    input = Serial.read();
  }
  
  gyro.beginTransmission(MPU_addr);
  gyro.write(byte(0x3B));  // starting with register 0x3B (ACCEL_XOUT_H)
  gyro.endTransmission(false);
  gyro.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=gyro.read()<<8|gyro.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=gyro.read()<<8|gyro.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=gyro.read()<<8|gyro.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=gyro.read()<<8|gyro.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=gyro.read()<<8|gyro.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=gyro.read()<<8|gyro.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=gyro.read()<<8|gyro.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  // Serial.println(AcX);

  int nowMovX = AcX;
  int nowMovY = AcY;



  if(digitalRead(button))
  {
    idotime += hour * 3600 + minute * 60 + second;
    second=0;
    minute=0;
    hour=0;
    print_hour();
    return;
  }
  

  float duration, distance;
  digitalWrite(trigPin, HIGH);
  delay(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = ((float)(340 * duration) / 10000) / 2;
    
/*
  if( (false || (-500 <= beforeMovX-nowMovX && beforeMovX-nowMovX <= 500) || (-500 <= beforeMovY-nowMovY && beforeMovY-nowMovY <= 500)) && beforeMovX != 0 ) {
    return;
  }
  */

  // 2021/01/10 펜 올려져 있을 때 시간 못 보내던 문제 아마도 해결

  if (input == '5') {

    if (idotime == 0) {
      Serial.println("--");
    } else if (idotime < 10) {
      Serial.print(0);
      Serial.println(idotime);
    } else {
      Serial.println(idotime);
    }
    idotime = 0; // reset idotime
    input = '0';
  } else if (input == '9' || distance < 20) {
    
  } else if (input == '0') {
    second = second + 1;
    if(second == 60) {
      minute = minute + 1;
      second = 0;

      if(minute == 60) {
        hour = hour + 1;
        minute = 0;
      }
    }
    print_hour();
  }

  /* uncomment for debugging
  Serial.print("AcX = "); Serial.println(AcX);
  Serial.print("pressSensor: "); Serial.println(value);
  Serial.print("difference: "); Serial.println(beforeMov-nowMov);
  Serial.print(minute);
  Serial.print(",");
  Serial.println(second);
  */


  beforeMovX = nowMovX;
  beforeMovY = nowMovY;
  
  delay(1000);
}
