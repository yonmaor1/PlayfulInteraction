const int LED_PIN = 2; //Built in LED

void setup() {
  Serial.begin(115200);  // Initialize serial communication at a baud rate of 115200.

  pinMode(LED_PIN, OUTPUT); // Set the LED pin as an OUTPUT.

  // Initial rapid blinks
  Serial.println("Check, Check");
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, LOW);   // Turn the LED on (LOW is usually ON for ESP8266 built-in LEDs)
    delay(100);                   // Wait for 100 milliseconds
    digitalWrite(LED_PIN, HIGH);
    delay(100);
  }
  Serial.println("One, Two, One Two");
}

void loop() {
  
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  
  digitalWrite(LED_PIN, HIGH); 
  delay(1000);

  Serial.println("Is this thing on?!?!?");
}
