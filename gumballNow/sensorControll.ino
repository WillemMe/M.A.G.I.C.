// Constants
#define PROX_INT_HIGH   254 // Proximity level for interrupt
#define PROX_INT_LOW    0  // No far interrupt


void sensorSetup(){
  Serial.println("_____________Prox sensor___________");

  //Sensor setup
  if( apds.init()){
    Serial.println("Sensor initialization complete");
  } else {
    Serial.println("Something went wrong during sensor init!");
  }

  //Set gain high so it only looks close up
  if ( !apds.setProximityGain(PGAIN_8X) ) {
    Serial.println("Something went wrong trying to set PGAIN");
  }

    // Set proximity interrupt thresholds
  if ( !apds.setProximityIntLowThreshold(PROX_INT_LOW) ) {
    Serial.println(F("Error writing low threshold"));
  }
  if ( !apds.setProximityIntHighThreshold(PROX_INT_HIGH) ) {
    Serial.println(F("Error writing high threshold"));
  }

  //check if everthing works
  if ( apds.enableProximitySensor(true) ) {
    Serial.println("Proximity sensor is now running");
  } else {
    Serial.println("Something went wrong during sensor init!");
  }
}
