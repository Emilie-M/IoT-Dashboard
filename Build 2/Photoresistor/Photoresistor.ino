#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <ESP_Mail_Client.h>

#include <PubSubClient.h>
#include "DHTesp.h"

#define WIFI_SSID "TP-Link_8856" //CHANGE
#define WIFI_PASSWORD "87973365" //CHANGE

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

/* The sign in credentials */
#define AUTHOR_EMAIL "emmymayodon@gmail.com" //CHANGE
#define AUTHOR_PASSWORD "YmirRossQueen123" //CHANGE

/* Recipient's email*/
#define RECIPIENT_EMAIL "hakimbouabdellah02@gmail.com" //CHANGE

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

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

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

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
    sendEmail();
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
    sendEmail();
    delay(10000);
  }
  
  dtostrf(ldr_threshold,6,2,ldr_thresholdArr);
  client.publish("IoTlab/photoresistor", ldr_thresholdArr);

}

void sendEmail() {
  /** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1
  */
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  /* Declare the message class */
  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = "NodeMCU";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "LED Status";
  message.addRecipient("Admin", RECIPIENT_EMAIL);

  /*Send HTML message*/
  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>LED is ON</h1><p>- Sent from ESP board</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
  
}

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");
  }
}
