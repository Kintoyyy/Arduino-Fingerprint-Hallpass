#include <Adafruit_Fingerprint.h>
#include <ESP32_Servo.h>
#include <LiquidCrystal_I2C.h>

// Servo
Servo myservo;
int servoPin = 15;

// FingerPrint
HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

// LCD
// enter the I2C address and the dimensions of your LCD here
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Buttons
const int whiteBtn = 12;
const int yellowBtn = 14;
const int blueBtn = 27;
const int redBtn = 26;
const int adminMode = 18;
const int hallpassMode = 19;

// Card sensor
int IRSensor = 2;

int menuIndex = 0;
const int mainMenuSize = 2;
const char* mainMenuItems[] = { "Enroll Student", "Delete Student" };

int enrollMenuIndex = 0;
const int enrollMenuSize = 2;
const char* enrollMenuItems[] = { "Select ID", "Back" };

int deleteMenuIndex = 0;
const int deleteMenuSize = 2;
const char* deleteMenuItems[] = { "Select ID", "Back" };

int HallpassmenuIndex = 0;
const int HallpassMenuSize = 4;
const char* HallpassMenuItems[] = { "Comfort Room", "Canteen", "Library", "Laboratory" };

int subMenu = 0;
int fingerID = 1;

void setup() {
  Serial.begin(115200);

  myservo.attach(servoPin, 500, 2500);
  myservo.write(180);

  lcd.init();
  lcd.clear();
  lcd.backlight();

  pinMode(whiteBtn, INPUT_PULLUP);
  pinMode(yellowBtn, INPUT_PULLUP);
  pinMode(blueBtn, INPUT_PULLUP);
  pinMode(redBtn, INPUT_PULLUP);
  pinMode(adminMode, INPUT_PULLUP);
  pinMode(hallpassMode, INPUT_PULLUP);

  pinMode(IRSensor, INPUT);  // IR Sensor pin INPUT

  while (!Serial)
    ;
  delay(100);

  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");

    lcd.setCursor(0, 0);
    lcd.print("Sensor not");
    lcd.setCursor(0, 1);
    lcd.print("Found :(");

    while (1) { delay(1); }
  }
}

void loop() {
  int whiteBtnState = digitalRead(whiteBtn);
  int yellowBtnState = digitalRead(yellowBtn);
  int blueBtnState = digitalRead(blueBtn);
  int redBtnState = digitalRead(redBtn);

  int adminModeState = digitalRead(adminMode);
  int hallpassModeState = digitalRead(hallpassMode);


  // getFingerprintID();
  if (HallpassmenuIndex == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Choose Hallpass");
    lcd.setCursor(0, 1);
    lcd.print("Select below");
  } else {
    // lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Select Menu:");
    lcd.setCursor(0, 1);
    lcd.print(HallpassMenuItems[HallpassmenuIndex]);
  }
  if (hallpassModeState == LOW) {

    while (digitalRead(whiteBtn) == HIGH && digitalRead(yellowBtn) == HIGH && digitalRead(blueBtn) == HIGH && digitalRead(redBtn) == HIGH)
      ;

    Serial.print(HallpassmenuIndex);
    Serial.print(HallpassMenuItems[HallpassmenuIndex]);
    lcd.setCursor(0, 0);
    lcd.print("->");
    if (digitalRead(whiteBtn) == LOW) {  // Hallpass 1
      HallpassmenuIndex = 1;
      lcd.clear();
      while (getFingerprintID() == -1) {
      }
    } else if (digitalRead(yellowBtn) == LOW) {  // Hallpass 2
      HallpassmenuIndex = 2;
      lcd.clear();
      while (getFingerprintID() == -1) {
      }
    } else if (digitalRead(blueBtn) == LOW) {  // Hallpass 3
      HallpassmenuIndex = 3;
      lcd.clear();
      while (getFingerprintID() == -1) {
      }
    } else if (digitalRead(redBtn) == LOW) {  // Hallpass 4
      HallpassmenuIndex = 4;
      lcd.clear();
      while (getFingerprintID() == -1) {
      }
    }
    delay(100);
    while (digitalRead(whiteBtn) == LOW || digitalRead(yellowBtn) == LOW || digitalRead(blueBtn) == LOW || digitalRead(redBtn) == LOW)
      ;
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hallpass");
    lcd.setCursor(0, 1);
    lcd.print("No Card");
  }

  if (adminModeState == LOW) {
    AdminMode();
  }

  delay(50);
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintID() {
  uint8_t p = finger.getImage();
  while (p != FINGERPRINT_OK) {
    // if (digitalRead(whiteBtn) == LOW | digitalRead(yellowBtn) == LOW | digitalRead(blueBtn) == LOW | digitalRead(redBtn) == LOW) {
    //   HallpassmenuIndex = 0;
    //   lcd.clear();
    //   break;
    // }
    lcd.print("->");
    lcd.setCursor(2, 0);
    lcd.print(HallpassMenuItems[HallpassmenuIndex]);
    lcd.setCursor(0, 1);
    lcd.print("Scan finger");
    return -1;
  }
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  HallpassmenuIndex = 0;
  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);


  student(finger.fingerID);
  return finger.fingerID;
}

