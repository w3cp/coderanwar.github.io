/* 
 * auto-shutter.ino
 * 
 * Author: Anwar Hossain
 * github.com/w3cp
 * anwar.ubuntu@gmail.com
 * 01768310499
 * 
 * Date Created 20190702
 * Last Modified 20190707
 *
 */

// MOTOR == LED (SAME JINIS)


#include <LiquidCrystal.h>


// lcd pin declare
const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// pin declare (for output)
const int led[] = {A0, A1, A2, A3, A4, A5};

// pin declare (for input)
#define buttonOff 2
#define buttonDire 3
#define buttonUnload 4
#define buttonLoad 5

// pin declare (for output signal)
#define signalDire 13
#define signalLoad A6
#define signalUnload A7

String motorName[6] = {"Elevator>", "FrontGear>", "Loader  >", "Blower   >", "Jhorna  >", "Gate     >"};
int totalLed = 6;
int duration = 5000;

int i = 0;

int totalMotorOn = 0;
boolean motorState = false;
int signalPin;

String msg = "";
String buttonName = "";


void setup() {
  lcd.begin(20, 4);
  
  lcd.setCursor(0, 0);
  lcd.print("Toni Electrical");
  delay(800);
  lcd.setCursor(0, 1);
  lcd.print("Utpol Kumar Das");
  delay(800);
  lcd.setCursor(0, 2);
  lcd.print("Mohadevpur, Naogaon");
  delay(800);
  lcd.setCursor(0, 3);
  lcd.print("Phone: 01716727945");
  
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
        lcd.print("Nothing to close.");
        lcd.setCursor(1, 2);
        lcd.print("[+8801736106218]");
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
    msg = buttonName + "is running...";
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
    msg = buttonName + "is closed";
    lcd.print(msg);
    lcd.setCursor(1, 2);
    msg = "Thank You";
    lcd.print(msg);
  }
}
