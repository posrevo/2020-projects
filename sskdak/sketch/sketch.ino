/*
쓱딱 스케치
*/

/* 라이브러리 include */
#include <HCMotor.h>

/* 모터드라이버 연결핀 */
#define DIR_PIN 8 //스텝모터드라이버 DIR 연결핀
#define CLK_PIN 9 //스텝모터드라이버 CLK 연결핀

/* 리미트 스위치 연결핀 */
#define RLIMIT_PIN 2

#define CLK 100
#define MAXP 58000

/* HCMotor 라이브러리 인스턴스 생성 */
HCMotor HCMotor;

char Incoming = 0;
unsigned int pos=0;

void setup() 
{

  /* 라이브러리 초기화 */
  HCMotor.Init();

  /* 모터0을 스텝모터로 설정하고 연결된 핀을 지정 */
  HCMotor.attach(0, STEPPER, CLK_PIN, DIR_PIN);

  /* 리미트 스위치 핀 지정 */
  pinMode(RLIMIT_PIN, INPUT);

  /* 모터를 연속동작모드로 설정*/
  HCMotor.Steps(0,CONTINUOUS);

  /* set serial */
  Serial.begin(9600);

  /* 위치 사전 보정 */
  recalibrate();
}

int fullSpeed = 10;

void recalibrate()
{
  HCMotor.Direction(0, REVERSE);
  for (;;) {
    if (digitalRead(RLIMIT_PIN)==LOW) {
      break;
   }
    go_and_stop(REVERSE, CLK);
  }
  HCMotor.DutyCycle(0, 0);
  pos=0;
}

void go_and_stop(boolean direction, unsigned int time) {
  HCMotor.Direction(0, direction);
  HCMotor.DutyCycle(0, fullSpeed);
  delay(time);
  HCMotor.DutyCycle(0, 0);
  return 0;
}

void zero() {
  HCMotor.DutyCycle(0, 0);
}

void loop() 
{
  // 값이 들어올 때만 Incoming 변경
  if(Serial.available() > 0) {
    Incoming = Serial.read();
    if (Incoming=='0') {
      Serial.println("STOP");
    } else if (Incoming == '1') {
      Serial.println("FORWARD");
    } else if (Incoming == '2') {
      Serial.println("REVERSE");
    }
  }
  // 멈추기
  if (Incoming == '0') {
    //Serial.print("** STOP ** \n");
    zero();
  // 정방향
  } else if (Incoming == '1') {
    //Serial.print("** FORWARD **\n");
    if(pos<MAXP) {
      go_and_stop(FORWARD, CLK);
      pos+=CLK;
    }
  // 역방향 
  } else if (Incoming == '2') {
    //Serial.print("** REVERSE **\n");
    if(pos>0&&digitalRead(RLIMIT_PIN)) {
      go_and_stop(REVERSE, CLK);
      pos-=CLK;
    } else if (digitalRead(RLIMIT_PIN)==LOW) {
      Incoming='0';
      pos=0;
    }
  }

} 
