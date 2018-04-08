#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WIFI
const char* WIFI_SSID = "FRANK_2G";
const char* WIFI_PASSWORD =  "";

// MQTT
const char* MQTT_SERVER = "rabbitmq.jirifrank.eu";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "iot";
const char* MQTT_PASSWORD = "";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
StaticJsonBuffer<200> jsonBuffer;



void mqttReconnect(){
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");

    if (mqttClient.connect(MQTT_USER, MQTT_USER, MQTT_PASSWORD )) {
      Serial.println("Connected.");
    } else {
      Serial.print("Failed with state: ");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }

  mqttClient.subscribe("springler.actions");
}


void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println("-----------------------");
}


void setup()
{
  Serial.begin(9600);
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Establishing connection to MQTT server.");
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(callback);
  mqttReconnect();
}

void loop() {
  if (!mqttClient.connected()) {
    mqttReconnect();
  }
  mqttClient.loop();

  // Create the root object

JsonObject& root = jsonBuffer.createObject();
   root["sensorType"] = "TEMPERATURE";
   root["value"] = "10.0";
   root["location"] = "OPENED";


   char jsonChar[100];
   root.printTo(jsonChar);

   mqttClient.publish("springler/measurings", jsonChar);

}
