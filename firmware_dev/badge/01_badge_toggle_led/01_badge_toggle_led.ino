/**
 * @file badge_blink_local.ino
 * @brief BLE badge with simple LED blink, no global variables (Adafruit nRF52)
 */

 #include <Arduino.h>
 #include <bluefruit.h>
 
 #define BADGE_NAME "ETWOW-BADGE"
 #define ADV_INTERVAL 1000      // BLE advertising interval in ms
 #define BLINK_INTERVAL 500     // LED blink interval in ms
 
 
 // --- Function to toggle LED using static local state ---
 void toggleLED() {
     static bool ledState = false;
     static unsigned long lastToggle = 0;
 
     unsigned long now = millis();
     if (now - lastToggle >= BLINK_INTERVAL) {
         ledState = !ledState;
         digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
         lastToggle = now;
     }
 }
 
 void setup() {
     // Initialize BLE
     Bluefruit.begin();
     Bluefruit.setName(BADGE_NAME);
     Bluefruit.Advertising.addName();
     Bluefruit.Advertising.setInterval(ADV_INTERVAL, ADV_INTERVAL);
     Bluefruit.Advertising.start();
 
     // LED setup
     pinMode(LED_BUILTIN, OUTPUT);
     digitalWrite(LED_BUILTIN, LOW);
 }
 
 void loop() {
     toggleLED();  // blink LED
     delay(1);
 }
 