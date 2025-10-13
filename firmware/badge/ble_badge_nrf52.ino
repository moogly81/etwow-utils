/**
 * @file ble_badge_nrf52.ino
 * @brief BLE Badge firmware for Etwow GT Auto-lock System
 * @author Etwow Utils Project
 * @version 1.0.0
 * 
 * This firmware implements a low-power BLE beacon that continuously advertises
 * its presence for proximity detection by the ESP32 controller.
 * 
 * Features:
 * - Continuous BLE advertising
 * - Low power consumption (1+ year on CR2032)
 * - Unique MAC address identification
 * - Battery level monitoring
 * - Configurable advertising interval
 */

#include <bluefruit.h>

// Configuration
#define BADGE_NAME "ETWOW-BADGE"
#define ADV_INTERVAL_MS 1000        // Advertising interval in milliseconds
#define BATTERY_CHECK_INTERVAL 60000 // Check battery every 60 seconds
#define LOW_BATTERY_THRESHOLD 2800  // Low battery threshold in mV
#define MAC_ADDRESS_SIZE 6          // MAC address size in bytes

// Pin definitions (adjust for your hardware)
#define LED_PIN LED_RED             // Status LED pin
#define BATTERY_PIN PIN_A0          // Battery voltage measurement pin

// Global variables
unsigned long lastBatteryCheck = 0;
unsigned long lastHeartbeat = 0;
bool lowBatteryWarning = false;
uint8_t macAddress[MAC_ADDRESS_SIZE];
char macAddressString[18]; // "AA:BB:CC:DD:EE:FF\0"

/**
 * @brief Initialize the badge system
 */
void setup() {
    // Initialize serial for debugging
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    
    Serial.println("=== Etwow GT BLE Badge ===");
    Serial.println("Initializing badge...");
    
    // Initialize pins
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    // Initialize Bluefruit
    Bluefruit.begin();
    Bluefruit.setName(BADGE_NAME);
    
    // Get and display MAC address
    getMACAddress();
    Serial.printf("Badge MAC Address: %s\n", macAddressString);
    
    // Configure advertising
    setupAdvertising();
    
    // Start advertising
    Bluefruit.Advertising.start();
    
    // Initial battery check
    checkBattery();
    
    Serial.println("Badge initialized successfully!");
    Serial.printf("Advertising interval: %d ms\n", ADV_INTERVAL_MS);
    Serial.println("Badge is now advertising...");
    
    // Blink LED to indicate startup
    blinkLED(3, 200);
}

/**
 * @brief Main loop
 */
void loop() {
    unsigned long currentTime = millis();
    
    // Check battery level periodically
    if (currentTime - lastBatteryCheck > BATTERY_CHECK_INTERVAL) {
        checkBattery();
        lastBatteryCheck = currentTime;
    }
    
    // Heartbeat for debugging
    if (currentTime - lastHeartbeat > 30000) { // Every 30 seconds
        lastHeartbeat = currentTime;
        Serial.printf("Badge running - Uptime: %lu ms, Battery: %s\n", 
                     currentTime, lowBatteryWarning ? "LOW" : "OK");
    }
    
    // Low battery warning LED blink
    if (lowBatteryWarning && (currentTime % 5000 < 100)) {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
    
    // Small delay to reduce power consumption
    delay(100);
}

/**
 * @brief Setup BLE advertising
 */
void setupAdvertising() {
    // Create advertising data
    Bluefruit.Advertising.clearData();
    
    // Add device name
    Bluefruit.Advertising.addName();
    
    // Add custom data (badge identifier)
    uint8_t customData[] = {'E', 'T', 'W', 'O', 'W', '_', 'B', 'A', 'D', 'G', 'E'};
    Bluefruit.Advertising.addData(BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, 
                                 customData, sizeof(customData));
    
    // Configure advertising parameters
    Bluefruit.Advertising.setInterval(ADV_INTERVAL_MS, ADV_INTERVAL_MS);
    Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_ADV_NONCONN_IND);
    Bluefruit.Advertising.setChannelMap(BLE_GAP_ADV_CHANNEL_MAP_ALL);
    Bluefruit.Advertising.setFilter(BLE_GAP_ADV_FP_ANY);
}

/**
 * @brief Get MAC address and format as string
 */
void getMACAddress() {
    // Get MAC address from Bluefruit
    uint8_t mac[6];
    Bluefruit.getAddr(mac);
    
    // Copy to global array
    memcpy(macAddress, mac, MAC_ADDRESS_SIZE);
    
    // Format as string
    sprintf(macAddressString, "%02X:%02X:%02X:%02X:%02X:%02X",
            macAddress[5], macAddress[4], macAddress[3],
            macAddress[2], macAddress[1], macAddress[0]);
}

/**
 * @brief Check battery voltage
 */
void checkBattery() {
    // Read battery voltage (assuming voltage divider)
    int rawValue = analogRead(BATTERY_PIN);
    float voltage = (rawValue * 3.3 * 2.0) / 1024.0; // Adjust multiplier for your voltage divider
    
    // Check for low battery
    bool wasLowBattery = lowBatteryWarning;
    lowBatteryWarning = (voltage < LOW_BATTERY_THRESHOLD);
    
    // Print battery status
    Serial.printf("Battery voltage: %.2fV (%s)\n", voltage, 
                 lowBatteryWarning ? "LOW" : "OK");
    
    // Warning if battery just went low
    if (lowBatteryWarning && !wasLowBattery) {
        Serial.println("WARNING: Low battery detected!");
        blinkLED(5, 100); // Rapid blink warning
    }
}

/**
 * @brief Blink LED pattern
 * @param count Number of blinks
 * @param delayMs Delay between blinks in milliseconds
 */
void blinkLED(int count, int delayMs) {
    for (int i = 0; i < count; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(delayMs);
        digitalWrite(LED_PIN, LOW);
        if (i < count - 1) {
            delay(delayMs);
        }
    }
}

/**
 * @brief Get MAC address as string (for external use)
 * @return MAC address string
 */
const char* getMACAddressString() {
    return macAddressString;
}

/**
 * @brief Check if battery is low
 * @return true if battery is low
 */
bool isBatteryLow() {
    return lowBatteryWarning;
}

/**
 * @brief Get current battery voltage
 * @return Battery voltage in volts
 */
float getBatteryVoltage() {
    int rawValue = analogRead(BATTERY_PIN);
    return (rawValue * 3.3 * 2.0) / 1024.0; // Adjust multiplier for your voltage divider
}
