#include <DS1307RTC.h>
#include <Dusk2Dawn.h>
#include <TimeLib.h>
#include <Wire.h>
#include <Servo.h>

const int ledPin =  LED_BUILTIN;// the number of the LED pi
bool ledState = false;             // ledState used to set the LED
const int BUTTON_PIN = 2;
const int SERVO_PIN = 3;

// servo initi
Servo myServo;  // create a servo object
int angle;   // variable to hold the angle for the servo motor
bool switchState;
bool isOpen;
const int openAngle = 165;
const int closedAngle = 0;
const int switchDelay = 3000;

// timing intervals
const long blinkInterval = 1000;
const long printInterval = 50000;

// timing constants
bool Century = true;
bool h12 = true;
bool pm_time = true;

// keep track of last time things happened (for time delay)
unsigned long previousMillisLed = 0;
unsigned long previousMillisPrint = 0;
unsigned long previousButtonPush = 0;

// clock stuff
tmElements_t tm;
Dusk2Dawn sunTracker(37.4852, -122.2364, -8);
int sunrise;
int sunset;
const bool dst = true;  // make sure if you re-sync the RTC outside of DST then you set this to false

// track changes in day light
bool isDay;
bool wasDay;

void setup() {
  // set pins
  pinMode(ledPin, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  myServo.attach(SERVO_PIN); // attaches the servo on pin 9 to the servo object

  // initialize stuff
  Wire.begin();
  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);

  // print start up time
  Serial.print("Starting up at time: ");
  printTime();

  // get sunrise and sunset
  RTC.read(tm);
  sunset = sunTracker.sunset(tmYearToCalendar(tm.Year), tm.Month, tm.Day, dst);
  sunrise = sunTracker.sunrise(tmYearToCalendar(tm.Year), tm.Month, tm.Day, dst);
  isDay = isDayNow();
  wasDay = isDay;
  Serial.print("Sunrise is at ");
  Serial.print(sunrise);
  Serial.print(" today and sunset is at ");
  Serial.println(sunset);
  if (isDay) {
    Serial.println("It is currently day time");
  } else {
    Serial.println("It is currently night time");
  }
}

bool isDayNow() {
  RTC.read(tm);
  int secondsTime = int(tm.Hour) * 60 + int(tm.Minute);
  return (secondsTime > sunrise & secondsTime <= sunset);
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

void printTime() {
  if (RTC.read(tm)) {
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
  }
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
  ledState = !ledState;

  // set the LED with the ledState of the variable:
  digitalWrite(ledPin, ledState);
}

void closeDoor() {
  // TODO: flash LED
  Serial.println("Closing door");
  myServo.write(closedAngle);
  isOpen = !isOpen;
}

void openDoor() {
  // TODO: flash LED
  Serial.println("Opening door");
  myServo.write(openAngle);
  isOpen = !isOpen;
}

void switchDoor() {
  if (isOpen) {
    openDoor();
  }
  else {
    closeDoor();
  }
}

void loop() {
  if (isDay && !wasDay) {
    // it's now sunrise, need to open the door
    Serial.print("It's now sunrise -- time to open the door: ");
    printTime();
    openDoor();
  } else if (!isDay && wasDay) {
    // it's now sunset, need to close the door
    Serial.print("It's now sunset -- time to close the door: ");
    closeDoor();
  }

  // manual override if button is pushed
  switchState = digitalRead(BUTTON_PIN);
  if (switchState) {
    executeSleep(switchDoor, previousButtonPush, switchDelay);
  }

  // executeSleep(blink, previousMillisLed, blinkInterval);
  executeSleep(printTime, previousMillisPrint, printInterval);
}
