#include <LiquidCrystal.h>
#include <Wire.h>
#include <LIDARLite.h>
#include <math.h>

LIDARLite myLidar;

LiquidCrystal lcd(37, 36, 35, 34, 33, 32);

void setup(){
  Serial.begin(9600);
  Serial.println("Lidar booting up");

  lcd.begin(20, 4);
  lcd.setCursor(6, 0);
  lcd.print("Distance ");
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
  showInfo(lidarValueTrue);
  //Continue printing value but without caliberation
  for (int i = 0; i < 100; i++){
    showInfo(myLidar.distance());
  }
}

void showInfo(int lidar){
  //Print to Serial
  Serial.print("Distance: ");
  Serial.println(lidar);

  //Print to LCD with a bar to determine the distance
  int spaceAmount = 0;
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
