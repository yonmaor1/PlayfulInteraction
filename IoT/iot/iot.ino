#include <ESP8266WiFi.h>        // Include the Wi-Fi library

// WiFi Settings
const char* ssid     = "SSID";
const char* password = "PASSWORD";     // The password of the Wi-Fi network

// MQTT settings
const string ID = "<ID>"; // Must be unique! Use https://www.guidgen.com/

const string BROKER = "test.mosquitto.org";
const string CLIENT_NAME = ID + "nightlight_client";


void setup() {
  // Start the Serial communication to send messages to the computer
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');
  
  // Connect to the network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  // Wait for the Wi-Fi to connect
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("."); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer

  createMQTTClient();
}

void loop() {
  reconnectMQTTClient();
  client.loop();

  delay(2000);
}


void reconnectMQTTClient() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(CLIENT_NAME.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("Retying in 5 seconds - failed, rc=");
      Serial.println(client.state());
      
      delay(5000);
    }
  }
}

void createMQTTClient() {
  client.setServer(BROKER.c_str(), 1883);
  reconnectMQTTClient();
}
