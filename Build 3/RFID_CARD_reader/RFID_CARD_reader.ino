#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>

const char* ssid = "TP-Link_2AD8";
const char* password = "14730078";

const char* mqtt_server = "192.168.0.150";
#define D3 0
#define D4 2
constexpr uint8_t RST_PIN = D3;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;     // Configurable, see typical pin layout above

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;


WiFiClient vanieriot;
PubSubClient client(vanieriot);


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
  Serial.print("WiFi connected - ESP-8266 IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messagein;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messagein += (char)message[i];
  }
  
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
 
    if (client.connect("vanieriot")) {

      Serial.println("connected");  
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

String tag;
String user;
String temp_threshold;
String light_threshold;

void setup() {
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  pinMode(D8, OUTPUT);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("vanieriot");

  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    Serial.println(tag);

    if (tag == "10814135109") {
      Serial.println("Access Granted!");
      buzz_right();
      user = "Allie";
      temp_threshold = "25.5";
      light_threshold = "400";
      login();
    } else if (tag == "169225254228") {
      Serial.println("Access Granted!");
      buzz_right();
      user = "Sam";
      temp_threshold = "23";
      light_threshold = "200";
      login();
    } else {
      Serial.println("Access Denied!");
      buzz_wrong();
      user = "---";
      tag = "---";
      temp_threshold = "---";
      light_threshold = "---";
    }

    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

}

void buzz_right() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(D8, HIGH);
    delay(100);
    digitalWrite(D8, LOW);
    delay(100);
  }
}

void buzz_wrong() {
  digitalWrite(D8, HIGH);
  delay(2000);
  digitalWrite(D8, LOW);
}

void login() {
    int tag_len = tag.length() + 1;
    int name_len = user.length() + 1;
    int temp_len = temp_threshold.length() + 1;
    int light_len = light_threshold.length() + 1;
    
    char tag_array[tag_len];
    char namearray[name_len];
    char temp_array[temp_len];
    char light_array[light_len];
    
    tag.toCharArray(tag_array, tag_len);
    user.toCharArray(namearray, name_len);
    temp_threshold.toCharArray(temp_array, temp_len);
    light_threshold.toCharArray(light_array, light_len);
    
    client.publish("IoTlab/Name", namearray);
    client.publish("IoTlab/Tag", tag_array);
    client.publish("IoTlab/Temp", temp_array);
    client.publish("IoTlab/Light", light_array);
}
