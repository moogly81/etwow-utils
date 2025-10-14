/**
 * @file etwow.ino
 * @brief Etwow GT Auto-lock using standard Arduino BLE libraries
 *
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// Configuration - Update these for your setup
#define TARGET_MAC "AA:BB:CC:DD:EE:FF"  // Your badge MAC address
#define UNLOCK_RSSI -60                 // Unlock when closer than this
#define LOCK_RSSI -80                   // Lock when farther than this
#define UART_BAUD 115200                // UART baud rate
#define RX_PIN 16                       // UART RX pin
#define TX_PIN 17                       // UART TX pin
#define OE_PIN 25                       // Output enable pin
#define LED_PIN 2                       // Status LED pin

// Global variables
HardwareSerial BusSerial(1);
BLEScan* pBLEScan;
int lastRSSI = -127;
bool isUnlocked = false;
unsigned long lastDecisionTime = 0;

// Simple BLE callback class
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        // Check if this is our target device
        if (advertisedDevice.getAddress().toString() == String(TARGET_MAC).toLowerCase()) {
            lastRSSI = advertisedDevice.getRSSI();
            Serial.printf("Badge found! RSSI: %d dBm\n", lastRSSI);
        }
    }
};

// Simple function to send lock/unlock command
void sendCommand(const char* cmd) {
    digitalWrite(OE_PIN, HIGH);  // Enable transmission
    delay(2);
    BusSerial.print(cmd);
    BusSerial.print('\n');
    BusSerial.flush();
    delay(10);
    digitalWrite(OE_PIN, LOW);   // Disable transmission
    Serial.printf("Sent: %s\n", cmd);
}

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    
    Serial.println("=== Simple Etwow Auto-lock ===");
    
    // Setup pins
    pinMode(OE_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(OE_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    
    // Setup UART
    BusSerial.begin(UART_BAUD, SERIAL_8N1, RX_PIN, TX_PIN);
    
    // Setup BLE
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
    pBLEScan->start(0, nullptr, false);  // Continuous scanning
    
    Serial.printf("Looking for badge: %s\n", TARGET_MAC);
    Serial.println("System ready!");
}

void loop() {
    // Simple proximity logic with hysteresis
    if (lastRSSI > UNLOCK_RSSI && !isUnlocked) {
        sendCommand("UNLOCK");
        isUnlocked = true;
        digitalWrite(LED_PIN, HIGH);
        Serial.println("UNLOCKED");
    } else if (lastRSSI < LOCK_RSSI && isUnlocked) {
        sendCommand("LOCK");
        isUnlocked = false;
        digitalWrite(LED_PIN, LOW);
        Serial.println("LOCKED");
    }
    
    // Forward UART traffic to USB for debugging
    while (BusSerial.available()) {
        Serial.write(BusSerial.read());
    }
    
    // Restart BLE scan if it stopped
    if (!pBLEScan->isScanning()) {
        Serial.println("Restarting BLE scan...");
        pBLEScan->start(0, nullptr, false);
    }
    
    delay(10);
}
