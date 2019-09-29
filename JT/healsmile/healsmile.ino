#include<Wire.h>
 
const int MPU = 0x68;  //MPU 6050 의 I2C 기본 주소
int16_t AcX,AcY,AcZ, Tmp, GyX,GyY,GyZ;


#define SYSTEM_STATUS_FLOAT 10
#define SYSTEM_STATUS_ERROR 20
#define SYSTEM_STATUS_READY 30
#define SYSTEM_STATUS_PLAYING 40
#define SYSTEM_STATUS_END 50

#define PLAY_TIMEOUT 29000

long int end_check_time = -1;
int system_status = SYSTEM_STATUS_FLOAT;
int inputPin = 7;   // choose input pin 7 for the push button
int count = 0;

void setup() {

  pinMode(inputPin, INPUT); // declare push button as input

  
  Wire.begin();      //Wire 라이브러리 초기화
  Wire.beginTransmission(MPU); //MPU로 데이터 전송 시작
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     //MPU-6050 시작 모드로
  Wire.endTransmission(true);
  
  Serial.begin(9600);
  
  system_status = SYSTEM_STATUS_READY;

  Serial.println("serial 9600 gegin");
  system_status = SYSTEM_STATUS_READY;
  Serial.println("system ready");
 
}

void loop() {

  //ready
  if (system_status == SYSTEM_STATUS_READY) {
      delay(100);
      
      

//      end_check_time = millis();
    
      int pushed = digitalRead(inputPin);  // read input value

      if (pushed == LOW) {
//        Serial.println("LOW");
      }

      if (pushed == HIGH) {
        count = 0;
        
        Serial.println("HIGH");

        pinMode(inputPin, INPUT_PULLUP);
        delay(1000);
        Serial.println("3");
        delay(1000);
        Serial.println("2");
        delay(1000);
        Serial.println("1");
        system_status = SYSTEM_STATUS_PLAYING;
        
      }
    
  } else if (system_status == SYSTEM_STATUS_PLAYING) {//game playing -> count
    int pushed = digitalRead(inputPin);  // read input value

    if (inputPin == LOW) {
      Serial.println("HIGH");
      system_status = SYSTEM_STATUS_READY;
    }
    
    delay(100);
    Wire.beginTransmission(MPU);    //데이터 전송시작
    Wire.write(0x3B);               // register 0x3B (ACCEL_XOUT_H), 큐에 데이터 기록
    Wire.endTransmission(false);    //연결유지
      
    Wire.requestFrom(MPU,6,true);  //MPU에 데이터 요청
    
    
    //데이터 한 바이트 씩 읽어서 반환
    AcX = Wire.read() << 8 | Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
    AcY = Wire.read() << 8 | Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ = Wire.read() << 8 | Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    //Tmp = Wire.read() << 8 | Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    //GyX = Wire.read() << 8 | Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    //GyY = Wire.read() << 8 | Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    //GyZ = Wire.read() << 8 | Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
    
    
    float RADIAN_TO_DEGREES = 180/3.14139;
    float val_y = atan(AcX/sqrt(pow(AcY,2) + pow(AcZ,2))) * RADIAN_TO_DEGREES;
    float val_x = atan(AcY/sqrt(pow(AcX,2) + pow(AcZ,2))) * RADIAN_TO_DEGREES;
    
    //시리얼 모니터에 출력
    //Serial.print("AcX = "); 
    Serial.println(val_y);
  //Serial.print(" | AcY = "); Serial.println(val_x);
//  Serial.print(" | AcZ = "); Serial.println(AcZ);
  //Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  
//  Serial.print(" | GyX = "); Serial.println(GyX);
  //Serial.print(" | GyY = "); Serial.print(GyY);
//  Serial.print(" | GyZ = "); Serial.println(GyZ);


    
//    if (end_btn_status == LOW) {
//
//
//      Serial.println("end button pushed");
//      system_status = SYSTEM_STATUS_END;
//    }
    
  } else if (system_status == SYSTEM_STATUS_END) {
    delay(2000);
    system_status = SYSTEM_STATUS_READY;
  } else if (system_status == SYSTEM_STATUS_ERROR) {

  }

  
//  if (end_check_time != -1) {
//    if ((end_check_time + PLAY_TIMEOUT) < millis()) {
//      Serial.print(end_check_time); Serial.print(" "); Serial.println(millis());
//      end_check_time = -1;
//
//      system_status = SYSTEM_STATUS_END;
//    }
//  }
}
