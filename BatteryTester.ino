/*
  This sketch monitors the variables of a battery as it discharges and gives you the total milliamp hours capacity of the battery

  Based on original sketch from electronicsblog.net

  todo:  add safety circuit
       add selectable voltage? need?
       flashing LED to show finished state

*/
#include <LiquidCrystal.h>
#include <Time.h>
#include <DS1307RTC.h>

#define LED 13
const int cutoffPin = 9;
const int ratePin = 8;
const int chargePin = 7;
const int startPin = 10;
//LiquidCrystal lcd(12, 11, 6, 5, 4, 3);
LiquidCrystal lcd(12, 11, 3, 4, 5, 6);

// Change the values directly below to suit your discharge resistor
int slowResistor = 5; // a 15 ohm resistor would be optimal
int fastResistor = 5; //Ohms faster discharge of battery. More heat and possible battery shutout on smaller capacities. Use slow discharge for those.
int slowMaxWatts = 5; // if you exceed this value you will fry your resistor   //Need to add code and circuit to break circuit if this is exceeded
int fastMaxWatts = 50;
float powerV = 4.25; // If readings are off check arduino supply voltage and adjust here
int rateSwitch = 0;
// Thats it!

float voltageCutoff = 2.5; // if the LiPo battery does not have internal cutoff security this will stop the test before damaging battery.
int capacitymAh = 0;
float capacity = 0, value, shuntV, current, batteryV, time = 0, watts = 0;
int buttonState = 0;
int lastButtonState = LOW;
int startState = 0;
int lastStartState = 1;
int processSecond = second();

long debounceDelay = 100;    // the debounce time; increase if the output flickers

boolean debounce(int pin) {
  boolean state;
  boolean previousState;

  //switch debounce
  previousState = digitalRead(pin);
  for (int counter = 0; counter < debounceDelay; counter++) {
    delay(1);
    state = digitalRead(pin);
    if (state != previousState) {
      counter = 0;
      previousState = state;
    }
  }
  return state;
}



boolean x = false;
ISR(TIMER1_OVF_vect) {
  TCNT1 = 0x0BDC;
  x = !x;
  Serial.print("ratePin= ");
  Serial.print(digitalRead(ratePin));
  rateSwitch = (digitalRead(ratePin));
  if (rateSwitch == 0) {
    slowmeasure();
  }
  else if (rateSwitch == 1) {

    fastmeasure();
  }
}

void setup() {
  lcd.begin(16, 2);
  pinMode(LED, OUTPUT);
  pinMode(cutoffPin, OUTPUT);
  pinMode(ratePin, INPUT);
  pinMode(chargePin, INPUT);
  pinMode(startPin, INPUT);
  digitalWrite(cutoffPin, LOW);
  digitalWrite(LED, LOW);

  //RTX Time Sync

  if (timeStatus() != timeSet)
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");
  digitalClockDisplay();
  TIMSK1 = 0x01; // enabled global and timer overflow interrupt;
  TCCR1A = 0x00; // normal operation page 148 (mode0);
  TCNT1 = 0x0BDC; // set initial value to remove time error (16bit counter register)
  TCCR1B = 0x04; // start timer/ set clock
  Serial.begin(9600);
  Serial.println("new version");
};


