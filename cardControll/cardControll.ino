#include <SPI.h>          // RC522 Module over the SPI protocol
#include <MFRC522.h>      // Library for Mifare RC522
#include <EEPROM.h>       // Library for EEPROM editing
#include <Wire.h>         //I2C library

#include "SETTINGS.h"     // Import settings

//Global var initiation
boolean match = false;      //If card is in system
boolean programingMode = false;     //If enabled cards can be added/removed
boolean successfulRead; //reader succesfully reads

byte readCard[4];     //Stores currently read card
byte masterCard[4];  //Stores/Caches mastercard
int endOfCardMemory = 0;  //Stores last address, for efficiency
boolean holesInEEPROM = true;
boolean access = false;
unsigned long startTimeout;
unsigned long currentTime;


MFRC522 rfid(MRC522_SS_PIN, MRC522_RST_PIN);      //Make instance of MRC522 module


void setup() {
  //Start serial for debuging purposes
  Wire.begin(0x10);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
  Serial.println("___________Card control V0.5___________");

  //Start SPI comunication
  SPI.begin();
  rfid.PCD_Init();     //Initialize MFRC522

  //Set up rgb led
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  pinMode(PROGRAMPIN, INPUT_PULLUP);

  //Check if button is pressed, for reseting
  if (digitalRead(PROGRAMPIN) == LOW) {
    Serial.println("Formating EEPROM");
    blickColor("purple", 100, 3, "blue");
    setupUpEEPROM();
  }
  checkMaster(); // Check if a mastercard record is present, else generate one.
  findLastAdress(); // Check EEPROM
  displayEEPROM(); // Display EEPROM in serial monitor

  Serial.println("Everything is ready!");
  Serial.println("Waiting for card");
  blickColor("blue", 100, 2, "blue");
  findAddressOfHole();
  findLastAdress();
}

void loop() {
  do {
    if(access){
      // Time out for access
      currentTime = millis();
      if(startTimeout + 4000 < currentTime){
        access = false;
        blickColor("blue",100,0,"blue");
      }
    }
    successfulRead = gotNewUID();
    if (digitalRead(PROGRAMPIN) == LOW) {
      displayEEPROM();
    }
    delay(100);
  } while (!successfulRead);
  if (compareByteArray(readCard, masterCard)) { // Check if it is the masterCard
    Serial.println("Found the almighty masterCard");
    if (!programingMode) {
      Serial.println("Going into programing mode");
      blickColor("green", 100, 1, "purple");
      programingMode = true;
    } else {
      Serial.println("Existing programing mode");
      blickColor("red", 100, 1, "blue");
      programingMode = false;
    }
  } else {
    if (!programingMode) {
      if (findCard(readCard) != 0) {
        Serial.println("Known card!");
        access = true;
        startTimeout = millis();
        blickColor("green", 100, 2, "green");
      } else {
        Serial.println("Unknow card!");
        access = false;
        blickColor("red", 100, 2, "blue");
      }
    } else { // In programing mode
      if (findCard(readCard) == 0) {
        blickColor("green", 100, 2, "purple");
        Serial.println("Programing mode adding card");
        addCard(readCard);
      } else {
        blickColor("red", 100, 2, "purple");
        Serial.println("Programing mode removing card");
        removeCard(readCard);
      }
    }
  }
}
// Connection with Gumballmachine to grand access
void requestEvent(int bytes){
   byte respond;
    if(access){
      respond = 0xFF;
    }else{
      respond = 0x00;
    }
    Wire.write(respond);  
}
