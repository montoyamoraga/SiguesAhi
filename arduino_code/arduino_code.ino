
// started september 2020
// by aaron montoya-moraga

// answerType 0 for beep, 1 for print, 2 for servo
int answerType = 0;

// answerType 0 for 1 minute, 1 for 1 hour, 2 for 1 day
int intervalType = 0;

// variables for time
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
unsigned long interval = 0;

void setup() {
  Serial.begin(9600);

  if (intervalType == 0) {
    interval = 1000 * 60;
  } else if (intervalType == 1) {
    interval = 1000 * 60 * 60;
    
  } else if (intervalType == 2) {
    interval = 1000 * 60 * 60 * 24; 
  }
  
}

void loop() {

  // update currentMillis to the current time
  currentMillis = millis();

  // check if time passed since
  if (currentMillis - previousMillis >= interval) {

    // update previousMillis to the current time
    previousMillis = currentMillis;


    Serial.println("hello world");

    if (answerType == 0) {
      answerBeep();
    } else if (answerType == 1) {
      answerPrint();
    } else if (answerType == 2) {
      answerServo();
    }

  }
  // check if millis() did a a wrap around
  else if (currentMillis - previousMillis < 0) {

    // update previousMillis to the current time
    previousMillis = currentMillis;
  }

}


void answerBeep() {

}


void answerPrint() {

}

void answerServo() {

}