void loop () {
  if (shuntV == 0.00) { //flash LED when test complete (lithium battery under voltage cutoff)
    //digitalWrite(LED, x);
  }

  if ((batteryV <= voltageCutoff) && (shuntV > 0.01)) {
    digitalWrite(cutoffPin, LOW);               //*****Finish this
    digitalWrite(LED, LOW);
    Serial.println("Voltage Cut Off Reached!");

  }
  if (ratePin == LOW) {
    if  (watts >= slowMaxWatts) {
      digitalWrite(cutoffPin, LOW);
      digitalWrite(LED, LOW);
      Serial.println("Watt limit reached on SLOW discharge!");
    }

    else if (ratePin == HIGH) {
      if  (watts >= fastMaxWatts) {
        digitalWrite(cutoffPin, LOW);
        digitalWrite(LED, LOW);
        Serial.println("Watt limit reached on FAST discharge!");
      }
    }
  }

  // buttonState = digitalRead(startPin);
  // int reading = digitalRead(startPin);
  startState = (debounce(startPin));
  //Serial.print("Start state: ");
  //Serial.println(startState);
  // if (startState == HIGH && lastStartState == LOW) {
  if (startState == HIGH) {

    // lastStartState = startState;

    if (buttonState == HIGH) {
      digitalWrite(LED, HIGH);
      Serial.println("Go!");
      digitalWrite(cutoffPin, HIGH);
      buttonState = LOW;
    } else  {
      digitalWrite(LED, LOW);
      digitalWrite(cutoffPin, LOW);
      Serial.println("No Go!");
      buttonState = HIGH;
    }
  }
  //hmmmm this has been functioned
  if (processSecond != second()) {
    processSecond = second();
    //   slowmeasure();
  }
  /*
    value = analogRead(0);
    voltage = (value / 1024) * powerV;
    current = voltage / slowResistor; /////*****This needs fixing hard set to slow
    capacity = capacity + current / 3600;
    watts = voltage * current;
    capacitymAh = capacity * 1000;
    lcdupdate();

    }
  */

}

//////////////////////////////////////////////////////////////////////////////
void fastmeasure (void) {
  Serial.println("Fast Discharge");
  value = analogRead(0);
  shuntV = (value / 1024) * powerV;
  value = analogRead(1);
  batteryV = (value / 1024) * powerV;
  current = shuntV / fastResistor;
  capacity = capacity + current / 3600;
  watts = shuntV * current;
  capacitymAh = capacity * 1000;
  time++;
  Serial.print("Shunt Voltage= ");
  Serial.print(shuntV);
  Serial.print("Battery Voltage= ");
  Serial.print(batteryV);
  Serial.print("V Current= ");
  Serial.print(current);
  Serial.print("A Capacity= ");
  Serial.print(capacity);
  Serial.print(" ");
  Serial.print(capacitymAh);
  Serial.print("mAh ");
  Serial.print(" Watts= ");
  Serial.print(watts);
  Serial.print("W ");
  Serial.print("Discharging time= ");
  Serial.print(time / 60);
  Serial.println("m ");

  lcd.setCursor(0, 0);
  // lcd.print("V:");
  lcd.print(shuntV);
  lcd.print("V ");
  lcd.print(current);
  lcd.print("A ");
  lcd.print(watts);
  lcd.print("W");

  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(time / 60);
  lcd.print("M ");
  lcd.print("C:");
  lcd.print(capacitymAh);
  lcd.print("mAh");
}
void lcdupdate (void) {
  lcd.setCursor(0, 0);
  lcd.print(shuntV);
  lcd.print("V ");
  lcd.print(current);
  lcd.print("A ");
  lcd.print(watts);
  lcd.print("W");
  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(time / 60);
  lcd.print("M ");
  lcd.print("C:");
  lcd.print(capacitymAh);
  lcd.print("mAh");
}

void slowmeasure (void) {
  Serial.println(" Slow Discharge");
  value = analogRead(0);
  shuntV = (value / 1024) * powerV; // Takes analogue value and converts to V
  value = analogRead(1);
  batteryV = (value / 1024) * powerV;
  current = shuntV / slowResistor;
  capacity = capacity + current / 3600;
  watts = shuntV * current;
  capacitymAh = capacity * 1000;
  time++;
  Serial.print("Shunt Voltage= ");
  Serial.print(shuntV);
  Serial.print("Battery Voltage= ");
  Serial.print(batteryV);
  Serial.print("V Current= ");
  Serial.print(current);
  Serial.print("A Capacity= ");
  Serial.print(capacity);
  Serial.print(" ");
  Serial.print(capacitymAh);
  Serial.print("mAh ");
  Serial.print(" Watts= ");
  Serial.print(watts);
  Serial.print("W ");
  Serial.print("Discharging time= ");
  Serial.print(time / 60);
  Serial.println("m ");

  lcd.setCursor(0, 0);
  // lcd.print("V:");
  lcd.print(shuntV);
  lcd.print("V ");
  lcd.print(current);
  lcd.print("A ");
  lcd.print(watts);
  lcd.print("W");

  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(time / 60);
  lcd.print("M ");
  lcd.print("C:");
  lcd.print(capacitymAh);
  lcd.print("mAh");
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
