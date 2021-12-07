#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"

DHTesp dht;

const char* ssid = "Mayodon";
const char* password = "5400coolbrook";

const char* mqtt_server = "test.mosquitto.org";

WiFiClient tempClient;
PubSubClient client(tempClient);

//DHT pins and temperature threshold
const int DHTPin = 12;
const float temp_threshold = 26;

//DC motor pins
const int enable = 4;
const int input1 = 5;
const int input2 = 16;

//const int lamp = 5;

void setup_wifi() {
  delay(10);
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("tempClient")) {
      Serial.println("connected");
      client.subscribe("IoTlab/led");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {

  //DHT11 setup
  dht.setup(DHTPin, DHTesp::DHT11);

  //DC motor setup
  pinMode(enable, OUTPUT);
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("tempClient");

  float temp = dht.getTemperature();
  float hum = dht.getHumidity();
    
  char tempArr [8];
  dtostrf(temp,6,2,tempArr);
  char humArr [8];
  dtostrf(hum,6,2,humArr);

  char threshArr[8];
  dtostrf(temp_threshold,6,2,threshArr);

  client.publish("IoTlab/temperature", tempArr);
  client.publish("IoTlab/humidity", humArr);
  client.publish("IoTlab/threshold", threshArr);

  if (temp >= temp_threshold) {
      digitalWrite(enable, HIGH);
      digitalWrite(input1, LOW);
      digitalWrite(input2, HIGH);
    }

  delay(1000);
} 
