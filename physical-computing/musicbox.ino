int map(int val, int ilo, int ihi, int olo, int ohi) {
  return olo + (ohi - olo) * ((val - ilo) / (ihi - ilo));
}

void setup() {
  pinMode(A1, INPUT);
  pinMode(A0, OUTPUT);
}

int volume;
void loop() {

  volume = analogRead(A1);

  Serial.println(volume);

  tone(A0, volume, 10);

  // noTone(3);

}
