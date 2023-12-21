#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define WIFI_SSID "ENARXI"
#define WIFI_PASSWORD "Enarxi123@"
#define FIREBASE_HOST "https://majestic-10e3f.firebaseio.com/"
#define FIREBASE_AUTH "257xo0NgKT5RtxOtDXtJ00XgsGUAfnKHbmDFLpiU"

int a=0;
float farh2;
float farh1;
float mv;
float cel;
int val;
int tempPin = 39;
int b=0;
int vbmotor = 2;
int acpt = 34;
int rjct =35;
FirebaseData fbdo;
int rssi;
int Stat;
String dev1;
LiquidCrystal_I2C lcd(0x27, 16, 2);
void set_as_broadcaster(void);
void safedistance();
void donothing();
void accepts();
void rejects();
void readtemp();


void setup() 
{
  Stat=0;
  pinMode(vbmotor,OUTPUT);
  pinMode(acpt,INPUT_PULLUP);
  pinMode(rjct,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(acpt), accepts , FALLING);
  attachInterrupt(digitalPinToInterrupt(rjct), rejects , FALLING);
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  set_as_broadcaster();
  lcd.begin();
  lcd.backlight();
 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    lcd.setCursor(0,0);
    lcd.print("Not Connected");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connected");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

}

void loop() {
    readtemp();
    if(Firebase.setFloat(fbdo, "/TEMP2", farh2))
    {
      Serial.print(farh2);
   Serial.println("Success");
   }
   else
   {
    Serial.print("Error in setInt, ");
    Serial.println(fbdo.errorReason());
  }
  if(Firebase.getInt(fbdo, "/RSSI"))
  {
    //Success
      //Serial.println(fbdo.intData());
      rssi = -(fbdo.intData());
      Serial.print(rssi);
      }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in getInt, ");
    Serial.println(fbdo.errorReason());
  }
   
   if(Firebase.getFloat(fbdo, "/TEMP1"))
  {
   farh1=fbdo.floatData();
   Serial.print(farh1);
   Serial.println("Received");
   }
   else
   {
    Serial.print("Error, ");
    Serial.println(fbdo.errorReason());
  }
if(Firebase.getString(fbdo, "/Stat1"))
{
 String checkg = fbdo.stringData(); 
 Stat = checkg.toInt();
 Serial.println(Stat);
 delay(500);
}
if (Stat == 1){
  if(Firebase.getString(fbdo, "/dev1"))
  {
  cdata();
    }   
    else
    { 
     Serial.print("Error in getInt, ");
     Serial.println(fbdo.errorReason());
  }
}
if(Stat == 0)
{
if(rssi <=58)
{
  safedistance();
}
if(rssi>58)
{
  donothing();
}
}

if(a == 1)
{
 Firebase.setString(fbdo, "/ans1" ,"Accpeted");
 Firebase.setString(fbdo, "/Stat1","0");
 a=0;
}
if(b == 1)
{
 Firebase.setString(fbdo, "/ans1","Rejected");
 Firebase.setString(fbdo, "/Stat1","0");
 b=0;
}
}


void set_as_broadcaster(void)
{
  BLEDevice::init("ble server");
  //BLEDevice::setPower(ESP_PWR_LVL_P7);
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Hello World");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}
void safedistance()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Safe dist alert");
  digitalWrite(vbmotor,HIGH);
  delay(500);
  lcd.setCursor(0,1);
  lcd.print("M:");
  lcd.print(farh2);
  lcd.setCursor(6,1);
  lcd.print("  O:");
  lcd.print(farh1);
}
void donothing()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Scaning");
  digitalWrite(vbmotor,LOW);
  delay(500);
}
void cdata()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(fbdo.stringData());
}
void accepts(){
  a = 1;
  Serial.println("acpt");
 }
void rejects(){
 b=1;
 Serial.println("rjct");
}
void readtemp()
{
  val = analogRead(tempPin);
  mv = (val*1.221);
  farh2 = mv/10;
  //farh2 = (cel*9)/5 + 32;
  lcd.setCursor(0,1);
  lcd.print("M:");
  lcd.print(farh2);
  delay(10);
}
