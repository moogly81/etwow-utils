/**
 * @file badge.ino
 * @brief Simple BLE badge using Adafruit Bluefruit
 */

 #include <Arduino.h>
 #include <bluefruit.h>
 
 // Configuration
 #define BADGE_NAME "ETWOW-BADGE"
 #define ADV_INTERVAL 1000  // Advertising interval in ms
 
 // --- Helper: print MAC address ---
 void printMac(const ble_gap_addr_t& addr) {
     for (int i = 0; i < 6; i++) {
         if (addr.addr[i] < 16) Serial.print("0");
         Serial.print(addr.addr[i], HEX);
         if (i < 5) Serial.print(":");
     }
     Serial.println();
 }
 
 void setup() {
     Serial.begin(115200);
     delay(10); // short delay to ensure Serial is ready
 
     Serial.println("=== Etwow Badge ===");
 
     // Initialize Bluefruit
     Bluefruit.begin();
     Bluefruit.setName(BADGE_NAME);
 
     // Simple advertising setup
     Bluefruit.Advertising.addName();
     Bluefruit.Advertising.setInterval(ADV_INTERVAL, ADV_INTERVAL);
     Bluefruit.Advertising.start();
 
     // Print MAC
     Serial.println("Badge advertising...");
     Serial.print("MAC: ");
     printMac(Bluefruit.getAddr());
 
     // Optional: turn on built-in LED to indicate badge is running
     pinMode(LED_BUILTIN, OUTPUT);
     digitalWrite(LED_BUILTIN, HIGH);
 }
 
 void loop() {
     // Nothing needed in loop for simple advertising
     delay(1000);
 }
 