void student(int id) {
  String student;
  switch (id) {
    case 1:
      student = "Kent Rato";
      break;
    case 2:
      student = "Nash Gwapo";
      break;
    case 3:
      student = "Erie myIdol";
      break;
    case 4:
      student = "Aries my bby";
      break;
    case 5:
      student = "gelo for president";
      break;
    case 6:
      student = "mommy charles";
      break;
    default:
      student = "student #" + (String)id;
  }
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(student);
  lcd.setCursor(0, 0);
  lcd.print("Please Get Card");
  releaseCard();
}
void AdminMode() {
  if (subMenu == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Select Menu:");
    lcd.setCursor(0, 1);
    lcd.print(mainMenuItems[menuIndex]);
  } else if (subMenu == 1) {
    if (menuIndex == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print((String)mainMenuItems[menuIndex] + ":");
      lcd.setCursor(0, 1);
      lcd.print(enrollMenuItems[enrollMenuIndex]);
    } else if (menuIndex == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print((String)mainMenuItems[menuIndex] + ":");
      lcd.setCursor(0, 1);
      lcd.print(deleteMenuItems[deleteMenuIndex]);
    }
  } else if (subMenu == 2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set number: " + (String)fingerID);
    lcd.setCursor(0, 1);
    lcd.print("Ok   +  -   Exit");
  }

  while (digitalRead(whiteBtn) == HIGH && digitalRead(yellowBtn) == HIGH && digitalRead(blueBtn) == HIGH && digitalRead(redBtn) == HIGH)
    ;
  if (digitalRead(whiteBtn) == LOW) {  // Green Button // Cycle menus

    if (subMenu == 0) {
      menuIndex = (menuIndex + 1) % mainMenuSize;  // Cycle through Mainmenu options
    } else if (subMenu == 1) {
      if (menuIndex == 0) {
        enrollMenuIndex = (enrollMenuIndex + 1) % enrollMenuSize;  // Cycle through enrollmenu options

      } else if (menuIndex == 1) {
        deleteMenuIndex = (deleteMenuIndex + 1) % deleteMenuSize;  // Cycle through deletemenu options
      }
    } else if (subMenu == 2) {
      lcd.clear();
      lcd.setCursor(0, 0);
      if (menuIndex == 0) {
        while (!getFingerprintEnroll(fingerID))
          ;
        enrollMenuIndex = 0;
        subMenu = 0;
      } else if (menuIndex == 1) {

        while (!deleteFingerprint(fingerID))
          ;

        delay(50);
        deleteMenuIndex = 0;
        subMenu = 0;
      }
      Serial.print(fingerID);
      int fingerID = 0;
      delay(1000);
      lcd.clear();
    }


  } else if (digitalRead(yellowBtn) == LOW) {  // Yellow button // Select menu // add

    if (subMenu == 0) {
      subMenu = 1;
    } else if (subMenu == 1) {
      if (menuIndex == 0) {
        if (enrollMenuIndex == 0) {
          subMenu = 2;
        } else {
          enrollMenuIndex = 0;
          subMenu = 0;
        }
      } else if (menuIndex == 1) {
        if (deleteMenuIndex == 0) {
          subMenu = 2;
        } else {
          deleteMenuIndex = 0;
          subMenu = 0;
        }
      }
    } else if (subMenu == 2) {
      if (fingerID < 127) {
        fingerID++;
      }
    }

  } else if (digitalRead(blueBtn) == LOW) {  // Blue button // subtract

    if (subMenu == 0) {
      finger.getTemplateCount();
      lcd.clear();
      if (finger.templateCount == 0) {

        lcd.setCursor(0, 0);
        lcd.print("No Fingerprint");
        lcd.setCursor(0, 1);
        lcd.print("Data");

      } else {
        Serial.println("Waiting for valid finger...");
        Serial.print("Sensor contains ");
        Serial.print(finger.templateCount);
        Serial.println(" templates");

        lcd.setCursor(0, 0);
        lcd.print("Data Found!");
        lcd.setCursor(0, 1);
        lcd.print("fingerprint: " + (String)finger.templateCount);
      }
      delay(1000);
      lcd.clear();
    } else if (subMenu == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Developer :)");
      lcd.setCursor(0, 1);
      lcd.print("github/Kintoyyy");
      delay(3000);
      subMenu = 0;
    } else if (subMenu == 2) {
      if (fingerID > 1) {
        fingerID--;
      }
    }
  } else if (digitalRead(redBtn) == LOW) {  // Red button // Exit
    lcd.clear();
    subMenu = 0;
    menuIndex = 0;
    enrollMenuIndex = 0;
    deleteMenuIndex = 0;
  }
  delay(100);
  while (digitalRead(whiteBtn) == LOW || digitalRead(yellowBtn) == LOW || digitalRead(blueBtn) == LOW || digitalRead(redBtn) == LOW)
    ;
}

