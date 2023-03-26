#include <Adafruit_Fingerprint.h>
#include <LCDWIKI_GUI.h>
#include <LCDWIKI_KBV.h>

LCDWIKI_KBV mylcd(ILI9341,A3,A2,A1,A0,A4);

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF



#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)

SoftwareSerial mySerial(17, 18);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


void setup()
{
  Serial.begin(9600);
  mylcd.Init_LCD();
  Serial.println(mylcd.Read_ID(), HEX);
  mylcd.Fill_Screen(BLACK); 
    mylcd.Set_Rotation(1); 
  mylcd.Set_Text_Mode(0);
  
  mylcd.Fill_Screen(BLACK);  
  mylcd.Set_Text_Back_colour(BLACK);

  mylcd.Set_Text_colour(WHITE);
  mylcd.Set_Text_Size(4);  
  mylcd.Print_String("Prilozte", 70, 66); 

  mylcd.Set_Text_Size(4);
  mylcd.Print_String("prst", 120, 106);

  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nDetekcia odtlacku prsta");
  pinMode(14, OUTPUT);

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Našiel sa senzor!");
  } else {
    Serial.println("Nenašiel sa senzor");
    while (1) { delay(1); }
  }

  Serial.println(F("Načitavam parametre senzoru"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please add some.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
}

void loop()                     // run over and over again
{
  getFingerprintID();
  delay(100);            //don't ned to run this at full speed.

}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }


  // OK success!

  p = finger.image2Tz();
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
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Našla sa zhoda!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("chyba");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("nenašlo zhodu"); 
    finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 25, FINGERPRINT_LED_RED, 10);
  delay(800);
  
  mylcd.Set_Rotation(1);
  mylcd.Fill_Screen(BLACK);  
  mylcd.Set_Text_Mode(0);
  
  mylcd.Fill_Screen(BLACK);  
  mylcd.Set_Text_Back_colour(BLACK);

  mylcd.Set_Text_colour(RED);
  mylcd.Set_Text_Size(5);  
  mylcd.Print_String("Vstup", 100, 56); 

  mylcd.Set_Text_Size(4);
  mylcd.Print_String("zamietnuty", 50, 96);

  delay(2000);
  finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
  delay(1000);

   mylcd.Fill_Screen(BLACK);
  mylcd.Set_Text_colour(WHITE);
  mylcd.Set_Text_Size(4);  
  mylcd.Print_String("Prilozte", 70, 66); 
  mylcd.Set_Text_Size(4);
  mylcd.Print_String("prst", 120, 106);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // Našlo zhodu
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  digitalWrite(14, HIGH);
  finger.LEDcontrol(FINGERPRINT_LED_BREATHING, 100, FINGERPRINT_LED_BLUE);

  mylcd.Set_Rotation(1);
  mylcd.Fill_Screen(BLACK);  
  mylcd.Set_Text_Mode(0);
  
  mylcd.Fill_Screen(BLACK);  
  mylcd.Set_Text_Back_colour(BLACK);

  mylcd.Set_Text_colour(GREEN);
  mylcd.Set_Text_Size(5);  
  mylcd.Print_String("Vstup", 80, 56); 

  mylcd.Set_Text_Size(4);
  mylcd.Print_String("povoleny", 60, 96);

  delay(2000);
  finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_BLUE);
  delay(1000);
  digitalWrite(14,LOW);
  mylcd.Fill_Screen(BLACK);
  mylcd.Set_Text_colour(WHITE);
  mylcd.Set_Text_Size(4);  
  mylcd.Print_String("Prilozte", 70, 66); 

  mylcd.Set_Text_Size(4);
  mylcd.Print_String("prst", 120, 106); 
  return finger.fingerID;

}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  digitalWrite(14, HIGH);
  finger.LEDcontrol(FINGERPRINT_LED_BREATHING, 100, FINGERPRINT_LED_BLUE);
  delay(2000);
  finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_BLUE);
  return finger.fingerID;
}
