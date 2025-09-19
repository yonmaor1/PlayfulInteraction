#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "Mojo Dojo Casa House";
const char* password = "password";

// Set the GPIO pin to control an LED
const int ledPin = 0; //set to whatever pin you need

// Create a web server object that listens for HTTP requests on port 80
WebServer server(80);

// Variable to hold the desired state of the LED (false = OFF, true = ON)
bool ledState = false;

// Function to handle the root URL ("/") and display the web page
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 LED Control</title>
    <style>
        body {
            font-family: 'Inter', sans-serif;
            background-color: #f3f4f6;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }
        .container {
            background-color: #ffffff;
            border-radius: 12px;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
            padding: 2rem;
            text-align: center;
            max-width: 400px;
            width: 90%;
        }
        h1 {
            color: #1f2937;
            font-size: 1.5rem;
            margin-bottom: 1.5rem;
        }
        .button-group {
            display: flex;
            flex-direction: column;
            gap: 1rem;
        }
        .btn {
            padding: 1rem;
            border-radius: 8px;
            font-size: 1.1rem;
            font-weight: bold;
            text-decoration: none;
            color: white;
            transition: background-color 0.3s ease;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }
        .on-btn {
            background-color: #10b981;
        }
        .on-btn:hover {
            background-color: #059669;
        }
        .off-btn {
            background-color: #ef4444;
        }
        .off-btn:hover {
            background-color: #dc2626;
        }
        .ip-info {
            margin-top: 1.5rem;
            color: #4b5563;
            font-size: 0.875rem;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 LED Controller</h1>
        <div class="button-group">
            <a href="/on" class="btn on-btn">Turn LED On</a>
            <a href="/off" class="btn off-btn">Turn LED Off</a>
        </div>
        <p class="ip-info">The IP address of this server will be printed to your Arduino IDE's Serial Monitor.</p>
    </div>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

// Function to handle turning the LED on
void handleOn() {
  ledState = true; // Just update the state variable
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
  Serial.println("On");
}

// Function to handle turning the LED off
void handleOff() {
  ledState = false; // Just update the state variable
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
  Serial.println("Off");
}

void setup() {
  // Start the serial communication for debugging
  Serial.begin(115200);

  // Set the LED pin as an output and set initial state
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Connect to Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Define the server routes
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
  delay(10); // A small delay to prevent resource hogging
}
