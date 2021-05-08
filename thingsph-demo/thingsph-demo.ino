/*
  ArduinoMqttClient - WiFi Simple Sender

  This example connects to a MQTT broker and publishes a message to
  a topic once a second.

  The circuit:
  - Arduino MKR 1000, MKR 1010 or Uno WiFi Rev.2 board

  ThingsPH Demo
*/

#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ESP8266_ESP12)
  #include <ESP8266WiFi.h>
#endif

#include "credentials.h"
///////please enter your sensitive data in the Secret tab/credentials.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

//DHT
#include "DHT.h"

#define DHTPIN 4   
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);
//end DHT


// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "mqtt.things.ph";
int        port     = 1883;
const char topic1[]  = "arduino/temperature";
const char topic2[]  = "arduino/humidity";
const char topic3[]  = "arduino/heatindex";

const long interval = 6000; //10 telemetries per minute
unsigned long previousMillis = 0;


float h,t,hic ;// DHT11 Variables



void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  dht.begin();
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
   mqttClient.setId("ArduinoWiFi1010");

  // You can provide a username and password for authentication
  mqttClient.setUsernamePassword("USERNAME", "PASSWORD");

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();
  unsigned long currentMillis = millis();
  readTemp(); //reading temp
  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;
    Serial.print("Sending message to topic: ");
    Serial.println(topic1);
    Serial.println(t);
    Serial.println(topic2);
    Serial.println(h);
    Serial.println(topic3);
    Serial.println(hic);

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic1);
    mqttClient.print(t);
    mqttClient.endMessage();
    
    mqttClient.beginMessage(topic2);
    mqttClient.print(h);
    mqttClient.endMessage();
    
    mqttClient.beginMessage(topic3);
    mqttClient.print(hic);
    mqttClient.endMessage();

    Serial.println();

  }
}

void readTemp(){
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  hic = dht.computeHeatIndex(t, h, false); //heat index

  //Serial.print("Humidity: " + String(h) + ",");
 // Serial.print("Temp: " + String(t) + "°C,");
 // Serial.println("Heat Index: " + String(hic) + "°C");
}
