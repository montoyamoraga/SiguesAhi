
// started september 2020
// by aaron montoya-moraga


unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

// 1 minute
const long interval = 1000 * 60;

// 1 hour
// const long interval = 1000 * 60 * 60;

// 1 day
// const long interval = 1000 * 60 * 60 * 24;

void setup() {
  Serial.begin(9600);

}

void loop() {

  currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Serial.println("hello world");
  }

}
