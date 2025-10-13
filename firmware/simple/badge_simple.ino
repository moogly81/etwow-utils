/**
 * @file badge_simple.ino
 * @brief Ultra-simple BLE badge using standard Arduino libraries
 * 
 * This is a much simpler version of the badge firmware that uses
 * standard Arduino BLE libraries instead of complex custom code.
 */

#include <Arduino.h>
#include <bluefruit.h>

// Configuration
#define BADGE_NAME "ETWOW-BADGE"
#define ADV_INTERVAL 1000  // Advertising interval in ms

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    
    Serial.println("=== Simple Etwow Badge ===");
    
    // Initialize Bluefruit
    Bluefruit.begin();
    Bluefruit.setName(BADGE_NAME);
    
    // Simple advertising setup
    Bluefruit.Advertising.addName();
    Bluefruit.Advertising.setInterval(ADV_INTERVAL, ADV_INTERVAL);
    Bluefruit.Advertising.start();
    
    Serial.println("Badge advertising...");
    Serial.printf("MAC: %s\n", Bluefruit.getAddr().toString().c_str());
}

void loop() {
    // Nothing needed in loop for simple advertising
    delay(1000);
}