void releaseCard() {
  Serial.print("Release card");
  int pos = 180;
  for (pos = 180; pos >= 0; pos -= 1) {
    myservo.write(pos);
    delay(5);
  }

  delay(10000);
  lcd.clear();
  for (pos = 0; pos <= 180; pos += 1) {
    myservo.write(pos);
    delay(5);
  }
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;

  p = finger.deleteModel(id);
  Serial.println(p);
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Deleted: " + (String)id);

    Serial.println("Deleted!");

  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cannot Delete");
    return p;

  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.print("Unknown error: 0x");
    Serial.println(p, HEX);
    return p;
  }
  return true;
}


uint8_t getFingerprintEnroll(uint8_t id) {
  lcd.clear();
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Fingerprint");
        lcd.setCursor(0, 1);
        lcd.print("Found!");
        Serial.println("Image taken");

        delay(500);

        break;
      case FINGERPRINT_NOFINGER:

        lcd.setCursor(0, 0);
        lcd.print("waiting for");
        lcd.setCursor(0, 1);
        lcd.print("finger");

        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cannot find");
      lcd.setCursor(0, 1);
      lcd.print("fingerprint");

      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cannot find");
      lcd.setCursor(0, 1);
      lcd.print("fingerprint");

      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Remove finger");

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place finger");
  lcd.setCursor(0, 1);
  lcd.print("again");

  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cannot find");
      lcd.setCursor(0, 1);
      lcd.print("fingerprint");

      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cannot find");
      lcd.setCursor(0, 1);
      lcd.print("fingerprint");

      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fingerprints");
    lcd.setCursor(0, 1);
    lcd.print("did not match");

    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Finger Print");
    lcd.setCursor(0, 1);
    lcd.print("Saved!");

  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}


void idle_screen() {
  lcd.setCursor(1, 0);
  lcd.print("FingerPrint");

  lcd.setCursor(2, 1);
  lcd.print("Hallpass");
}
