#include "DHT.h"
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <UrlEncode.h>

#define DHTPIN A4
#define DHTTYPE DHT11

#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6TCbLCn29"
#define BLYNK_TEMPLATE_NAME "Monitoring Deteksi Kebakaran"
#define BLYNK_AUTH_TOKEN "75zgOlhZ1jBIBbcIy35f-V6nBI1g5T61"

BlynkTimer timer;

const char *ssid =  "Riyadi";
const char *pass =  "akunangis"; 
String url;

const int RELAYPIN = 15;
const int BUZZERPIN = 4;
int GASPIN = A6;
int RELAYSTATE;

String phoneNumber = "+6281230509131";
String apiKey = "1207394";

void sendMessage(String message)
{
  // Data to send with HTTP POST
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);
  HTTPClient http;
  http.begin(url);
  
  // Send HTTP POST request
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200){
    Serial.println("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
}

void myTimerEvent()
{
    DHT dht(DHTPIN,DHTTYPE);
    dht.begin();
    
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);

    if (isnan(h) || isnan(t) || isnan(f)){
      return;
    }
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%, Temperature: "));
    Serial.print(t);
    Serial.println(F("°C"));

    int sensorValue = analogRead(GASPIN);
    Serial.print("Gas Quality ");
    Serial.print(sensorValue);
    Serial.println(" PPM");
    if(sensorValue>=1500){
      digitalWrite(BUZZERPIN, HIGH);
      delay(500);
      digitalWrite(BUZZERPIN, LOW);
      delay(100);
    }else{
      digitalWrite(BUZZERPIN, LOW);
    }

    if(sensorValue>=1500){
      digitalWrite(RELAYPIN, HIGH);
      RELAYSTATE=1;
      sendMessage("Bahaya!!!");
    }else{
      digitalWrite(RELAYPIN, LOW);
      RELAYSTATE=0;
    }
    
  Blynk.virtualWrite(V0, RELAYSTATE);
  Blynk.virtualWrite(V1, sensorValue);
  Blynk.virtualWrite(V2, t);
  Blynk.virtualWrite(V3, h);
}

WiFiClient espClient;

void setup() {
    Serial.begin(115200);
    pinMode(DHTPIN, INPUT);
    pinMode(GASPIN, INPUT);
    pinMode(RELAYPIN, OUTPUT);
    pinMode(BUZZERPIN, OUTPUT);

    Serial.println("Connecting to ");
    Serial.println(ssid);
    Serial.print("Menghubungkan ke ");
    Serial.println(ssid);

    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    delay(1000);

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    timer.setInterval(1000L, myTimerEvent);
}

void loop() {
  Blynk.run();
  timer.run();
} 
