#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h> // tested with 2.8.0 on 1/31/25
#include <NTPClient.h> // tested with 3.2.1 on 1/31/25
#include <WiFiUdp.h>

#define CERT mqtt_broker_cert
#define MSG_BUFFER_SIZE (50)

//--------------------------------------
// config (edit here before compiling)
//--------------------------------------
//#define MQTT_TLS // uncomment this define to enable TLS transport
//#define MQTT_TLS_VERIFY // uncomment this define to enable broker certificate verification
const char* SSID = "yon";
const char* PASSWORD = "soPlayful";
const char* MQTT_SERVER = "pf-4pbo68mmklw1arbgld3b.cedalo.cloud"; // eg. your-demo.cedalo.cloud or 192.168.1.11
const uint16_t MQTT_SERVER_PORT = 1883; // or 8883 most common for tls transport
const char* MQTT_USER = "team1";
const char* MQTT_PASSWORD = "pass";
const char* MQTT_TOPIC_IN = "command";
const char* MQTT_TOPIC_OUT = "data";

//--------------------------------------
// globals
//--------------------------------------
#ifdef MQTT_TLS
  WiFiClientSecure wifi_client;
#else
  WiFiClient wifi_client;
#endif
WiFiUDP ntpUDP;
NTPClient time_client(ntpUDP);
PubSubClient mqtt_client(wifi_client);

//--------------------------------------
// function setup_wifi called once
//--------------------------------------
void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  time_client.begin();

#ifdef MQTT_TLS
  #ifdef MQTT_TLS_VERIFY
    X509List *cert = new X509List(CERT);
    wifi_client.setTrustAnchors(cert);
  #else
    wifi_client.setInsecure();
  #endif
#endif

  Serial.println("WiFi connected");
}

//--------------------------------------
// function callback called everytime 
// if a mqtt message arrives from the broker
//--------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {

  // uncomment to see received messages
  /*
  Serial.print("Message arrived on topic: '");
  Serial.print(topic);
  Serial.print("' with payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  */

  digitalWrite(D0, HIGH);
}

//--------------------------------------
// function connect called to (re)connect
// to the broker
//--------------------------------------
void connect() {
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String mqtt_client_id = "1";
    if (mqtt_client.connect(mqtt_client_id.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
      mqtt_client.subscribe(MQTT_TOPIC_IN);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" will try again in 5 seconds");
      delay(5000);
    }
  }
}

//--------------------------------------
// main arduino setup fuction called once
//--------------------------------------
void setup() {
  Serial.begin(115200);
  setup_wifi();
  mqtt_client.setServer(MQTT_SERVER, MQTT_SERVER_PORT);
  mqtt_client.setCallback(callback);
}

//--------------------------------------
// main arduino loop fuction called periodically
//--------------------------------------
void loop() {
   digitalWrite(D2, HIGH);
  
  if (!mqtt_client.connected()) {
    connect();
  }
 
  mqtt_client.loop();
  time_client.update();

  int data = analogRead(A0);
  String data_string = String(data);
  Serial.println(data_string.c_str());
  mqtt_client.publish(MQTT_TOPIC_OUT, data_string.c_str());

  delay(10);
}
