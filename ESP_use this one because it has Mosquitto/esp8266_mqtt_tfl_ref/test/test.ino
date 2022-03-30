void setup(){
    pinMode(D5, OUTPUT);
    pinMode(D4, OUTPUT);
    pinMode(D0, OUTPUT);
}

void loop(){
    delay(1000);
    digitalWrite(D5, LOW);
    digitalWrite(D4, LOW);
    digitalWrite(D0, LOW);
    delay(1000);
}