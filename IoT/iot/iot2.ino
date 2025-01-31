//#include <ESP8266WiFi.h>
//#include <ArduinoJson.h>
//#include <PubSubClient.h>
//
//#pragma once
//
//#include <string>
//
//using namespace std;
//
//#define DATA_PIN A0
//
//const char* ssid     = "yon";
//const char* password = "soPlayful";
//
//const string ID = "0f261a58-e137-4fc6-8a89-7d9c2881f7e0";
//const string BROKER = "test.mosquitto.org";
//const string CLIENT_NAME = ID + "team_name";
//const string CLIENT_DATA_TOPIC = ID + "/data";
//const string SERVER_COMMAND_TOPIC = ID + "/commands";
//
//WiFiClient espClient;
//PubSubClient client(espClient);
//
//void setup() {
//  Serial.begin(115200);
//  delay(10);
//  Serial.println('\n');
//  
//  WiFi.begin(ssid, password);
//  Serial.print("Connecting to ");
//  Serial.print(ssid); Serial.println(" ...");
//
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(1000);
//    Serial.print(". ");
//  }
//
//  Serial.println('\n');
//  Serial.println("Connection established!");  
//  Serial.print("IP address:\t");
//  Serial.println(WiFi.localIP());
//
//  createMQTTClient();
//}
//
//void loop() {
//  reconnectMQTTClient();
//  client.loop();
//
//  int sensor_data = analogRead(DATA_PIN);
//
//  StaticJsonDocument<200> doc;
//  doc["data"] = sensor_data;
//
//  string data_string;
//  serializeJson(doc, data_string);
//
//  Serial.print("Sending telemetry to ");
//  Serial.print(CLIENT_DATA_TOPIC.c_str());
//  Serial.print(": ");
//  Serial.println(data_string.c_str());
//
//  client.publish(CLIENT_DATA_TOPIC.c_str(), data_string.c_str());
//
//  delay(2000);
//}
//
//void reconnectMQTTClient() {
//  while (!client.connected()) {
//    Serial.print("Attempting MQTT connection...");
//
//    if (client.connect(CLIENT_NAME.c_str())) {
//      Serial.println("connected");
//      client.subscribe(SERVER_COMMAND_TOPIC.c_str());
//      Serial.print("Subscribed to topic: ");
//      Serial.println(SERVER_COMMAND_TOPIC.c_str());
//    } else {
//      Serial.print("Retrying in 5 seconds - failed, rc=");
//      Serial.println(client.state());
//      delay(5000);
//    }
//  }
//}
//
//void clientCallback(char *topic, uint8_t *payload, unsigned int length) {
//  char buff[length + 1];
//  for (int i = 0; i < length; i++) {
//      buff[i] = (char)payload[i];
//  }
//  buff[length] = '\0';
//
//  Serial.print("Message received:");
//  Serial.println(buff);
//
//  DynamicJsonDocument doc(1024);
//  deserializeJson(doc, buff);
//  JsonObject obj = doc.as<JsonObject>();
//
//  bool led_on = obj["led_on"];
//
//  if (led_on) digitalWrite(D0, LOW);
//  else digitalWrite(D0, HIGH);
//}
//
//void createMQTTClient() {
//  client.setServer(BROKER.c_str(), 1883);
//  reconnectMQTTClient();
//  client.setCallback(clientCallback);
//}
