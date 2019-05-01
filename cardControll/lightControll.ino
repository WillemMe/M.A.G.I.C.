/*
  Color codes:
    red: 255, 0, 0
    green: 0, 255, 0
    blue: 0,0,255
    purple: 128, 16, 15
    orange: 244, 111, 17

*/
byte offCode[3] = {0,0,0};
byte colorCode[3];
//String to color code conversion
void getColorCode(String color){
  if(color == "red"){
    colorCode[0] = 255;
    colorCode[1] = 0;
    colorCode[2] = 0;
  }else if(color == "green"){
    colorCode[0] = 0;
    colorCode[1] = 255;
    colorCode[2] = 0;
  }else if(color == "blue"){
    colorCode[0] = 0;
    colorCode[1] = 0;
    colorCode[2] = 255;
  }else if(color == "purple"){
    colorCode[0] = 128;
    colorCode[1] = 16;
    colorCode[2] = 15;
  }
}
//Set to color of the Led by given 3 byte values and a brightness
void setColor(byte colorCode[3], int brightness){
  byte red = colorCode[0] * (brightness/100);
  byte green = colorCode[1] * (brightness/100);
  byte blue = colorCode[2] * (brightness/100);

  analogWrite(REDPIN, red);
  analogWrite(GREENPIN, green);
  analogWrite(BLUEPIN, blue);
}

//Change to color to make a blink effect given the color as a string, and an ending color to resume one
void blickColor(String color, int brightness, int blickTimes, String endColor){
  getColorCode(color);
  for(int i = 0; i<blickTimes; i++){
    setColor(offCode,0);
    delay(200);
    setColor(colorCode,brightness);
    delay(200);
  }
  getColorCode(endColor);
  setColor(colorCode,100);
}
