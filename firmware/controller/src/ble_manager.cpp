/**
 * @file ble_manager.cpp
 * @brief Implementation of BLE Manager class
 */

#include "ble_manager.h"

BLEManager::BLEManager() 
    : pBLEScan(nullptr)
    , initialized(false)
    , targetFound(false)
    , lastRSSI(RSSI_MIN_VALID)
    , lastScanTime(0)
    , targetMAC(TARGET_BADGE_MAC) {
}

BLEManager::~BLEManager() {
    if (initialized && pBLEScan) {
        pBLEScan->stop();
    }
}

bool BLEManager::initialize() {
    if (initialized) {
        return true;
    }
    
    // Initialize BLE device
    BLEDevice::init("");
    
    // Get scan object
    pBLEScan = BLEDevice::getScan();
    if (!pBLEScan) {
        Serial.println("ERROR: Failed to get BLE scan object");
        return false;
    }
    
    // Set up scan parameters
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(this));
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(BLE_SCAN_INTERVAL);
    pBLEScan->setWindow(BLE_SCAN_WINDOW);
    
    // Start scanning
    if (!startScanning()) {
        Serial.println("ERROR: Failed to start BLE scanning");
        return false;
    }
    
    initialized = true;
    Serial.println("BLE Manager initialized successfully");
    Serial.printf("Scanning for target MAC: %s\n", targetMAC.c_str());
    
    return true;
}

void BLEManager::update() {
    if (!initialized || !pBLEScan) {
        return;
    }
    
    // Check if scanning is still active
    if (!pBLEScan->isScanning()) {
        Serial.println("BLE scan stopped, restarting...");
        startScanning();
    }
    
    // Update scan status periodically
    unsigned long currentTime = millis();
    if (currentTime - lastScanTime > 10000) {  // Every 10 seconds
        lastScanTime = currentTime;
        if (ENABLE_DEBUG_PRINTS) {
            Serial.printf("BLE Status - Target Found: %s, RSSI: %d dBm\n", 
                         targetFound ? "YES" : "NO", lastRSSI);
        }
    }
}

bool BLEManager::startScanning() {
    if (!pBLEScan) {
        return false;
    }
    
    // Start continuous scanning (timeout = 0)
    pBLEScan->start(BLE_SCAN_TIMEOUT, nullptr, false);
    return true;
}

void BLEManager::stopScanning() {
    if (pBLEScan && pBLEScan->isScanning()) {
        pBLEScan->stop();
    }
}

void BLEManager::reset() {
    targetFound = false;
    lastRSSI = RSSI_MIN_VALID;
}

void BLEManager::handleDeviceFound(BLEAdvertisedDevice device) {
    String deviceMAC = device.getAddress().toString();
    
    if (isTargetDevice(deviceMAC)) {
        targetFound = true;
        lastRSSI = device.getRSSI();
        
        if (ENABLE_DEBUG_PRINTS) {
            Serial.printf("Target device found - MAC: %s, RSSI: %d dBm\n", 
                         deviceMAC.c_str(), lastRSSI);
        }
    }
}

bool BLEManager::isTargetDevice(const String& deviceMAC) {
    // Convert both MACs to lowercase for comparison
    String targetLower = targetMAC;
    String deviceLower = deviceMAC;
    
    targetLower.toLowerCase();
    deviceLower.toLowerCase();
    
    return (targetLower == deviceLower);
}
