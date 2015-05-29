/*
This sketch monitors the variables of a battery as it discharges and gives you the total milliamp hours capacity of the battery

Based on original sketch from electronicsblog.net

todo:  add safety circuit
       add selectable voltage? need?
       flashing LED to show finished state

*/
#include <LiquidCrystal.h>
#define LED 13

//LiquidCrystal lcd(12, 11, 6, 5, 4, 3);
LiquidCrystal lcd(12, 11, 3, 4, 5, 6);

// Change the values directly below to suit your discharge resistor
int resistor = 25; // a 10 ohm resistor would be optimal
int maxWatts = 25; // if you exceed this value you will fry your resistor   //Need to add code and circuit to break circuit if this is exceeded
float powerV = 4.34; // If readings are off check arduino supply voltage and adjust here
// Thats it!

int capacitymAh = 0;
float capacity = 0, value, voltage, current, time = 0, watts = 0;

void measure (void) {
  value = analogRead(0);
  voltage = (value / 1024) * powerV;
  current = voltage / resistor;
  capacity = capacity + current / 3600;
  watts = voltage * current;
  capacitymAh = capacity * 1000;
  time++;
  Serial.print("Voltage= ");
  Serial.print(voltage);
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
  lcd.print(voltage);
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
boolean x = false;
ISR(TIMER1_OVF_vect) {
  TCNT1 = 0x0BDC;
  x = !x;

  measure();
}
void setup() {
  lcd.begin(16, 2);
  pinMode(LED, OUTPUT);

  TIMSK1 = 0x01; // enabled global and timer overflow interrupt;
  TCCR1A = 0x00; // normal operation page 148 (mode0);
  TCNT1 = 0x0BDC; // set initial value to remove time error (16bit counter register)
  TCCR1B = 0x04; // start timer/ set clock
  Serial.begin(9600);
};
void loop () {
  if (voltage==0.00) { //flash LED when test complete (lithium battery under voltage cutoff)  
      digitalWrite(LED, x);
  }

};

