#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "CSIS Security Van #2";
const char* password = "jigx5340+";

const char* mqtt_server = "192.168.43.54";

WiFiClient ledClient;
PubSubClient client(ledClient);

//LED pins and photoresistor
const int led1 = 15;
const int led2 = 13;
const int led3 = 16;
const int ldr = A0;

const float ldr_threshold = 930;

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

  if(topic == "IoTlab/led"){
      Serial.print("Changing LED to ");
      if(messageTemp == "ON"){
        digitalWrite(led3, HIGH);
        Serial.print("On");
      }
      else if(messageTemp == "OFF"){
        digitalWrite(led3, LOW);
        Serial.print("Off");
      }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ledClient")) {
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
  
  //LEDs and photoresistor setup
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(ldr, INPUT);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  
  if(!client.loop()) {
    client.connect("ledClient");
  }

  int ldrstatus = analogRead(ldr);
  
  if (ldrstatus <= ldr_threshold) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    Serial.print(ldrstatus);
    Serial.println("LED is OFF");
  } else {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    Serial.print(ldrstatus);
    Serial.println("LED is ON");
    delay(1000);
  }

  char ldr_thresholdArr[8];
  char ldrArr[8];
  
  dtostrf(ldr_threshold,6,2,ldr_thresholdArr);
  dtostrf(ldrstatus,6,2,ldrArr);
  
  client.publish("IoTlab/photoresistor", ldr_thresholdArr);
  client.publish("IoTlab/photovalue", ldrArr);

  delay(1000);
} 
