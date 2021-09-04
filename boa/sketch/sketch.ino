/*
BOA 스케치
*/

int A = 8;
int B = 9;
 
void setup() {
  pinMode(A,OUTPUT);
  pinMode(B,OUTPUT);
  Serial.begin(9600);  
}

int input = '0';

void all_stop() {
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
}
 
void loop() {
  if (Serial.available() > 0) {
    input = Serial.read();
    all_stop();
  }

  if (input == '1') {
    digitalWrite(A,HIGH);
  } else if (input == '2') {
    digitalWrite(B, HIGH);
  } else if (input == '0') {
    all_stop();
  }
}
