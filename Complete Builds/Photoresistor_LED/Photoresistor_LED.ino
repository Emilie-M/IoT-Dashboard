#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Mayodon";
const char* password = "5400coolbrook";

const char* mqtt_server = "test.mosquitto.org";

WiFiClient ledClient;
PubSubClient client(ledClient);

//LED pins and photoresistor
const int led1 = 15;
const int led2 = 13;
const int ldr = A0;

const float ldr_threshold = 300;

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
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
        Serial.print("On");
      }
      else if(messageTemp == "OFF"){
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
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
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ldrPin, INPUT);
  
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

  char ldr_thresholdArr[8];
  
  dtostrf(ldr_threshold,6,2,ldr_thresholdArr);
  client.publish("IoTlab/photoresistor", ldr_thresholdArr);

  delay(1000);
} 
