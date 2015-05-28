// Very simple Arduino Lithium-ion battery capacity tester
// from electronicsblog.net

#define LED 13
#define resistor 25

float capacity = 0, value, voltage, current, time = 0;
int maxWatts = 25;

void measure (void) {
  value = analogRead(0);
  voltage = value / 1024 * 5.0;
  current = voltage / resistor;
  capacity = capacity + current / 3600;
  watts = voltage * current
  time++;
  Serial.print("Voltage= ");
  Serial.print(voltage);
  Serial.print("V Current= ");
  Serial.print(current);
  Serial.print("A Capacity= ");
  Serial.print(capacity/1000);
  Serial.print("mAh ");
  Serial.print(" Watts= ");
  Serial.print(watts);
  Serial.print("W ");
  Serial.print("Discharging time= ");
  Serial.print(time / 60);
  Serial.println("m ");
}
boolean x = false;
ISR(TIMER1_OVF_vect) {
  TCNT1 = 0x0BDC;
  x = !x;

  measure();
}
void setup() {
  pinMode(LED, OUTPUT);

  TIMSK1 = 0x01; // enabled global and timer overflow interrupt;
  TCCR1A = 0x00; // normal operation page 148 (mode0);
  TCNT1 = 0x0BDC; // set initial value to remove time error (16bit counter register)
  TCCR1B = 0x04; // start timer/ set clock
  Serial.begin(9600);
};
void loop () {
  digitalWrite(LED, x);

};

