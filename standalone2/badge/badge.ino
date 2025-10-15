#include <Arduino.h>
#include <bluefruit.h>

#define LED_PIN        LED_BUILTIN
#define BLINK_INTERVAL 500
#define ADV_NAME       "ETWOW-BADGE"

// --- LED toggle function ---
void toggleLED() {
    static bool ledState = false;
    static unsigned long lastToggle = 0;

    unsigned long now = millis();
    if (now - lastToggle >= BLINK_INTERVAL) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
        lastToggle = now;
    }
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Initialize BLE
    Bluefruit.begin();
    Bluefruit.setName(ADV_NAME);
    Bluefruit.Advertising.addName();
    Bluefruit.Advertising.setInterval(1000, 1000);
    Bluefruit.Advertising.start();

    Serial.begin(115200);
    Serial.println("BLE advertising started...");
}

void loop() {
    toggleLED();
    delay(1);
}
