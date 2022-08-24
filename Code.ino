/*
 * Kết nối:
 *        CB Mưa          Arduino
 *         VCC              5V
 *         GND              GND
 *         D0               4
 *         
 *         CB nắng
 *         SCL            22
 *         SDA            21
 *         
 *         LCD
 *         SCL            22
 *         SDA            21
 *         
 *         dong co
 *         in1            13 
 *         in2            14
 *         
 *         cong tac hanh trinh
 *         cong tac 1       16
 *         cong tac 2       17
 *         
 *         Nut nhan
 *         mode           19   
 *         Tiến           25  
 *         Lùi            26
 */


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BH1750.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); 
BH1750 lightMeter;

#include <WiFi.h>
#include "FirebaseESP32.h"
#include <ArduinoJson.h>
#define WIFI_SSID "PHONG PHU"
#define WIFI_PASSWORD "lamtruong"

#define FIREBASE_HOST "project-2-38914-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "F1LteIln4RVnIGMIqrh9KvsbavJd82yEHXtyM9p5"
FirebaseData firebaseData;
String path="/";
FirebaseJson json;

int in1=13;
int in2=14;

int congtac1 = 16; //công tac trong
int congtac2 = 17;


int Mode=1;
int nutnhan1=25;
int nutnhan2=26;

int value;
int giatri1;
int giatri2;
float lux;

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(115200); 
  Wire.begin();
  lightMeter.begin();
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(congtac1, INPUT);
  pinMode(congtac2, INPUT);
  pinMode(19, INPUT_PULLUP);
  pinMode(nutnhan1,INPUT_PULLUP);
  pinMode(nutnhan2,INPUT_PULLUP);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  if (!Firebase.beginStream(firebaseData, path))
   {
     Serial.println("REASON: " + firebaseData.errorReason());
     Serial.println();
   }
  Serial.print("connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

///////////////////////////////////

void tien(){
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  lcd.setCursor(0, 1);
  lcd.print("DC: ra  ");
  Firebase.setString(firebaseData, path + "/Trang thai dong co", "đang chay ra");
  }
 void lui(){
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  lcd.setCursor(0, 1);
  lcd.print("DC: vao ");
  Firebase.setString(firebaseData, path + "/Trang thai dong co", "đang chay vao");
  }
 void dung(){
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  lcd.setCursor(0, 1);
  lcd.print("DC:dung");  
  Firebase.setString(firebaseData, path + "/Trang thai dong co", "dung");
  }

/////////////////////////////////////

void Auto(){
  value = digitalRead(4);
  lux = lightMeter.readLightLevel();
  giatri1 = digitalRead(congtac1); // Đọc tín hiệu từ chân số 
  giatri2 = digitalRead(congtac2); // Đọc tín hiệu từ chân số 
  Serial.print("GIA TRI NHAN DUOC: ");
  Serial.println(giatri1);
  Serial.println(giatri2);
  lcd.setCursor(8, 1);
  lcd.print(lux);
  lcd.print("lx  ");
  Firebase.setFloat(firebaseData, path + "/Cam bien cuong do sang", lux);
if (value == HIGH) { // Cảm biến đang không mưa
    lcd.setCursor(0, 0);
    lcd.print("Khong mua");
    if (lux > 200 && giatri2 == 1) {
          tien();
          }
      else if (lux < 200 && giatri1 == 1) {
          lui();
          }
      else {dung();}
    
    Firebase.setString(firebaseData, path + "/Cam bien mua", "No Rain");
  } else {      /// có mưa
//    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dang mua ");
    if (giatri2 == 1){
       tien ();
    }
      else {
     dung();
    }
    Firebase.setString(firebaseData, path + "/Cam bien mua", "Rain");
  }

}

/////////////////////////////////////////
void Manual(){
  int trangthainut1=digitalRead(nutnhan1);
  int trangthainut2=digitalRead(nutnhan2);
  value = digitalRead(4);
  lux = lightMeter.readLightLevel();
  giatri1 = digitalRead(congtac1); // Đọc tín hiệu từ chân số 
  giatri2 = digitalRead(congtac2); // Đọc tín hiệu từ chân số 
  Serial.print("GIA TRI NHAN DUOC: ");
  Serial.print(giatri1);
  Serial.println(giatri2);
  lcd.setCursor(8, 1);
  lcd.print(lux);
  lcd.print("lx  ");
  Firebase.setFloat(firebaseData, path + "/Cam bien cuong do sang", lux);
  if (value == HIGH) { // Cảm biến đang không mưa
    lcd.setCursor(0, 0);
    lcd.print("Khong mua");
    Firebase.setString(firebaseData, path + "/Cam bien mua", "No Rain");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Dang mua ");
    Firebase.setString(firebaseData, path + "/Cam bien mua", "Rain");
  }
if  (trangthainut1 == 0 && trangthainut2 !=0 && giatri2 == 1 ){
    tien();
    }
else if  (trangthainut1 != 0 && trangthainut2 ==0 && giatri1 == 1){
    lui();
    }
else
    dung();
}

////////////////////////////////////////////

void loop() {
  int nutnhan = digitalRead(19);     
           
      if (nutnhan ==0)
        {
         delay (500);
          if (nutnhan ==0){
          Mode = Mode + 1;
            if(Mode > 2){
                Mode=1;
       }
         Serial.print("Mode: ");
         Serial.println(Mode);
                 
          }
        }
      if (Mode == 1) 
        {                           
            Auto();
            lcd.setCursor(10, 0);
            lcd.print("Auto  ");
            Firebase.setString(firebaseData, path + "/Che do", "Auto");
        }

      if (Mode == 2) 
        { 
            Manual();
            lcd.setCursor(10, 0);
            lcd.print("Manual");
            Firebase.setString(firebaseData, path + "/Che do", "Manual");
        }
}
