#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//#include "DHT.h"
#include "DHTesp.h"

DHTesp dht;
//#define DHTTYPE DHT11   // DHT 11

const char* ssid = "TP-Link_8856";
const char* password = "87973365";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "192.168.0.150";

WiFiClient espClient;
PubSubClient client(espClient);

// DHT Sensor - GPIO 5 = D1 on ESP-12E NodeMCU board
const int DHTPin = 12;

const int enable = 4;
const int input1 = 5;
const int input2 = 16;

float threshold = 26;
char temp_thresholdArr[150];

// Initialize DHT sensor.
//DHT dht(DHTPin, DHTTYPE);

// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
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

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your ESP8266 is subscribed you can actually do something
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

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect

    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("IoTlab/led");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs
void setup() {
  pinMode(enable, OUTPUT);
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
  
  //dht.begin();
  dht.setup(DHTPin, DHTesp::DHT11);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

// For this project, you don't need to change anything in the loop function. Basically it ensures that you ESP is connected to your broker
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");

  float temp = dht.getTemperature();
  float hum = dht.getHumidity();
    
    char tempArr [8];
    dtostrf(temp,6,2,tempArr);
    char humArr [8];
    dtostrf(hum,6,2,humArr);

    dtostrf(threshold,6,2,temp_thresholdArr);
      
    client.publish("IoTlab/temperature", tempArr);
    client.publish("IoTlab/humidity", humArr);
    client.publish("IoTlab/temp_threshold", temp_thresholdArr);

    if (temp >= threshold) {
      digitalWrite(enable, HIGH);
      digitalWrite(input1, LOW);
      digitalWrite(input2, HIGH);
    }

  delay(1000);
} 
