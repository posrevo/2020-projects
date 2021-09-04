/*
아이두 스케치

사용하는 센서
- 자이로 센서
- 블루투스 모듈
- 푸시 버튼
- 압력 센서
- LCD
*/


#include <SoftwareWire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
const int button = 5;
const int pressSensor = A1;
SoftwareWire gyro(2, 3);
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

  lcd.setCursor(0, 1);
  lcd.print("GO FOR 10 HOURS!");
  delay(3000);
}

int hour = 0;
int minute = 0;
int second = -1;
int idotime = 0;

// 현재 시간을 출력한다.
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
int half = false;

void loop() {
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

  int nowMovX = AcX;
  int nowMovY = AcY;

  if(digitalRead(button))
  {
    idotime = hour*3600 + minute*60 + second;
    second=0;
    minute=0;
    hour=0;
    half=true;
    print_hour();
    return;
  }

  // if bluetooth input available
  if (Serial.available() > 0) {
    int input = Serial.read();
    if (input == '5') {
      Serial.println(idotime);
      idotime = 0; // reset idotime
    }
  }

  int value = analogRead(pressSensor); // read value from the sensor


  if( (false || (-500 <= beforeMovX-nowMovX && beforeMovX-nowMovX <= 500) || (-500 <= beforeMovY-nowMovY && beforeMovY-nowMovY <= 500)) && beforeMovX != 0 ) {
    return;
  }

  if (!half) {
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

  half = !half;
  delay(500);
}
