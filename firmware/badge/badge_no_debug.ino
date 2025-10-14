/**
 * @file badge_minimal.ino
 * @brief Minimal BLE badge (Adafruit nRF52) without debug prints
 */

 #include <Arduino.h>
 #include <bluefruit.h>
 
 #define BADGE_NAME "ETWOW-BADGE"
 #define ADV_INTERVAL 1000  // Advertising interval in ms
 
 void setup() {
     // Initialize Bluefruit BLE
     Bluefruit.begin();
     Bluefruit.setName(BADGE_NAME);
 
     // Simple advertising setup
     Bluefruit.Advertising.addName();
     Bluefruit.Advertising.setInterval(ADV_INTERVAL, ADV_INTERVAL);
     Bluefruit.Advertising.start();
 
     // Optional: turn on built-in LED to indicate badge is running
     pinMode(LED_BUILTIN, OUTPUT);
     digitalWrite(LED_BUILTIN, HIGH);
 }
 
 void loop() {
     // Nothing needed in loop for minimal advertising
     delay(1000);
 }
 