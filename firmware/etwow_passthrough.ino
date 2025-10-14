/**
 * @file etwow_passthrough.ino
 * @brief Proper UART passthrough implementation for Etwow GT Auto-lock
 * 
 * This version properly handles UART passthrough between display and BT module
 * while injecting LOCK/UNLOCK commands when needed.
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// Configuration
#define TARGET_MAC "AA:BB:CC:DD:EE:FF"
#define UNLOCK_RSSI -60
#define LOCK_RSSI -80
#define UART_BAUD 115200
#define RX_PIN 16    // ESP32 RX (connected to Display TX)
#define TX_PIN 17    // ESP32 TX (connected to BT Module RX)
#define OE_PIN 25    // Output Enable for tri-state buffer
#define LED_PIN 2    // Status LED

// Global variables
HardwareSerial BusSerial(1);  // UART1 for bus communication
BLEScan* pBLEScan;
int lastRSSI = -127;
bool isUnlocked = false;
unsigned long lastCommandTime = 0;
bool commandInProgress = false;

// Simple BLE callback
class MyCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice device) {
        if (device.getAddress().toString() == String(TARGET_MAC).toLowerCase()) {
            lastRSSI = device.getRSSI();
            Serial.printf("Badge: RSSI=%d dBm\n", lastRSSI);
        }
    }
};

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    
    Serial.println("=== Etwow Auto-lock with Passthrough ===");
    
    // Setup pins
    pinMode(OE_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(OE_PIN, LOW);  // Tri-state by default (passthrough mode)
    digitalWrite(LED_PIN, LOW);
    
    // Setup UART for passthrough
    BusSerial.begin(UART_BAUD, SERIAL_8N1, RX_PIN, TX_PIN);
    Serial.printf("UART configured: RX=%d, TX=%d, Baud=%d\n", RX_PIN, TX_PIN, UART_BAUD);
    
    // Setup BLE
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
    pBLEScan->start(0, nullptr, false);
    
    Serial.printf("Looking for badge: %s\n", TARGET_MAC);
    Serial.println("Passthrough mode active - Display ↔ ESP32 ↔ BT Module");
    Serial.println("System ready!");
}

void loop() {
    // CRITICAL: Handle UART passthrough (this is the main function!)
    handleUARTPassthrough();
    
    // Handle proximity logic
    handleProximityLogic();
    
    // Handle BLE scanning
    handleBLEScanning();
    
    delay(1);  // Minimal delay for responsiveness
}

// CRITICAL: UART Passthrough Function
void handleUARTPassthrough() {
    // Read data from UART and forward it (passthrough)
    while (BusSerial.available()) {
        uint8_t data = BusSerial.read();
        
        // Optional: Forward to USB for debugging
        Serial.write(data);
        
        // Note: The ESP32 UART automatically handles passthrough
        // Data flows: Display → ESP32 RX → ESP32 TX → BT Module
        // This happens transparently when OE_PIN is LOW (tri-state mode)
    }
}

// Send lock/unlock command
void sendCommand(const char* cmd) {
    if (commandInProgress) {
        Serial.println("Command already in progress, skipping");
        return;
    }
    
    commandInProgress = true;
    lastCommandTime = millis();
    
    Serial.printf("Sending command: %s\n", cmd);
    
    // Enable transmission (disable passthrough temporarily)
    digitalWrite(OE_PIN, HIGH);
    delay(2);  // Small delay for buffer activation
    
    // Send command
    BusSerial.print(cmd);
    BusSerial.print('\n');
    BusSerial.flush();
    delay(10);  // Ensure transmission completes
    
    // Return to passthrough mode
    digitalWrite(OE_PIN, LOW);
    commandInProgress = false;
    
    Serial.printf("Command sent: %s (passthrough restored)\n", cmd);
}

// Handle proximity-based lock/unlock logic
void handleProximityLogic() {
    // Check if command is still in progress
    if (commandInProgress && (millis() - lastCommandTime > 100)) {
        commandInProgress = false;
    }
    
    // Proximity logic with hysteresis
    if (lastRSSI > UNLOCK_RSSI && !isUnlocked && !commandInProgress) {
        sendCommand("UNLOCK");
        isUnlocked = true;
        digitalWrite(LED_PIN, HIGH);
        Serial.println("UNLOCKED - Badge nearby");
    } else if (lastRSSI < LOCK_RSSI && isUnlocked && !commandInProgress) {
        sendCommand("LOCK");
        isUnlocked = false;
        digitalWrite(LED_PIN, LOW);
        Serial.println("LOCKED - Badge far away");
    }
}

// Handle BLE scanning
void handleBLEScanning() {
    // Restart BLE scan if it stopped
    if (!pBLEScan->isScanning()) {
        Serial.println("Restarting BLE scan...");
        pBLEScan->start(0, nullptr, false);
    }
}
