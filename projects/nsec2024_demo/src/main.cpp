#include <stdio.h>
#include <Arduino.h>

#error "damn"
void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("Hello, world!");
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
}