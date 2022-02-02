#include <LiquidCrystal.h>
#include <Wire.h>
#include <LIDARLite.h>
#include <math.h>

LIDARLite myLidar;

LiquidCrystal lcd(37, 36, 35, 34, 33, 32);

//joystick pin
int xPin = A0;
int yPin = A1;
int swPin = 18;

//lidar measurements
int lidarMeasure = 0;

int buttonState = 0;
bool taken = false;

void setup(){
  pinMode(swPin, INPUT);
  Serial.begin(9600);
  Serial.println("Lidar booting up");

  lcd.begin(20, 4);
  lcd.setCursor(4, 0);
  lcd.print("Distance ");
       //01234567890123456789
  lcd.setCursor(0, 2);
  lcd.print("20    40    60    80");
           //01234567890123456789

  myLidar.begin(0, true); 
  //Default configuration(0) and I2C frequency to 400kHz(true = 400/false = 100)

  myLidar.configure(1); 
  //Better accuracy close range
}

void loop(){
  /*
   * For lidar setting 
   */
  
  int lidarValueTrue = myLidar.distance(true);
  //True = taking error into consideration(recaliberation)
  //Every 100 measures should have a caliberation
  //Default setting = false
  if (lidarMeasure == 0){
    showInfo(lidarValueTrue);  
  } else {
    //Continue printing value but without caliberation
    showInfo(myLidar.distance());
    if (lidarMeasure == 100) lidarMeasure = -1;
  }
  lidarMeasure++;

  /*
   * Motor controlled by joystick
   */
  double joystickX = analogRead(xPin);
  double joystickY = 1023 - analogRead(yPin);
  motor(joystickX, joystickY);

  /*
   * Calculate volume and control bot using serial;
   */
  serialControl();
}

void serial_control(){
  if (Serial.available() > 0){
    String message = Serial.readStringUntil('\n');
    String stat;
    int colon, value; 
    int movePos = message.indexOf("Move");
    int dirPos = message.indexOf("Turn");
    int lcdPos = message.indexOf("LCD");
    int printPos = message.indexOf("Print");
    int area = message.indexOf("Area");
    int volume = message.indexOf("Volume");

    if (area > -1){
      int x, y;
      Serial.println("Keep the robot in one direction then wait for 3 seconds");
      delay(3000);
      x = myLidar.distance(true);
      Serial.println("Keep the robot in another direction then wait for 3 seconds");
      delay(3000);
      y = myLidar.distance(true);
      x = x * y;
      lcd.setCursor(0, 3);
      lcd.print("Area: ");      lcd.print(x);
    }

    if (volume > -1){
      int x, y, z;
      Serial.println("Keep the robot in one direction then wait for 3 seconds");
      delay(3000);
      x = myLidar.distance(true);
      Serial.println("Keep the robot in another direction then wait for 3 seconds");
      delay(3000);
      y = myLidar.distance(true);
      Serial.println("Keep the robot in another direction then wait for 3 seconds");
      delay(3000);
      z = myLidar.distance(true);
      x = x * y * z;
      lcd.setCursor(0, 3);
      lcd.print("Volume: ");      lcd.print(x);
    }
    
    if (movePos > -1){
      colon = message.indexOf(":");
      if (colon > -1){
        stat = message.substring(colon + 1);
        move_distance = stat.toInt();
        gobackward = true;
        buttonPress = true;
        Serial.print("Move :");  Serial.println(move_distance);
      }
    }

    if (dirPos > -1){
      colon = message.indexOf(":");
      if (colon > -1){
        stat = message.substring(colon + 1);
        value = stat.toInt();
        Serial.print("Turn: "); Serial.println(value);
        turnBool = true;
        wheel_control(value);        
      }
    }

    if (lcdPos > -1){
      colon = message.indexOf(":");
      if (colon > -1){
        stat = message.substring(colon + 1);
        lcd.setCursor(0,0);
        lcd.print("                    ");
        lcd.setCursor(0,0);
        lcd.print(stat);  
        Serial.println("Print to LCD");
      }
    }

    if (printPos > -1){
      colon = message.indexOf(":");
      if (colon > -1){
        stat = message.substring(colon + 1);
        Serial.print("Print = ");  Serial.println(stat);  
      }
    }

  }  
}

void showInfo(int lidar){
  //Print to Serial
  //Serial.print("Distance: ");
  //Serial.println(lidar);

  //Print to LCD with a bar to determine the distance
  //Print raw value to LCD
  lcd.setCursor(13, 0);
  lcd.print(lidar);
  lcd.print("  ");
  //Print the asterisk based on a metric
  int spaceAmount = 0;
  //lcd.setCursor(0, 0);
  //lcd.print("                    ");
  lcd.setCursor(0, 1);
  lcd.print("                    ");
           //01234567890123456789
  
  if (lidar <= 20){
    spaceAmount = 0;
  } else 
  if (lidar >= 80){
    spaceAmount = 19;
  } else {
    //18 positions for 59 spaces (21 to 79)
    spaceAmount = round(0.29 * lidar - 6.16);
  }

  lcd.setCursor(spaceAmount, 1);
  lcd.print("*");
}

/*
 * Controlling using joystick
 */
#define Dir_left    7
#define Dir_right   8
#define Power_left  9
#define Power_right 10
#define forw LOW
#define back HIGH

void motor(double x, double y){ 
  //control bot using joystick

  //convert analog value from joystick to motor speed value(0-255)
  //7 and 8 is direction
  //9 and 10 is power
  double power_L = x * 254 / 1023 - 127;
  double power_R = - power_L;
  double power_Y = y * 254 / 1023 - 127;
  //maths to change determined whether the car is shifting to left or right
  double left = power_L + power_Y;
  double right = power_R + power_Y;  

  if (left > 0){
    digitalWrite(Dir_left, forw);
  }
  if (right > 0){
    digitalWrite(Dir_right, forw);
  }
  if (left < 0){
    digitalWrite(Dir_left, back);
    left = -left;
  }
  if (right < 0){
    digitalWrite(Dir_right, back);
    right = -right;
  }
  analogWrite(Power_left, left);
  analogWrite(Power_right, right);
}

int button(int &x, int &y, int &z){
  int distance = myLidar.distance(true);
  switch (buttonState)
  {
  case 0:
    x = distance;
    buttonState = 1;
    break;
  case 1:
    y = distance;
    buttonState = 2;
    break;
  case 2:
    z = distance;
    buttonState = 0;
    taken == true;
  default:
    break;
  }
}

void dimension(){
  int x, y, z;
  int pullup = digitalRead(swPin);
  if (pullup == HIGH) button(x,y,z);
  if (taken == true){
    x = x*y*z;
    taken = false;
  }
  Serial.println(x);
  lcd.setCursor(0,3);
  lcd.print(x);
  lcd.print("    ");
}
