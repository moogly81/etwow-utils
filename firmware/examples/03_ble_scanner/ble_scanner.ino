/**
 * @file ble_scanner.ino
 * @brief BLE Scanner Example - Test BLE scanning and RSSI measurement
 * 
 * This example demonstrates how to scan for BLE devices and measure RSSI
 * for proximity detection testing.
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "config.h"

// BLE scan configuration
BLEScan* pBLEScan;
String targetMAC = TARGET_BADGE_MAC;
int lastRSSI = RSSI_MIN_VALID;
bool targetFound = false;
unsigned long lastScanTime = 0;
unsigned long scanCount = 0;

// RSSI history for analysis
const int RSSI_HISTORY_SIZE = 20;
int rssiHistory[RSSI_HISTORY_SIZE];
int historyIndex = 0;
int historyCount = 0;

/**
 * @brief BLE scan callback class
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        String deviceMAC = advertisedDevice.getAddress().toString();
        deviceMAC.toLowerCase();
        
        // Check if this is our target device
        if (deviceMAC == targetMAC.toLowerCase()) {
            targetFound = true;
            lastRSSI = advertisedDevice.getRSSI();
            lastScanTime = millis();
            
            // Add to history
            rssiHistory[historyIndex] = lastRSSI;
            historyIndex = (historyIndex + 1) % RSSI_HISTORY_SIZE;
            if (historyCount < RSSI_HISTORY_SIZE) {
                historyCount++;
            }
            
            // Print RSSI reading
            Serial.printf("Target found - RSSI: %d dBm, Distance: ~%.1f m\n", 
                         lastRSSI, rssiToDistance(lastRSSI));
        }
        
        scanCount++;
    }
};

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    
    Serial.println("=== BLE Scanner Example ===");
    Serial.println("Scanning for BLE devices and measuring RSSI");
    Serial.printf("Target MAC: %s\n", targetMAC.c_str());
    Serial.printf("Unlock threshold: %d dBm\n", UNLOCK_RSSI_THRESHOLD);
    Serial.printf("Lock threshold: %d dBm\n", LOCK_RSSI_THRESHOLD);
    Serial.println("================================================");
    
    // Initialize BLE
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    
    // Configure scan parameters
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(BLE_SCAN_INTERVAL);
    pBLEScan->setWindow(BLE_SCAN_WINDOW);
    
    // Start scanning
    pBLEScan->start(BLE_SCAN_TIMEOUT, nullptr, false);
    
    Serial.println("BLE scanning started!");
    Serial.println("Commands:");
    Serial.println("  status  - Show scan status and RSSI history");
    Serial.println("  reset   - Reset RSSI history");
    Serial.println("  help    - Show this help");
    Serial.println("================================================");
    
    // Initialize RSSI history
    for (int i = 0; i < RSSI_HISTORY_SIZE; i++) {
        rssiHistory[i] = RSSI_MIN_VALID;
    }
}

void loop() {
    // Check if scanning is still active
    if (!pBLEScan->isScanning()) {
        Serial.println("BLE scan stopped, restarting...");
        pBLEScan->start(BLE_SCAN_TIMEOUT, nullptr, false);
    }
    
    // Handle USB serial commands
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        command.toUpperCase();
        
        if (command == "STATUS") {
            printStatus();
        } else if (command == "RESET") {
            resetHistory();
        } else if (command == "HELP") {
            printHelp();
        } else if (command.length() > 0) {
            Serial.printf("Unknown command: %s\n", command.c_str());
            Serial.println("Type 'help' for available commands.");
        }
    }
    
    // Periodic status update
    static unsigned long lastStatusUpdate = 0;
    if (millis() - lastStatusUpdate > 10000) { // Every 10 seconds
        lastStatusUpdate = millis();
        if (targetFound) {
            Serial.printf("Status: Target visible, RSSI: %d dBm, Scans: %lu\n", 
                         lastRSSI, scanCount);
        } else {
            Serial.printf("Status: Target not found, Scans: %lu\n", scanCount);
        }
    }
    
    delay(100);
}

void printStatus() {
    Serial.println("=== Scan Status ===");
    Serial.printf("Target MAC: %s\n", targetMAC.c_str());
    Serial.printf("Target Found: %s\n", targetFound ? "YES" : "NO");
    Serial.printf("Last RSSI: %d dBm\n", lastRSSI);
    Serial.printf("Last Scan: %lu ms ago\n", millis() - lastScanTime);
    Serial.printf("Total Scans: %lu\n", scanCount);
    
    if (historyCount > 0) {
        Serial.println("RSSI History (last 20 readings):");
        int start = (historyIndex - min(historyCount, 10)) % RSSI_HISTORY_SIZE;
        if (start < 0) start += RSSI_HISTORY_SIZE;
        
        for (int i = 0; i < min(historyCount, 10); i++) {
            int idx = (start + i) % RSSI_HISTORY_SIZE;
            Serial.printf("  %d: %d dBm\n", i + 1, rssiHistory[idx]);
        }
        
        // Calculate statistics
        int minRSSI = RSSI_MIN_VALID;
        int maxRSSI = RSSI_MIN_VALID;
        int sumRSSI = 0;
        
        for (int i = 0; i < historyCount; i++) {
            int rssi = rssiHistory[i];
            if (rssi > RSSI_MIN_VALID) {
                if (minRSSI == RSSI_MIN_VALID || rssi < minRSSI) minRSSI = rssi;
                if (rssi > maxRSSI) maxRSSI = rssi;
                sumRSSI += rssi;
            }
        }
        
        if (historyCount > 0) {
            Serial.printf("RSSI Stats: Min=%d, Max=%d, Avg=%.1f dBm\n", 
                         minRSSI, maxRSSI, (float)sumRSSI / historyCount);
        }
    }
    
    Serial.println("==================");
}

void resetHistory() {
    for (int i = 0; i < RSSI_HISTORY_SIZE; i++) {
        rssiHistory[i] = RSSI_MIN_VALID;
    }
    historyIndex = 0;
    historyCount = 0;
    scanCount = 0;
    targetFound = false;
    lastRSSI = RSSI_MIN_VALID;
    
    Serial.println("RSSI history reset");
}

void printHelp() {
    Serial.println("=== Available Commands ===");
    Serial.println("STATUS  - Show scan status and RSSI history");
    Serial.println("RESET   - Reset RSSI history and counters");
    Serial.println("HELP    - Show this help message");
    Serial.println("===========================");
    Serial.println("Note: Target device must be advertising to be detected");
    Serial.println("RSSI values closer to 0 indicate closer proximity");
}

/**
 * @brief Convert RSSI to approximate distance
 * @param rssi RSSI value in dBm
 * @return Approximate distance in meters
 */
float rssiToDistance(int rssi) {
    // Rough estimation based on free space path loss
    if (rssi >= -30) return 1.0;   // Very close
    if (rssi >= -40) return 2.0;   // Close
    if (rssi >= -50) return 4.0;   // Near
    if (rssi >= -60) return 8.0;   // Medium
    if (rssi >= -70) return 16.0;  // Far
    if (rssi >= -80) return 32.0;  // Very far
    if (rssi >= -90) return 64.0;  // Extremely far
    return 128.0;  // Out of range
}
