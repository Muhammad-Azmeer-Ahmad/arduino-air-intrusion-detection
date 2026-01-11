#include <Servo.h>

Servo s;
const int trigPin = 7;
const int echoPin = 6;

int alertDistance = 50;
bool sweeping = false;
int fixedAngle = 90;

void setup() {
  s.attach(8);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  showMenu();
}

long readDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;

  long distance = duration * 0.034 / 2;
  return distance;
}

void showMenu() {
  Serial.println(F("\n===================="));
  Serial.println(F("       MAIN MENU        "));
  Serial.println(F("===================="));
  Serial.println(F("1 : Start Sweep"));
  Serial.println(F("2 : Stop Sweep"));
  Serial.println(F("3 : Check Angle / Distance"));
  Serial.println(F("4 : Change Alert Distance"));
  Serial.println(F("M : Show Menu"));
  Serial.print(F("Current Alert Distance: "));
  Serial.println(alertDistance);
  Serial.println(F("====================\n"));
}

void loop() {

  if (Serial.available() > 0) {
    char input = Serial.read();
    input = toupper(input);

    switch (input) {
      case '1':
        sweeping = true;
        Serial.println(F("Sweep Started"));
        break;

      case '2':
        sweeping = false;
        Serial.println(F("Sweep Stopped"));
        break;

      case '3':
        Serial.println(F("Enter angle 0-180: "));
        while (!Serial.available());
        fixedAngle = Serial.parseInt();
        
        if (fixedAngle >= 0 && fixedAngle <= 180) {
          s.write(fixedAngle);
          long dist = readDistanceCM();
          Serial.print(F("Angle: "));
          Serial.print(fixedAngle);
          Serial.print(F(" | Distance: "));
          if (dist != -1) Serial.println(dist);
          else Serial.println(F("Out of range"));

          if (dist != -1 && dist <= alertDistance)
            Serial.println(F("!! ALERT: Object Near !!"));

        } else {
          Serial.println(F("Invalid angle!"));
        }
        break;

      case '4':
        Serial.println(F("Enter new alert distance: "));
        while (!Serial.available());
        alertDistance = Serial.parseInt();
        Serial.print(F("Alert distance set to: "));
        Serial.println(alertDistance);
        break;

      case 'M':
        showMenu();
        break;

      default:
        Serial.println(F("Invalid option! Press M for Menu"));
    }
  }

  if (sweeping) {
    for (int pos = 0; pos <= 180 && sweeping; pos += 2) {
      s.write(pos);
      delay(40);
      displayDistance(pos);
    }

    for (int pos = 180; pos >= 0 && sweeping; pos -= 2) {
      s.write(pos);
      delay(40);
      displayDistance(pos);
    }
  }
}

void displayDistance(int angle) {
  long dist = readDistanceCM();
  Serial.print(F("A:"));
  Serial.print(angle);
  Serial.print(F(" D:"));

  if (dist != -1) {
    Serial.println(dist);
    if (dist <= alertDistance)
      Serial.println(F("ALERT"));
  } else {
    Serial.println(F("OUT"));
  }
}
