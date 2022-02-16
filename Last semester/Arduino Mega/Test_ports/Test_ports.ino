#include <LiquidCrystal.h>

LiquidCrystal lcd(37, 36, 35, 34, 33, 32);

void setup(){
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(2, INPUT);//direction
  pinMode(3, INPUT);//speed of left
  pinMode(22, INPUT);
  pinMode(24, INPUT);
  lcd.begin(20,4);
  Serial.begin(9600);
  Serial.println("Write something to the serial monitor.");
}

void loop(){
  if (Serial.available() > 0){
    String message = Serial.readStringUntil('\n'); 
    Serial.print("Message received, content: ");  
    Serial.println(message);
    int pos_s = message.indexOf("Print");

    if (pos_s > -1){
      Serial.println("Command = Print ");
      pos_s = message.indexOf(":");

      if (pos_s > -1){
        String stat = message.substring(pos_s + 1);
        if (stat == "Hi") {
          Serial.println("Hi!");
        }
        else if (stat == "Hello") {
          Serial.println("Hello there!");
        }
      }  
    }
    else{
      pos_s = message.indexOf("LCD");
      if (pos_s > -1){
        Serial.println("Command = Print to LCD screen");
        pos_s = message.indexOf(":");
        if (pos_s > -1){
          lcd.clear();
          String stat = message.substring(pos_s + 1);
          lcd.setCursor(0, 0);
          lcd.print(stat);
        }
      }
      else{
      Serial.println("No command found, try typing Print:Hi or Print:Hello or LCD\n");
      }
    }    
  }
}
