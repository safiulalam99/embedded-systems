#include <LiquidCrystal.h>
#include <math.h>

//pin 8,7,6,5,4,3 is assigned for lcd screen
LiquidCrystal lcd(8,7,6,5,4,3);

int count = 0;

void setup() {
  //set up lcd screen
  lcd.begin(20,4);
  
  //interrupt service routine
  pinMode(2, INPUT_PULLUP); //button is Digital 2
  attachInterrupt(digitalPinToInterrupt(2), isr, FALLING);
}

void loop() {

  button(); //display the number of time button is pressed
  
  xPos();
  //x-Position

  delay(200);
  lcd.clear();  
}

void button() {
  //if (digitalRead(2) == LOW){
  //  isr();
  //}
  
  lcd.setCursor(0,3);
  lcd.print("Pressed: ");
  lcd.print(count);
}

void isr() {
  count++;
}

void xPos() {
  lcd.setCursor(0,0);
  lcd.print("X-position");

  lcd.setCursor(0,1);
  lcd.print("-100%    0%    +100%");

  lcd.setCursor(0,2);
  int input=analogRead(A0); //X-axis is A0
  
  //convert Analog input to Voltage level
  double voltage = input*5.0/1023.0;
  
  //calculate where the "|" should be on the screen
  //filled the space before the "|" to push the symbol to the correct position
  int space = round((voltage*19/5)-0.5);
  for (int i = 0; i < space; i++) {
    lcd.print(" ");
  }

  //finally print the symbol after it is in the perfect position
  lcd.print("|");
}
