#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>   // tested with 2.8.0 on 1/31/25
#include <NTPClient.h>      // tested with 3.2.1 on 1/31/25
#include <WiFiUdp.h>

#define CERT mqtt_broker_cert
#define MSG_BUFFER_SIZE (50)

// config (edit here before compiling)
//--------------------------------------

//#define MQTT_TLS // uncomment this define to enable TLS transport
//#define MQTT_TLS_VERIFY // uncomment this define to enable broker certificate verification
const char* SSID = "yon"; // name of your wifi network
const char* PASSWORD = "soPlayful"; // password of your wifi network
const char* MQTT_SERVER = "pf-4pbo68mmklw1arbgld3b.cedalo.cloud";
const uint16_t MQTT_SERVER_PORT = 1883; // unencrypted, unauthenticated server
const char* MQTT_USER = ""; // name of client, set in platform.cedalo.cloud
const char* MQTT_PASSWORD = ""; // password of client, set in platform.cedalo.cloud
const char* MQTT_TOPIC_IN = ""; // what should we listen for
const char* MQTT_TOPIC_OUT = ""; // what should we publish


/* globals */
//--------------------------------------

#ifdef MQTT_TLS
  WiFiClientSecure wifi_client;
#else
  WiFiClient wifi_client;
#endif
WiFiUDP ntpUDP;
NTPClient time_client(ntpUDP);
PubSubClient mqtt_client(wifi_client);

/**
 * @brief initialize wifi connection
 */
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

/**
 * @brief callback function called everytime a message is received from the broker
 * 
 * @param topic the topic the message was received on
 * @param payload the message payload
 * @param length the length of the payload
 */
void callback(char* topic, byte* payload, unsigned int length) {

}

/**
 * @brief connect to the mqtt broker
 */
void connect() {
  
}

//--------------------------------------
void setup() {
  
  setup_wifi();          

  Serial.begin(115200);   // initialize serial communication
  mqtt_client.setCallback(callback);  // set callback function

  // init pins
  pinMode(D0, OUTPUT);
  pinMode(A0, INPUT);
}

//--------------------------------------
void loop() {


  // chill for a bit before next loop
  delay(10);
}
