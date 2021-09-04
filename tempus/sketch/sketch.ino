#include <Servo.h> 
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 7
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

Servo myservo;           
int pos = 0;            

void setup()
{
  myservo.attach(9);   
  Serial.begin(9600); 
  dht.begin();
}

void loop()
{
  float humi, temp;
  temp = dht.readTemperature();
  humi = dht.readHumidity();

  if(isnan(humi) || isnan(temp) ) {
    Serial.println("ERROR");
  }

  Serial.print("Ω¿µµ : ");
  Serial.println(humi);
  
  if ((15 <= humi) && (humi <= 20) ) {    
      for (; pos <= 90; pos += 1) {        
        myservo.write(pos);
        delay(15);        
      }
    }
  
  else if ((5 <= humi) && (humi <= 10)) {            
    for (; pos >= 0; pos -= 1) {             
      myservo.write(pos);              
      delay(15);                       
    }
  }
  delay(1000);
}
