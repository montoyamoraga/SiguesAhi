
// started september 2020
// by aaron montoya-moraga


unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

// 1 minute
// const long interval = 1000 * 60;

// 1 hour
const long interval = 1000 * 60 * 60;

// 1 day
// const long interval = 1000 * 60 * 60 * 24;

void setup() {
  Serial.begin(9600);
}

void loop() {

  // update currentMillis to the current time
  currentMillis = millis();

  // check if time passed since
  if (currentMillis - previousMillis >= interval) {

    // update previousMillis to the current time
    previousMillis = currentMillis;


    Serial.println("hello world");
  }
  // check if millis() did a a wrap around
  else if (currentMillis - previousMillis < 0) {

    // // update previousMillis to the current time
    previousMillis = currentMillis;
  }

}
