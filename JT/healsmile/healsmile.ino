#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>


#include "OLED_Driver.h"
#include "OLED_GUI.h"
#include "DEV_Config.h"
#include "Show_Lib.h"
#include "Debug.h"

const int MPU = 0x68;  //MPU 6050 의 I2C 기본 주소
int16_t AcX,AcY,AcZ, Tmp, GyX,GyY,GyZ;

//U8G2_SSD1327_WS_128X128_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 12, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 7, /* reset=*/ 8);//

  
#define SYSTEM_STATUS_FLOAT 10
#define SYSTEM_STATUS_ERROR 20
#define SYSTEM_STATUS_READY 30
#define SYSTEM_STATUS_PLAYING 40
#define SYSTEM_STATUS_END 50

#define PLAY_TIMEOUT 29000


long int end_check_time = -1;
int system_status = SYSTEM_STATUS_FLOAT;
int inputPin = 2;   // choose input pin 7 for the push button
long count = 0;
char charCount ="";
long timer = 0;
long gyroval = 0;
bool checkCount = true;


extern OLED_DIS sOLED_DIS;
void setup() {
  System_Init();
  pinMode(inputPin, INPUT); // declare push button as input
  count = -1;

  Serial.println("OLED Init ");
  OLED_SCAN_DIR OLED_ScanDir = SCAN_DIR_DFT;
  OLED_Init( OLED_ScanDir );

  Serial.println("OLED_Show ");
  //GUI_Show();
  OLED_ClearBuf();
  OLED_ClearScreen(OLED_BACKGROUND);

  GUI_DrawLine(0, 8, sOLED_DIS.OLED_Dis_Column - 1, 8, WHITE, LINE_SOLID , DOT_PIXEL_DFT);
  OLED_DisPage(0, 0);
  OLED_DisPage(0, 7);
  OLED_ClearBuf();


  GUI_DisString_EN(30, 2, "ELECTRONIC", &Font12, FONT_BACKGROUND, WHITE);
  OLED_DisPage(0, 2);
  OLED_ClearBuf();

//  GUI_DrawCircle(30,10,5,WHITE,DRAW_FULL,1);
//
//  GUI_Disbitmap(0  , 2, Signal816  , 16, 8);
//  GUI_Disbitmap(24 , 2, Bluetooth88, 8 , 8);
//  GUI_Disbitmap(40 , 2, Msg816     , 16, 8);
//  GUI_Disbitmap(64 , 2, GPRS88     , 8 , 8);
//  GUI_Disbitmap(90 , 2, Alarm88    , 8 , 8);
  GUI_Disbitmap(112, 2, Bat816     , 16, 8);
  OLED_Display(0, 52, 128, 52 + 16);
  OLED_ClearBuf();

  GUI_DisString_EN(0 , 0, "MUSIC", &Font12, FONT_BACKGROUND, WHITE);
  GUI_DisString_EN(48, 0, "MENU" , &Font12, FONT_BACKGROUND, WHITE);
  GUI_DisString_EN(90, 0, "PHONE", &Font12, FONT_BACKGROUND, WHITE);
  
  OLED_Display(0, 96, 128, 96 + 16);
  OLED_ClearBuf();
  
  Wire.begin();      //Wire 라이브러리 초기화
  Wire.beginTransmission(MPU); //MPU로 데이터 전송 시작
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     //MPU-6050 시작 모드로
  Wire.endTransmission(true);
  
  Serial.begin(9600);
  Serial.println("serial 9600 begin");

  
  system_status = SYSTEM_STATUS_READY;
  Serial.println("system ready");
 
}



uint8_t draw_state = 0;



void loop() {
  
  if (system_status == SYSTEM_STATUS_READY) {
      delay(100);
      

    
      int pushed = digitalRead(inputPin);  // read input value

      if (pushed == HIGH) { //push button
        count = 0;
        Serial.println("HIGH");

        pinMode(inputPin, INPUT_PULLUP);
        delay(1000);
        Serial.println("3");
        timer = 3;

        delay(1000);
        Serial.println("2");
        timer = 2;
        delay(1000);
        timer = 1;
        Serial.println("1");
        delay(1000);
        count = 0;
        system_status = SYSTEM_STATUS_PLAYING;
        
      }else{
        
      }
      
    
  } else if (system_status == SYSTEM_STATUS_PLAYING) {//game playing -> count
    
    int pushed = digitalRead(inputPin);  // read input value

    if (inputPin == LOW) {
      Serial.println("LOW");
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
    gyroval = val_y;
   
   
    
    if(gyroval < -70){
      if(checkCount == true){
          checkCount = false;
          
          count = count +1;
         
          
          GUI_DisString_EN(30, 2, "O", &Font12, FONT_BACKGROUND, WHITE);
          OLED_DisPage(0, 2);
          OLED_ClearBuf();
      }
     
    }

     if(gyroval > -10){
      if(checkCount == false){
          checkCount = true;
          GUI_DisString_EN(30, 2, "X", &Font12, FONT_BACKGROUND, WHITE);
          OLED_DisPage(0, 2);
          OLED_ClearBuf();
      }
    }

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
    delay(2000);
    system_status = SYSTEM_STATUS_READY;
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
