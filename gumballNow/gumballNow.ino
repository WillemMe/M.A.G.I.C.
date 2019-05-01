#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include <Servo.h>
Servo rotary;

int pos = 0;

// Pins
#define SensorInterupt  2   // Needs to be an interrupt pin
#define rfidAdress 0x10

SparkFun_APDS9960 apds = SparkFun_APDS9960();
byte proximity_data = 0;
boolean interruptFlag = false;
boolean rfidConected = false;
boolean dispensing = false;
unsigned long startTimeout;
unsigned long currentTime;

void setup() {
  Serial.begin(9600);
  //I2C begin en check if rfid reader is available, else continue without
  Wire.begin();
  Wire.beginTransmission(rfidAdress);
  Wire.write(0);
  int error = Wire.endTransmission();
  if(error != 0){
    Serial.print("I2C error: ");
    Serial.println(error);
  }else{
    Serial.println("success found rfid reader");
    rfidConected = true;
  }

  rotary.attach(9);
  pinMode(SensorInterupt, INPUT);
  attachInterrupt(0, interruptRoutine, FALLING);
  sensorSetup();
}

void loop() {
  //Time out function for after dropping a ball
  currentTime = millis();
  if(dispensing && ( currentTime > (startTimeout + 1000))){
    rotary.write(0);
    dispensing = false;
    while(checkAccess()){};
  }
  if (interruptFlag) {
    if(rfidConected){
     if(checkAccess()){
      dropBall();
     } 
    }else{
      dropBall();
    }

    // Reset flag and clear APDS-9960 interrupt
    interruptFlag = false;
    if ( !apds.clearProximityInt() ) {
      Serial.println("Error clearing interrupt");
    }
  }
  delay(100);
}
//Used for interrupt
void interruptRoutine() {
  interruptFlag = true;
}

//Rotates the wheel dropping a ball
void dropBall() {
  if(!dispensing){
    rotary.write(172);
    dispensing = true;
    startTimeout = millis();
  }

}

boolean checkAccess(){
  Wire.requestFrom(rfidAdress,1);
  if(Wire.available()){
    byte result = Wire.read();
    if(result == 0xFF){
      return true;
    }else{
      return false;
    }
  }
}

