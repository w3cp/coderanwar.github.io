
/*
 * auto-shutter4.2.ino
 *
 * VERSION 4.2
 *
 * Author: Anwar Hossain
 * github.com/w3cp
 * anwar.ubuntu@gmail.com
 * 01768310499
 *
 * Date Created 20190702
 * Last Modified 20191029
 *
 */

// MOTOR == LED (SAME JINIS)


// gsm
#include <SoftwareSerial.h>
//eeprom
#include <EEPROM.h>
// lcd
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


SoftwareSerial sim(10, 11); // arduino rx pin 10 to gsm pin tx, arduino tx pin 11 to gsm pin rx

String phn_num = "";
String msg_cmd = "";
int index_start, index_end;

const String cmdOn = "I love you";
const String cmdOff = "I hate you";
const String cmdStatus = "Status";

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

String motorName[6] = {"Elevator>", "Con  Gear>", "Loader  >", "Blower   >", "Jhorna  >", "Gate     >"};
int totalLed = 6;

const int duration = 5000;

int i = 0;

int totalMotorOn = 0;
boolean motorState = false;
int signalPin;

String msg = "";
String buttonName = "";


void setup() {
  lcd.init();  // initialize the lcd
  lcd.backlight();

  Serial.begin(9600);
  sim.begin(9600);

  Serial.println("System Started...");

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

  sim.println("AT+CMGD=1,4"); // delete all msgs
  delay(200);
  // IMPORTANT: Please comment out below portion (below 10 lines) to use the gsm for the first time
  /******************************************
  delay(3000);
  sim.println("AT+CLIP=1"); // Calling line identification presentation
  delay(1000);
  sim.println("AT+CMGF=1"); //Sets the GSM Module in Text Mode
  delay(1000);
  sim.println("AT+CNMI=2,2,0,0,0"); // listen live msg
  delay(1000);
  sim.println("AT&W"); // save cunnrent command parameter permanently
  ******************************************/
}


void loop() {
    if (sim.available() > 0) {
       msg  = sim.readString();
     }

     Serial.println(msg);

     if (msg.indexOf("+CMT:") != -1) {
       index_start = msg.indexOf("\"");
       index_start++;
       index_end = msg.indexOf("\"", index_start);
       phn_num = msg.substring(index_start, index_end);
       Serial.println(phn_num);
       //Serial.println(msg);
       index_start = msg.indexOf("\n", index_end);
       index_end = msg.length();
       msg_cmd = msg.substring(index_start, index_end);
       msg_cmd.trim(); // to remove any leading and trailing whitespace
       Serial.println(msg_cmd);

       if (phn_num.indexOf("+880") != -1) {
         if (msg_cmd == cmdOn) {
           EEPROM.update(0, 1);
           SendMessage(phn_num, "Shutter System On");
         } else if (msg_cmd == cmdOff) {
           EEPROM.update(0, 0);
           SendMessage(phn_num, "Shutter System Off");
         } else if (msg_cmd == cmdStatus) {
            if (EEPROM.read(0) == 1) {
              if (digitalRead(signalDire) == HIGH) {
                SendMessage(phn_num, "Shutter System is On. Dire is running.");
              } else if (digitalRead(signalLoad) == HIGH) {
                SendMessage(phn_num, "Shutter System is On. Load is running.");
              }  else if (digitalRead(signalUnload) == HIGH) {
                SendMessage(phn_num, "Shutter System is On. Unload is running.");
              } else {
                SendMessage(phn_num, "Shutter System is On. But nothing is running.");
              }
            } else {
              SendMessage(phn_num, "Shutter System is Off.");
            }
         } else {
           SendMessage(phn_num, "Wrong password! Contact Star Electronics (Rofiqul). Mobile: [01736106218] to know the keyword.");
         }
       }
     }
     //Serial.println(msg.length());

     if (EEPROM.read(0) == 1) {
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
          }
        }
        } else {
        // ALL MOTOR OFF
        for (i = 0; i < totalLed; i++) {
          digitalWrite(led[i], LOW);
        }
        digitalWrite(signalDire, LOW);
        digitalWrite(signalLoad, LOW);
        digitalWrite(signalUnload, LOW);

        motorState = false;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("You are suspended!");
        lcd.setCursor(0, 1);
        lcd.print("Payment Please..");
        lcd.setCursor(0, 2);
        lcd.print("Contact Utpol Kumar");
        lcd.setCursor(0, 3);
        lcd.print("Phone: 01716727945");
        delay(3000);
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


void SendMessage(String number, String myMessage) {
  sim.println("AT+CMGF=1"); //Sets the GSM Module in Text Mode
  delay(1000);
  sim.println("AT+CMGS=\"" + number + "\"\r");
  delay(1000);
  sim.println(myMessage);
  delay(100);
  sim.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
