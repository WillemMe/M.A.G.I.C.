//_____________________ Functions for controling RFID module _____________________
boolean gotNewUID(){
  if( !rfid.PICC_IsNewCardPresent()){    //Only if a new carded is presented continue
    return false;
  }else if( ! rfid.PICC_ReadCardSerial()){      //Read the new card, if failed quit
    return false;
  }

  Serial.print(F("Card found, UID: "));
  displayReadCard();
  Serial.println("");     //Clear line
  rfid.PICC_HaltA();     // Stop reading
  return true;    //return successful reading of a card
}
//Display card in serial monitor
void displayReadCard(){
  for (int i=0 ; i<4; i++){     //Iterate for every part of the 4 block UID
    readCard[i] = rfid.uid.uidByte[i];     //Add block to read card
    Serial.print(readCard[i], HEX);     //Display block
    if(i < 3)Serial.print(":");
  }
}

