#include "mqtt.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "motor.h"
#include "imu.h"
#include "transform.h"

const char* ssid = "Rang Dong Lab_2G";
const char* password = "ktdt123456";

// const char* mqttServer = "Mqtt.mysignage.vn";
// const int mqttPort = 1883;
// const char* mqttUser = "admin"; // Leave empty if no authentication
// const char* mqttPassword = "admin"; // Leave empty if no authentication

const char* mqttServer = "192.168.31.41";
const int mqttPort = 1883;
const char* mqttUser = ""; // Leave empty if no authentication
const char* mqttPassword = ""; // Leave empty if no authentication

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  Serial.println();

  float value = messageTemp.toFloat(); 

  if (strcmp(topic, "agv/speed") == 0){
    Motor_Translation_Set(value, 0);
  }
  else if(strcmp(topic, "agv/angle") == 0){
    Motor_Translation_Set(0, value);
  }
  else if(strcmp(topic, "agv/translation") == 0){
    Translation_Set(400, value);
  }
  else if(strcmp(topic, "agv/rotation") == 0){
    Rotation_Set(0.34, value);
  }
}

void Mqtt_Connection(void){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Configure MQTT client
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  // Connect to MQTT broker
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client123456", mqttUser, mqttPassword)) {
      Serial.println("connected");

      // Subscribe to a topic
      client.subscribe("agv/#");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void Mqtt_Reconnect(void){
  while (!client.connected()) {
      client.connect("ESP32Client123456", mqttUser, mqttPassword);
  }
}

void Mqtt_Listen(void){
  client.loop();
}

void Mqtt_Publish(const char* topic, const char* payload){
  client.publish(topic, payload);
}

