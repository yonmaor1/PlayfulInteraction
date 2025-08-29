const int LED_PIN = 14; 
const int POT_PIN = A0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT); 
  pinMode(POT_PIN, INPUT);
  Serial.println("Is this thing on?");
}

void loop() {

  int read = analogRead(POT_PIN);
  Serial.println(read);

  if (read > 500) { //Change this logic to something you find interesting. Maybe it flashes or has a different on-off mapping
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

}
