
/*
 * auto-shutter10.ino
 *
 * VERSION 10.0
 *
 * Author: Anwar Hossain
 * github.com/w3cp
 * anwar.ubuntu@gmail.com
 * 01768310499
 *
 * Date Created 20190702
 * Last Modified 201901214
 *
 */

// MOTOR == LED (SAME JINIS)


// lcd
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27,20,4);
// set the LCD address to 0x27 for a 20 chars and 4 line display
// lcd pin to A4 & A5 (A4 > SDA; A5 > SCL)


// pin declare (for output)
const int led[] = {9, 12, A0, A1, A2, A3};

// pin declare (for input)
#define buttonOff 2
#define buttonDire 3
#define buttonUnload 4
#define buttonLoad 5

// pin declare (for output signal)
#define signalDire 6
#define signalLoad 7
#define signalUnload 8

String motorName[6] = {"Elevator>", "Con Gear>", "Loader  >", "Blower   >", "Jhorna  >", "Gate     >"};
int totalLed = 6;

const int duration = 5000;

int i = 0;

int totalMotorOn = 0;
boolean motorState = false;
int signalPin;

String msg = "";
String buttonName = "";


void setup() {
  lcd.init();
  lcd.backlight();

  for (i = 0; i < totalLed; i++) {
    pinMode(led[i], OUTPUT);
  }

  pinMode(signalDire, OUTPUT);
  pinMode(signalLoad, OUTPUT);
  pinMode(signalUnload, OUTPUT);

  pinMode(buttonOff, INPUT);
  pinMode(buttonLoad, INPUT);
  pinMode(buttonUnload, INPUT);
  pinMode(buttonDire, INPUT);

  // ad
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Toni Electrical");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print("Utpol Kumar Das");
  delay(300);
  lcd.setCursor(0, 2);
  lcd.print("Mohadevpur, Naogaon");
  delay(300);
  lcd.setCursor(0, 3);
  lcd.print("Phone: 01716727945");
}


void loop() {
  // Motor on off
  if (digitalRead(buttonDire) == HIGH ) {
    motorOnOff(6);
  } else if (digitalRead(buttonUnload) == HIGH) {
    motorOnOff(3);
  } else if (digitalRead(buttonLoad) == HIGH) {
    motorOnOff(1);
  } else if (digitalRead(buttonOff) == HIGH) {
    if (motorState == true) {
      motorOnOff(0);
    } else {
      lcd.clear();
      lcd.print("Nothing to close");
      lcd.setCursor(1, 2);
      lcd.print("<><><><><>");
    }
  }
}


void motorOnOff(int totalMotor) {

  int col = 0;

  if (motorState == false) {

    if (totalMotor == 6) {
      buttonName = "Dire ";
      signalPin = signalDire;
    } else if (totalMotor == 3) {
      buttonName = "Unload ";
      signalPin = signalUnload;
    } else if (totalMotor == 1) {
      buttonName = "Load ";
      signalPin = signalLoad;
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    msg = buttonName + "is running..";
    lcd.print(msg);

    delay(duration);

    digitalWrite(signalPin, HIGH);

    for (i = 0; i < totalMotor; i++) {

      digitalWrite(led[i], HIGH);
      totalMotorOn++;

      if (i % 2 == 0) {
          col = 0;
      } else {
          col = 10;
      }

      lcd.setCursor(col, (i+2) / 2);
      lcd.print(motorName[i]);

      delay(duration);

    }

    motorState = true;

  } else if (motorState == true) {

    if (totalMotorOn == 6) {
      buttonName = "Dire ";
    } else if (totalMotorOn == 3) {
      buttonName = "Unload ";
    } else if (totalMotorOn == 1) {
      buttonName = "Load ";
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    msg = buttonName + "is closing...";
    lcd.print(msg);
    delay(duration);

    for (i = totalMotorOn - 1; i >= 0; i--) {

      digitalWrite(led[i], LOW);

      if (i % 2 == 0) {
          col = 0;
      } else {
          col = 10;
      }

      lcd.setCursor(col, (i+2) / 2);
      lcd.print(motorName[i]);

      delay(duration);

    }

    digitalWrite(signalDire, LOW);
    digitalWrite(signalLoad, LOW);
    digitalWrite(signalUnload, LOW);

    totalMotorOn = 0;
    motorState = false;

    lcd.clear();
    lcd.setCursor(0, 0);
    msg = buttonName + "is closed";
    lcd.print(msg);
    lcd.setCursor(1, 2);
    //msg = "Thank You";
    lcd.print("Thank You");
  }
}
