#include <DS3231.h>
#include <Wire.h>
#include <Servo.h>

// constants won't change. Used here to set a pin number:
const int ledPin =  LED_BUILTIN;// the number of the LED pin

Servo myServo;  // create a servo object
int angle;   // variable to hold the angle for the servo motor
bool switchState;
bool isOpen;
const int openAngle = 165;
const int closedAngle = 0;
const int switchDelay = 3000;

// Variables will change:
int ledState = LOW;             // ledState used to set the LED

// constants won't change:
const long blinkInterval = 1000;
const long printInterval = 10000;

bool Century = true;
bool h12 = true;
bool pm_time = true;

unsigned long previousMillisLed = 0;
unsigned long previousMillisPrint = 0;
unsigned long previousButtonPush = 0;

DS3231 Clock;

void setup() {
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
    // declare the switch pin as an input
  pinMode(2, INPUT);

  Wire.begin();
  Serial.begin(9600);
  Serial.print("Starting up at time: ");
  printTime();

  myServo.attach(3); // attaches the servo on pin 9 to the servo object
}

void printTime() {
  Serial.print(Clock.getYear(), DEC);
  Serial.print("-");
  Serial.print(Clock.getMonth(Century), DEC);
  Serial.print("-");
  Serial.print(Clock.getDate(), DEC);
  Serial.print(" ");
  Serial.print(Clock.getHour(h12, pm_time), DEC); //24-hr
  Serial.print(":");
  Serial.print(Clock.getMinute(), DEC);
  Serial.print(":");
  Serial.println(Clock.getSecond(), DEC);
}

void executeSleep(void func (void), unsigned long& previousMillis, const long& interval) {
  // here is where you'd put code that needs to be running all the time.

  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis < interval) {
    return;
  }
  
  // save the last time you executed
  previousMillis = currentMillis;
  func();
}

void blink() {

  // if the LED is off turn it on and vice-versa:
  if (ledState == LOW) {
    ledState = HIGH;
  } else {
    ledState = LOW;
  }

  // set the LED with the ledState of the variable:
  digitalWrite(ledPin, ledState);
}

void switchServo() {
  if (isOpen) {
    Serial.println("Opening server");
    myServo.write(openAngle);
  }
  else {
    Serial.println("Closing server");
    myServo.write(closedAngle);
  }
  isOpen = !isOpen;
}

void loop() {
  switchState = digitalRead(2);

  if (switchState) {
    executeSleep(switchServo, previousButtonPush, switchDelay);
  }

  // executeSleep(blink, previousMillisLed, blinkInterval);
  executeSleep(printTime, previousMillisPrint, printInterval);
}
