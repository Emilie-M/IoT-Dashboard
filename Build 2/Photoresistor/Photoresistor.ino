#include <Arduino.h>
#include <WiFi.h>
#include <ESP8266WiFi.h>

#include <PubSubClient.h>
#include "DHTesp.h"

#define WIFI_SSID "TP-Link_2AD8" //CHANGE
#define WIFI_PASSWORD "14730078" //CHANGE

DHTesp dht;
const int DHTPin = 12;

//dc motor
const int enable = 4;
const int input1 = 5;
const int input2 = 16;

float temp_threshold = 26;

//led and photoresistor
const int ledPin1 = 15;
const int ledPin2 = 13;
const int ldrPin = A0;
float ldr_threshold = 300;
char ldr_thresholdArr[150];

const char* mqtt_server = "192.168.0.150";

WiFiClient espClient;
PubSubClient client(espClient);

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


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  pinMode(enable, OUTPUT);
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
  
  dht.setup(DHTPin, DHTesp::DHT11);

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  
  pinMode(ldrPin, INPUT);
 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  float temp = dht.getTemperature();
  float hum = dht.getHumidity();
    
  char tempArr [8];
  dtostrf(temp,6,2,tempArr);
  char humArr [8];
  dtostrf(hum,6,2,humArr);
      
  client.publish("IoTlab/temperature", tempArr);
  client.publish("IoTlab/humidity", humArr);

  if (temp >= temp_threshold) {
    digitalWrite(enable, HIGH);
    digitalWrite(input1, LOW);
    digitalWrite(input2, HIGH);
  }
  
  int ldrstatus = analogRead(ldrPin);
  
  if (ldrstatus <= ldr_threshold) {
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
    Serial.print(ldrstatus);
    Serial.println("LED is OFF");
  } else {
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
    Serial.print(ldrstatus);
    Serial.println("LED is ON");
    delay(10000);
  }
  
  dtostrf(ldr_threshold,6,2,ldr_thresholdArr);
  client.publish("IoTlab/photoresistor", ldr_thresholdArr);

}
