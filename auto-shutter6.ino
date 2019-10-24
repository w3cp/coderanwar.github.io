
/* 
 * auto-shutter6.ino
 *
 * VERSION 6.0
 * 
 * Author: Anwar Hossain
 * github.com/w3cp
 * anwar.ubuntu@gmail.com
 * 01768310499
 * 
 * Date Created 20190702
 * Last Modified 201901016
 *
 */

// MOTOR == LED (SAME JINIS)

const String BUILT_IN_CONTACT_NUMBER = "01736106218";
const String BUILT_IN_CONTACT_NAME ="Rofiqul Islam";

#define cmdOn "I love you"
#define cmdOff "I hate you"
#define cmdStatus "Status"

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
int index_num_start, index_num_end, index_msg_start, index_msg_end;

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

String motorName[6] = {"Elevator>", "FrontGear>", "Loader  >", "Blower   >", "Jhorna  >", "Gate     >"};
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
  
  Serial.begin(9600); 
  sim.begin(9600);

  //checkNumber();
  
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

  lcd.setCursor(0, 0);
  lcd.print("Checking System ...");
  lcd.setCursor(0, 1);
  lcd.print("Please wait");

  checkNumber();

  lcd.setCursor(8, 2);
  lcd.print("System is OK");
  delay(20);

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
  sim.println("AT&W"); // save current command parameter permanently
  ******************************************/

  //checkNumber();

  showAd();
  
  Serial.println("System Started...");
}


void loop() {
  if (sim.available() > 0) {
     msg  = sim.readString();

     // this portion is added for rare case (in case of suspend accidentaly)
     if (msg.indexOf("+CPBF:")!=-1) {
       if (msg.indexOf(BUILT_IN_CONTACT_NUMBER)!=-1) {
         EEPROM.update(0, 1);
         showAd();
         Serial.println("Number is right. [matched]");
       }
     }

     Serial.println(msg);
     
     if (msg.indexOf("+CMT:") != -1) { 
       index_num_start = msg.indexOf("\"");
       index_num_start++;
       index_num_end = msg.indexOf("\"", index_num_start);
       phn_num = msg.substring(index_num_start, index_num_end); 
       Serial.println(phn_num);
       //Serial.println(msg);
       index_msg_start = msg.indexOf("\n", index_num_end);
       index_msg_end = msg.length();
       msg_cmd = msg.substring(index_msg_start, index_msg_end);
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
              } else if (digitalRead(signalUnload) == HIGH) {
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
  }
  
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
          lcd.setCursor(1, 2);
          lcd.print("[+8801736106218]");
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
  //sim.println("AT+CMGF=1"); //Sets the GSM Module in Text Mode 
  //delay(1000); 
  sim.println("AT+CMGS=\"" + number + "\"\r"); 
  delay(100); 
  sim.println(myMessage); 
  delay(100); 
  sim.println((char)26);// ASCII code of CTRL+Z 
  delay(100); 
}


void checkNumber() {
  while (1) {
    //sim.println("ATD+8801768310499;\r\n");
    sim.println("AT+CPBS=\"SM\""); // SET PHONEBOOK STORAGE TO SIM
    delay(50);
    String cmd = "AT+CPBF=\"" + BUILT_IN_CONTACT_NAME + "\"";
    sim.println(cmd); //read phonebook from index 1 to 50
    delay(50);
    
    uint64_t timeOld = millis();
    while (!sim.available() && !(millis() > timeOld + 5000)) {
      delay(13);
    }
    
    String str;
    
    while(sim.available()) {
      if (sim.available() > 0) {
        //str += (char) sim.read();
        str = sim.readString();
        //Serial.println(str); // FOR DEBUGGING PURPOSE
      }
    }
  
    if (str.indexOf("+CPBF:")!=-1) {
      if (str.indexOf(BUILT_IN_CONTACT_NUMBER)==-1) {
        EEPROM.update(0, 0);
        
        Serial.println("System off due to unknown sim card");
    
        // you can delete from this line
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("System Shut Down!");
        lcd.setCursor(0, 1);
        lcd.print("Unknwon SIM Card");
        lcd.setCursor(0, 2);
        lcd.print("Power off first>");
        lcd.setCursor(0, 3);
        lcd.print("Insert right SIM");
        delay(10000);
        // to this line
        
      } else {
        EEPROM.update(0, 1);
        Serial.println("Number is right. [matched]");
        Serial.println(str);
      }
      break;
    }
  }
}


void showAd() {
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
