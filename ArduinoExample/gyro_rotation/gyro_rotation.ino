#include <EEPROM.h>

/*
    MPU6050 Triple Axis Gyroscope & Accelerometer. Pitch & Roll Accelerometer Example.
    Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/3-osiowy-zyroskop-i-akcelerometr-mpu6050.html
    GIT: https://github.com/jarzebski/Arduino-MPU6050
    Web: http://www.jarzebski.pl
    (c) 2014 by Korneliusz Jarzebski
*/

#include <Wire.h>
#include <MPU6050.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <SD.h>

byte mac[] =
{
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};


String string_udpsend = "";



//IP 세팅은 SD카드로 한다. (setSavedIp 함수 참고)
//펭귄레이스 어트랙션 IP
IPAddress PC_IP(192, 168, 0, 100);
//아두이노 IP
IPAddress DEVICE_IP(192, 168, 0, 111);
// the dns server ip
IPAddress DNS(192, 168, 0, 254);
// the router's gateway address:
IPAddress GATEWAY(192, 168, 0, 254);
// the subnet:
IPAddress SUBNET(255, 255, 255, 0);

EthernetUDP Udp;
MPU6050 mpu;

void setup() 
{
  Serial.begin(115200);

  Serial.println("Initialize MPU6050");

//  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
//  {
//    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
//    delay(500);
//  }
  mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G);
  Serial.println("PC_IP");
  Serial.println(PC_IP);
  
  Serial.println("DEVICE_IP");
  Serial.println(DEVICE_IP);
  
  Serial.println("DNS");
  Serial.println(DNS);
  
  Serial.println("GATEWAY");
  Serial.println(GATEWAY);
  
  Serial.println("SUBNET");
  Serial.println(SUBNET);
  
  // initialize the ethernet device
  Ethernet.begin(mac, DEVICE_IP, DNS, GATEWAY, SUBNET);

  Serial.println(Ethernet.localIP());

  Udp.begin(8888);
  
  Serial.println("UDP Server is listening..");
}

void loop()
{
  // Read normalized values 
  Vector normAccel = mpu.readNormalizeAccel();
//
//  // Calculate Pitch & Roll
  int pitch = -(atan2(normAccel.XAxis, sqrt(normAccel.YAxis*normAccel.YAxis + normAccel.ZAxis*normAccel.ZAxis))*180.0)/M_PI;
  int roll = (atan2(normAccel.YAxis, normAccel.ZAxis)*180.0)/M_PI;
//
//  // Output
////  Serial.print(" Pitch = ");
////  Serial.print(pitch);
////  Serial.print(" Roll = ");
////  Serial.print(roll);
  string_udpsend =  String(pitch)+"," + String(roll);
  sendUdp(string_udpsend);
  Serial.println(string_udpsend);
  
  delay(200);
}

void sendUdp(String push)
{
  Udp.beginPacket(PC_IP, 8888);
  Udp.print(push);
  Udp.endPacket();
}

///*
//    MPU6050 Triple Axis Gyroscope & Accelerometer. Simple Gyroscope Example.
//    Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/3-osiowy-zyroskop-i-akcelerometr-mpu6050.html
//    GIT: https://github.com/jarzebski/Arduino-MPU6050
//    Web: http://www.jarzebski.pl
//    (c) 2014 by Korneliusz Jarzebski
//*/
//
//#include <Wire.h>
//#include <MPU6050.h>
//
//MPU6050 mpu;
//
//void setup() 
//{
//  Serial.begin(115200);
//
//  // Initialize MPU6050
//  Serial.println("Initialize MPU6050");
//  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
//  {
//    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
//    delay(500);
//  }
//  
//  // If you want, you can set gyroscope offsets
//  // mpu.setGyroOffsetX(155);
//  // mpu.setGyroOffsetY(15);
//  // mpu.setGyroOffsetZ(15);
//  
//  // Calibrate gyroscope. The calibration must be at rest.
//  // If you don't want calibrate, comment this line.
//  mpu.calibrateGyro();
//
//  // Set threshold sensivty. Default 3.
//  // If you don't want use threshold, comment this line or set 0.
//  // mpu.setThreshold(3);
//  
//  // Check settings
//  checkSettings();
//}
//
//void checkSettings()
//{
//  Serial.println();
//  
//  Serial.print(" * Sleep Mode:        ");
//  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");
//  
//  Serial.print(" * Clock Source:      ");
//  switch(mpu.getClockSource())
//  {
//    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
//    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
//    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
//    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
//    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
//    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
//    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
//  }
//  
//  Serial.print(" * Gyroscope:         ");
//  switch(mpu.getScale())
//  {
//    case MPU6050_SCALE_2000DPS:        Serial.println("2000 dps"); break;
//    case MPU6050_SCALE_1000DPS:        Serial.println("1000 dps"); break;
//    case MPU6050_SCALE_500DPS:         Serial.println("500 dps"); break;
//    case MPU6050_SCALE_250DPS:         Serial.println("250 dps"); break;
//  } 
//  
//  Serial.print(" * Gyroscope offsets: ");
//  Serial.print(mpu.getGyroOffsetX());
//  Serial.print(" / ");
//  Serial.print(mpu.getGyroOffsetY());
//  Serial.print(" / ");
//  Serial.println(mpu.getGyroOffsetZ());
//  
//  Serial.println();
//}
//
//void loop()
//{
//  Vector rawGyro = mpu.readRawGyro();
//  Vector normGyro = mpu.readNormalizeGyro();
////
//  Serial.print(" Xraw = ");
//  Serial.print(rawGyro.XAxis);
//  Serial.print(" Yraw = ");
//  Serial.print(rawGyro.YAxis);
//  Serial.print(" Zraw = ");
//  Serial.println(rawGyro.ZAxis);
//
////  Serial.print(" Xnorm = ");
////  Serial.print(normGyro.XAxis);
////  Serial.print(" Ynorm = ");
////  Serial.print(normGyro.YAxis);
////  Serial.print(" Znorm = ");
////  Serial.println(normGyro.ZAxis);
//  
//  delay(1000);
//}
