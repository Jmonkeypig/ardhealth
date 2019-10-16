#include <SoftwareSerial.h>

SoftwareSerial BTSerial(2,3); //RX,TX

void setup(){
  Serial.begin(9600);
  BTSerial.begin(9600);
  
}

void loop(){
  if(BTSerial.available()){
    Serial.write(BTSerial.read());
    byte data = BTSerial.read();
    Serial.println(data);
    if(data == 255){
          Serial.println("test123");
     }
//    Serial.println("test123");
   }


   if(Serial.available()){
    BTSerial.write(Serial.read());
    }
}
