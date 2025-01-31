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
const char* SSID = "yon";               // name of your wifi network
const char* PASSWORD = "soPlayful";     // password of your wifi network
const char* MQTT_SERVER = "pf-4pbo68mmklw1arbgld3b.cedalo.cloud";
const uint16_t MQTT_SERVER_PORT = 1883; // unencrypted, unauthenticated server
const char* MQTT_USER = "team1";        // name of client, set in platform.cedalo.cloud
const char* MQTT_PASSWORD = "pass";     // password of client, set in platform.cedalo.cloud
const char* MQTT_TOPIC_IN = "command";  // what should we listen for
const char* MQTT_TOPIC_OUT = "data";    // what should we publish


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

  // uncomment to see received messages in serial monitor
  
  Serial.print("Message arrived on topic: '");
  Serial.print(topic);
  Serial.print("' with payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == 'T'){
     digitalWrite(D0, HIGH);
  } else {
    digitalWrite(D0, LOW);
  }
} 

/**
 * @brief connect to the mqtt broker
 */
void connect() {
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String mqtt_client_id = "1";
    if (mqtt_client.connect(mqtt_client_id.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
      mqtt_client.subscribe(MQTT_TOPIC_IN); // subscribe to the topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" will try again in 5 seconds");
      delay(5000);
    }
  }
}

//--------------------------------------
void setup() {
  Serial.begin(115200);   // initialize serial communication
  setup_wifi();           // initialize wifi connection
  mqtt_client.setServer(MQTT_SERVER, MQTT_SERVER_PORT); // initialize mqtt client
  mqtt_client.setCallback(callback);  // set callback function

  // init pins
  pinMode(D0, OUTPUT);
  pinMode(A0, INPUT);
}

//--------------------------------------
void loop() {

  if (!mqtt_client.connected()) {
    // if not connected, connect
    connect();
  }
 
  mqtt_client.loop();
  time_client.update();

  // unconditionaly publish sensor data
  int data = analogRead(A0);
  String data_string = String(data);
  Serial.println(data_string.c_str());
  mqtt_client.publish(MQTT_TOPIC_OUT, data_string.c_str());

  // chill for a bit before next loop
  delay(10);
}
