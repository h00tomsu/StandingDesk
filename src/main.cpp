#include <Arduino.h>

//OUTPUTS
#define LA1_RELAY_UP  23
#define LA1_RELAY_DOWN 25
#define LA2_RELAY_UP 26
#define LA2_RELAY_DOWN 27
//INPUTS
#define LA1_HALL1 16
#define LA1_HALL2 17
#define LA2_HALL1 18
#define LA2_HALL2 19
#define BUTTON_DOWN 21
#define BUTTON_UP 22
//SETTINGS
#define DEBOUNCE_TIME 50
//STRUCTS
struct button {
  int pin = 0;
  int lastSteadyState = LOW;
  int lastFlickerableState = LOW;
  int currentState = LOW;
  unsigned long lastDebounceTime = 0;
};
//VARIABLES
button btnDown;
button btnUp;

void debounce(button &tmp) {
  tmp.currentState = digitalRead(tmp.pin);
  if (tmp.currentState != tmp.lastFlickerableState) {
    tmp.lastDebounceTime = millis();
    tmp.lastFlickerableState = tmp.currentState;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LA1_RELAY_UP, OUTPUT);
  pinMode(LA1_RELAY_DOWN, OUTPUT);
  pinMode(LA2_RELAY_UP, OUTPUT);
  pinMode(LA2_RELAY_DOWN, OUTPUT);
  pinMode(LA1_HALL1, INPUT);
  pinMode(LA1_HALL2, INPUT);
  pinMode(LA2_HALL1, INPUT);
  pinMode(LA2_HALL2, INPUT);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  btnDown.pin = BUTTON_DOWN;
  btnUp.pin = BUTTON_UP;
}

void loop() {
  // put your main code here, to run repeatedly:
  debounce(btnDown);
  debounce(btnUp);

  //Failsafe, never let relay up and down be active at the same time
  if(( LA1_RELAY_DOWN || LA2_RELAY_DOWN) == HIGH && (LA1_RELAY_UP || LA2_RELAY_UP) == HIGH) {
      digitalWrite(LA1_RELAY_UP, LOW);
      digitalWrite(LA1_RELAY_DOWN, LOW);
      digitalWrite(LA2_RELAY_UP, LOW);
      digitalWrite(LA2_RELAY_DOWN, LOW);
  } 

  if((millis() - btnDown.lastDebounceTime) > DEBOUNCE_TIME && (millis() - btnUp.lastDebounceTime) > DEBOUNCE_TIME) {
    //both buttons stable
    if(btnDown.currentState == LOW && btnUp.currentState == HIGH) {
      //only button down pressed
      digitalWrite(LA1_RELAY_DOWN, HIGH);
      digitalWrite(LA2_RELAY_DOWN, HIGH);
      String stringOne = "BUTTON_DOWN pressed - ";
      String stringThree = stringOne + btnDown.lastDebounceTime;
      Serial.println(stringThree);
    } 
    else if (btnDown.currentState == HIGH && btnUp.currentState == LOW)
    {
      //only button up pressed
      digitalWrite(LA1_RELAY_UP, HIGH);
      digitalWrite(LA2_RELAY_UP, HIGH);
      String stringOne = "BUTTON_UP pressed - ";
      String stringThree = stringOne + btnUp.lastDebounceTime;
      Serial.println(stringThree);
    }

    else if (btnDown.currentState == btnUp.currentState) {
      //Same reading on both buttons, why?
      if(btnDown.currentState == LOW) {
        //Both buttons pressed
        Serial.println("BOTH BUTTONS PRESSED!");
      }
      //No button pressed
      digitalWrite(LA1_RELAY_UP, LOW);
      digitalWrite(LA1_RELAY_DOWN, LOW);
      digitalWrite(LA2_RELAY_UP, LOW);
      digitalWrite(LA2_RELAY_DOWN, LOW);
    }
  }
}