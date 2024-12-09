#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID "---"
#define WLAN_PASS "---"
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "---"
#define AIO_KEY "---"

// setup MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// set publish
Adafruit_MQTT_Publish light_pub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp");

int led_status = HIGH;
float temp;

void setup()
{
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);

  Serial.begin(115200);

  // connect Wifi
  connectWiFi();

  // connect MQTT
  connectMQTT();

  // finish setup , set busy pin LOW
  digitalWrite(5, LOW);
}

void loop()
{
  // Check if data is available from Serial
  if (Serial.available() > 0)
  {
    float tempValue = readSerialData();
    if (tempValue != -1)
    {
      light_pub.publish(tempValue);
    }
  }

  // Keep MQTT connection alive
  mqtt.processPackets(1000); // process MQTT messages in non-blocking way
}

void connectWiFi()
{
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void connectMQTT()
{
  while (!mqtt.connected())
  {
    if (mqtt.connect() == 0)
    {
      Serial.println("MQTT connected");
    }
    else
    {
      Serial.println("MQTT connection failed. Retrying...");
      delay(500);
    }
  }
}

float readSerialData()
{
  String str = Serial.readStringUntil('\n');
  str.trim();
  if (str.length() > 0)
  {
    return str.toFloat();
  }
  return -1; // return -1 if no valid float data
}
