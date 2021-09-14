//Script by ibrohim :)




#define TS_ENABLE_SSL // For HTTPS SSL connection
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "secrets.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int analogInPin = A0; 
int sensorValue = 0; 
unsigned long int avgValue; 
float b;
int buf[10],temp;




X509List cert(TELEGRAM_CERTIFICATE_ROOT);



float h;                  // humidity reading
float t;                  //temperature reading


#define DHTPIN D3          // GPIO Pin where the dht11 is connected
DHT dht(DHTPIN, DHT11);

// Ganti dengan alamat wifi masing-masing
const char* ssid = "MAKERINDO2";
const char* password = "makerindo2019";

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Ganti dengan Token telegram bot maisng-masing, link bot : https://t.me/botfather
#define BOTtoken "1976398512:AAF8A6PRE1zvhbJgvVMY7c9LHETcLbFtSes"  // your Bot Token (Get from Botfather)

// Gunakan @myidbot untuk mencari ID telegram anda
// Link Bot: https://t.me/myidbot
#define CHAT_ID "1287107861"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
String myStatus = "";

void setup() {
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  
dht.begin();

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  

  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
    client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
   client.setInsecure(); // To perform a simple SSL Encryption
ThingSpeak.begin(client);  // Initialize ThingSpeak


  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot started up", "");
}

void loop() {
 for(int i=0;i<10;i++) 
 { 
  buf[i]=analogRead(analogInPin);
  delay(10);
 }
 for(int i=0;i<9;i++)
 {
  for(int j=i+1;j<10;j++)
  {
   if(buf[i]>buf[j])
   {
    temp=buf[i];
    buf[i]=buf[j];
    buf[j]=temp;
   }
  }
 }
 avgValue=0;
 for(int i=2;i<8;i++)
 avgValue+=buf[i];
 float pHVol=(float)avgValue*5.0/1024/6;
 float phValue = -5.70 * pHVol + 21.34;
 Serial.print("sensor pH: ");
 Serial.println(phValue);
 delay(200);


 unsigned long currentMillis = millis(); // grab current time

  h = dht.readHumidity();     // menampung nilai kelembaban
  t = dht.readTemperature();     // menampung nilai suhu

  if (isnan(h) || isnan(t))
  {
    Serial.println("gagal menghubungkan ke DHT11");
    return;
  }
bot.sendMessage(CHAT_ID, "pH: "+String(phValue)+" | Suhu: "+String(t)+" | Kelembaban: "+String(h), "");
  Serial.println("Kelembaban: ");  
  Serial.println(String(h));
  Serial.println("Suhu: ");
  Serial.println(String(t));
delay(500);

  display.clearDisplay();

  display.setTextSize(1.5);
  display.setCursor(30, 5);
  // Display static text
  display.setTextColor(BLACK, WHITE); 
  display.println("Alat ukur pH");
  display.println();
  display.setTextColor(WHITE);
  display.println("pH: "+String(phValue));
  
  display.println("Suhu: "+String(t));
  
  display.println("Kelembaban: "+String(h));
  display.display(); 

 // set the fields with the values
  ThingSpeak.setField(1, String(t));
  ThingSpeak.setField(2, String(h));
  ThingSpeak.setField(3, String(phValue));
 
  // set the status
  ThingSpeak.setStatus(myStatus);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel berhasil di update.");
  }
  else{
    Serial.println("Terjadi masalah. Kode error HTTP " + String(x));
  }
  

  delay(2000); // Wait 20 seconds to update the channel again

}
