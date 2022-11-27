#include <Adafruit_Fingerprint.h>

#include <Servo.h>


#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)

SoftwareSerial mySerial(2, 3);
#else
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


Servo myservo;

int retroEclairage = 5;

bool ledState = LOW;
bool serrureState; //HIGH = ouvert       LOW = Fermé 
unsigned long retroEclairageMillis = 0; 
unsigned long serrureMillis = 0; 
const long interval = 5000; 
const long intervalSerrure = 10000; 
unsigned long currentMillis;


uint8_t eAigu[8] = {130,132,142,145,159,144,142,128};
uint8_t eGrave[8] = {136,132,142,145,159,144,142,128};

void setup() {
 
  Serial.begin(9600);

  
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  finger.begin(57600);
 
  pinMode(retroEclairage, OUTPUT);

  

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  Serial.println("Bienvenue !");
  myservo.attach(8);
  myservo.write(80);
  serrureState = LOW;
  delay(500);  
  myservo.detach();
}



void loop() {

  
  currentMillis = millis();
  if (currentMillis - retroEclairageMillis >= interval) {
    if (ledState == HIGH) {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(retroEclairage, ledState);
  }

  if (currentMillis - serrureMillis >= intervalSerrure) {
    if (serrureState == HIGH) {
      myservo.attach(8);
      myservo.write(80);
      serrureState = LOW;
      delay(500);  
      myservo.detach();
    }
  }
  
 

 
   // On regarde si une empreinte correspond
  getFingerprintID();
  delay(50); 

}




uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      digitalWrite(retroEclairage, HIGH);
      retroEclairageMillis = currentMillis;
      finger.getTemplateCount();
      Serial.print("Nombre d'empreintes : "); 
      Serial.println(finger.templateCount);
      finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_PURPLE);
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println("No finger detected");
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
    Serial.println("Found a print match!");
    finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_BLUE);
   
    //delay(700);
    myservo.attach(8);
    myservo.write(0);
    delay(500); 
    myservo.detach();
    serrureState = HIGH;
    serrureMillis = currentMillis;
    finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_BLUE);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Accès refusé");
    finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
    
  
    delay(700);
    finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
    delay(1000);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
 {  
  
    
  
  
  return finger.fingerID;
}
}
