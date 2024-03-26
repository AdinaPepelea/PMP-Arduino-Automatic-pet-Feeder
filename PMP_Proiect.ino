#include <Servo.h>
#include <LiquidCrystal_I2C.h>
Servo servo;

int const afisare_distanta = 1;

int TRIG_PIN = 2;
int ECHO_PIN = 3;
int SERVO_PIN = 9;
int LED_PIN = 8;

int MEASUREMENTS = 3;
long SECONDS_TO_WAIT_FOR_SERVO = 1000;
long TIME_BEFORE_EAT = 40000;
long MIN_DISTANCE = 15;
int OPEN_SERVO = 45;
int CLOSE_SERVO = 0;

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

unsigned long measure() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(15);
  digitalWrite(TRIG_PIN, LOW);
  long pulse_duration_ms = pulseIn(ECHO_PIN, HIGH);
  return microsecondsToCentimeters(pulse_duration_ms);
}

long lastMovementTime = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  servo.attach(SERVO_PIN);
  servo.write(CLOSE_SERVO);
  delay(1000);
  servo.detach();
  if (afisare_distanta) {
    Serial.begin(9600);
  }
  lcd.begin();
  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Timp ramas: ");
  float dist = 0;
  for (int i = 0; i < MEASUREMENTS; i++) {
    dist += measure();
    delay(50);
  }
  float avg_dist_cm = dist / MEASUREMENTS;

  long currentTime = millis();
  if (avg_dist_cm < MIN_DISTANCE && (currentTime - lastMovementTime >= TIME_BEFORE_EAT || lastMovementTime == 0)) {
    servo.attach(SERVO_PIN);
    digitalWrite(LED_PIN, HIGH);
    delay(1);
    servo.write(OPEN_SERVO);
    delay(SECONDS_TO_WAIT_FOR_SERVO);
    servo.write(CLOSE_SERVO);
    delay(1000);
    servo.detach();
    digitalWrite(LED_PIN, LOW);
    lastMovementTime = currentTime;
  }
  if (currentTime - lastMovementTime < TIME_BEFORE_EAT && lastMovementTime != 0 ){
    digitalWrite(LED_PIN, LOW);
    if((40 - (currentTime-lastMovementTime)/1000 )>= 10){
      lcd.setCursor(8,1);
      lcd.print(40 - (currentTime-lastMovementTime)/1000);
    } else {
      lcd.setCursor(8,1);
      lcd.print('0');
      lcd.print((40 - (currentTime-lastMovementTime)/1000));
    }
  }else{
    lcd.setCursor(8, 1);
    lcd.print("00");
    digitalWrite(LED_PIN, HIGH);
  }
  if (afisare_distanta) {
    Serial.print(avg_dist_cm);
    Serial.print("cm");
    Serial.println();
  }
}
