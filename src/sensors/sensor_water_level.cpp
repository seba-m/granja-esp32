int Liquid_level = 0;
void setup() {
  Serial.begin(9600);
  pinMode(5, INPUT);
}

void loop() {
  Liquid_level = digitalRead(5);
  Serial.print("Liquid_level= "); Serial.println(Liquid_level, DEC);
  delay(500);
}