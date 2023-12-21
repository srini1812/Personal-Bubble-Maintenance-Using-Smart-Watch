#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define SERVICE_UUID        "d218b592-3b8c-471f-bedb-ee5943dcb1b9"
#define CHARACTERISTIC_UUID "e0df84b6-0257-4feb-abda-6b87f54b7f06"
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
int rssi;
int Stat;
String dev2;
int scanTime = 1; //In seconds
BLEScan* pBLEScan;
void set_as_observer(void);
int8_t RSSI_Val = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2);
void safedistance();
void donothing();
void accepts();
void rejects();
void readtemp();
FirebaseData fbdo;
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks 
{
    void onResult(BLEAdvertisedDevice advertisedDevice) 
    {
      RSSI_Val = advertisedDevice.getRSSI();
    }
};

void setup() 
{
  Stat=0;
  pinMode(vbmotor,OUTPUT);
  pinMode(acpt,INPUT_PULLUP);
  pinMode(rjct,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(acpt), accepts , FALLING);
  attachInterrupt(digitalPinToInterrupt(rjct), rejects , FALLING);
  lcd.begin();
  lcd.backlight();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    lcd.setCursor(0,0);
    lcd.print("Not Connected");
    Serial.print(".");
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

void loop() 
{
  set_as_observer();
  btStart();
  readtemp();
  if(Firebase.setInt(fbdo, "/RSSI", RSSI_Val))
  {
   rssi = -(RSSI_Val);
   Serial.print(rssi);
   }
   else
   {
    Serial.print("Error in setInt, ");
    Serial.println(fbdo.errorReason());
  }
 if(Firebase.setFloat(fbdo, "/TEMP1", farh1))
  {
   
   Serial.print(farh1,2);
   Serial.println("Success");
   }
   else
   {
    Serial.print("Error in setInt, ");
    Serial.println(fbdo.errorReason());
  }
   if(Firebase.getFloat(fbdo, "/TEMP2"))
  {
   farh2=fbdo.floatData();
   Serial.print(farh2);
   Serial.println("Received");
   }
   else
   {
    Serial.print("Error, ");
    Serial.println(fbdo.errorReason());
  }
if(Firebase.getString(fbdo, "/Stat2"))
  {
    String checkg = fbdo.stringData(); 
    Stat = checkg.toInt();
    Serial.println(Stat);
    delay(500);
  }
if (Stat == 1)
  {
   if(Firebase.getString(fbdo, "/dev2"))
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
 Firebase.setString(fbdo, "/ans2" ,"Accpeted");
 Firebase.setString(fbdo, "/Stat2","0");
 a=0;
}
if(b == 1)
{
 Firebase.setString(fbdo, "/ans2","Rejected");
 Firebase.setString(fbdo, "/Stat2","0");
 b=0;
}

}



void set_as_observer(void)
{
    BLEDevice::init("");
    //BLEDevice::setPower(ESP_PWR_LVL_P4);
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);  // less or equal setInterval value
    pBLEScan->start(scanTime, false);
    pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
    delay(100);
    BLEDevice::deinit(0);
}
void safedistance()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Safe dist alert");
  digitalWrite(vbmotor,HIGH);
  lcd.setCursor(0,1);
  lcd.print("M:");
  lcd.print(farh1);
  lcd.setCursor(6,1);
  lcd.print("  O:");
  lcd.print(farh2);
}
void donothing()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Scaning");
  digitalWrite(vbmotor,LOW);
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
  farh1 = mv/10;
  //farh1 = (cel*9)/5 + 32;
  lcd.setCursor(0,1);
  lcd.print("M:");
  lcd.print(farh1);
  delay(10);
}
