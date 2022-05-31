#include <Wire.h>

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "project"
#define WLAN_PASS       "123456789"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Suswambica_111"
#define AIO_KEY         "aio_ivSq89ofBZarodIyjPclur6jUoFH"

/************ Global State (you don't need to change this!) ******************/

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);



Adafruit_MQTT_Publish level = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/level");
Adafruit_MQTT_Publish stat = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/status");




int period1   = 1000;
unsigned long time_now1 = 0;


int period2   = 3000;
unsigned long time_now2 = 0;

int l = 0;
int d = 0 ;
bool f1 = LOW, f2 = LOW;
bool f11 = LOW, f22 = LOW;
#define buz D3
void setup() {
  Serial.begin(9600); /* begin serial for debug */
    while (!Serial) {
    ;
  }
  pinMode(buz , OUTPUT);
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  digitalWrite(buz, LOW);
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
}

void loop() {
  String data = "";
  MQTT_connect();
  if (millis() >= time_now1 + period1) {
    time_now1 += period1;
    Wire.requestFrom(8, 22); /* request & read data of size 13 from slave */
    if (Wire.available()) {
      data = Wire.readString();
    }
    if (data.indexOf("level:") != -1) {
      l = data.substring(data.indexOf("level:") + 6, data.indexOf(',')).toInt();
      d = data.substring(data.indexOf("distance:") + 9, data.indexOf(';')).toInt();
      Serial.println("level : " + String(l) + " Distance : " + String(d));
      // feeds[i].publish((char*)Red.c_str());
      if (l != 2)f1 = HIGH;
      else{  
      if (f1) {
        stat.publish("Gates are opening");
        f1 = LOW;
        digitalWrite(buz, HIGH);
        delay(500);
        digitalWrite(buz, LOW);
      }
      }
          if (d > 10){
        f2 = HIGH;
      }
      else{
      if (f2) {
        stat.publish("DAM Crack detected");
        f2 = LOW;
        digitalWrite(buz, HIGH);
        delay(500);
        digitalWrite(buz, LOW);
      }
      }
    }
  }

  if (millis() >= time_now2 + period2) {
    time_now2 += period2;
    level.publish(l);
  }


}

void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
