void setupUpEEPROM(){
  Serial.println("------------------------------------------------------");
  Serial.println("Removing all existing RFID UID's                      ");
  Serial.println("By setting every address to 0                         ");
  Serial.println("Set EEPROMSize is:                                    ");
  Serial.println(EEPROMSIZE);
  Serial.println("------------------------------------------------------");
  byte zero  = 0;
  for(int adr = MEMBASE; adr <= EEPROMSIZE; adr++) {
    EEPROM.write(adr, zero);
  }
}


// Display the contents of the EEPROM in the serial monitor
void displayEEPROM(){
  Serial.println("-------------EEPROM MEMORY---------------");
  Serial.println("");
  for(int adr = MEMBASE; adr <= EEPROMSIZE; adr++) {
    byte value = EEPROM.read(adr);
    Serial.print(value,HEX);
    Serial.print("|");
    if(adr % 4 == 0) Serial.println("");
  }
  Serial.println("");
}

//Check two byte arrays
boolean compareByteArray(byte a[], byte b[]){
  for(int i = 0; i < 4; i++){
    if(a[i] != b[i]){
      return false; //If one isn't equal, then are the ID not equel
    }
  }
  return true;
}

void checkMaster(){
  //Read the first 4 bytes after adress 0 (cardAmount) of memory (masterCard location), if empty mastercard should be set
  boolean masterCardIsDefined = true;
  for(int adr = MEMBASE + 1; adr < MEMBASE+5; adr++){
    if(EEPROM.read(adr) == 0){
      masterCardIsDefined = false;
    }
  }
  if(!masterCardIsDefined){
    Serial.println("No master card has been set!");
    Serial.println("Scan card to be set as masterCard");
    blickColor("purple", 100, 5, "purple");
    do{
      successfulRead = gotNewUID();
    } while (!successfulRead);
    int cardIndex = 0;
    for(int adr = MEMBASE + 1; adr < MEMBASE+5; adr++){
      EEPROM.write(adr,readCard[cardIndex]);
      cardIndex++;
    }
    for(int i = 0; i < 4; i++){
      masterCard[i] = readCard[i]; //Caching masterCard
    }
    findLastAdress();   //Set the end of array after adding a card
  }else{
    //Mastercard is set, cache it for quick use
    int cardIndex = 0;
    for(int adr = MEMBASE+1; adr < MEMBASE+5; adr++){
      masterCard[cardIndex] = EEPROM.read(adr);
      cardIndex++;
    }
  }
}

//Checkes if a card is saved on the SD
void findLastAdress(){
  int amountOfCards = EEPROM.read(0);
  endOfCardMemory = 5 + amountOfCards * 4;
}
//Seaches for a card in the EEPROM
int findCard(byte cardToFind[4]){
  byte compareCard[4];
  int cardIndex = 0;
  for(int adr = MEMBASE+5; adr < endOfCardMemory; adr++){
    compareCard[cardIndex] = EEPROM.read(adr);
    if(cardIndex == 3){
      if(compareByteArray(cardToFind, compareCard)){
        return adr - 3;
      }else{
        cardIndex = 0;
      }
    }else{
      cardIndex++;
    }
  }
  return 0;
}

// Add a new file to SD card
void addCard(byte cardToAdd[4]){
  int addressToWrite;
  int hole = findAddressOfHole();
  if(hole != endOfCardMemory){
    addressToWrite = hole;
  }else{
    addressToWrite = endOfCardMemory;
  }
  int cardIndex = 0;
  Serial.println(addressToWrite);
  for(int adr = MEMBASE + addressToWrite; adr < MEMBASE + addressToWrite+4; adr++ ){
    EEPROM.write(adr,cardToAdd[cardIndex]);
    cardIndex++;
  }
  int amountOfCards = EEPROM.read(0);
  EEPROM.write(0,amountOfCards+1);
  findLastAdress();
  displayEEPROM();
}

// Remove a file/UID from SD card
void removeCard(byte cardToRemove[4]){
  int cardAdress = findCard(cardToRemove);
  if(cardAdress != 0){
    Serial.println("Found card, removing it!");
    for(int adr = cardAdress; adr < cardAdress+4; adr++){
      EEPROM.write(adr,0);
    }
    holesInEEPROM = true;
  }else{
    Serial.println("Card doesn't exist and can't be removed");
  }
  displayEEPROM();
}
//Searches for hole in the EEPROM to fill
int findAddressOfHole(){
  int holeSizeCounter = 0;
  for(int adr = MEMBASE+5; adr<endOfCardMemory; adr++){
      if(EEPROM.read(adr) == 0){
        holeSizeCounter++;
        if(holeSizeCounter == 4){
          boolean endOfMemory = true;
          for(int endingAdr = adr; endingAdr<endOfCardMemory; endingAdr++){
            if(EEPROM.read(endingAdr) != 0){
              endOfMemory = false;
            }
          }
          if(endOfMemory){
            Serial.println("Hole found at end");
            endOfCardMemory = adr;
            int amountOfCards = (adr - 5)/4;
            EEPROM.write(0,amountOfCards);  
            findLastAdress();
          }else{
            Serial.print("found Hole at:");
            Serial.println(adr-3);
            return adr - 3; 
          }
        }
      }
  }
  Serial.println("No holes found!");
  holesInEEPROM = false;
  return endOfCardMemory;
}

