/**
 * @file ble_manager.h
 * @brief BLE Manager class for handling BLE scanning and badge detection
 */

#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "config.h"

class BLEManager {
private:
    BLEScan* pBLEScan;
    bool initialized;
    bool targetFound;
    int lastRSSI;
    unsigned long lastScanTime;
    String targetMAC;
    
    // BLE scan callback class
    class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    private:
        BLEManager* parent;
        
    public:
        MyAdvertisedDeviceCallbacks(BLEManager* manager) : parent(manager) {}
        
        void onResult(BLEAdvertisedDevice advertisedDevice) {
            if (parent) {
                parent->handleDeviceFound(advertisedDevice);
            }
        }
    };

public:
    BLEManager();
    ~BLEManager();
    
    /**
     * @brief Initialize BLE scanning system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Update BLE scanning (call in main loop)
     */
    void update();
    
    /**
     * @brief Get the last RSSI reading from target badge
     * @return RSSI value in dBm, or RSSI_MIN_VALID if not found
     */
    int getLastRSSI() const { return lastRSSI; }
    
    /**
     * @brief Check if target badge is currently connected/visible
     * @return true if target is found, false otherwise
     */
    bool isConnected() const { return targetFound; }
    
    /**
     * @brief Set the target MAC address to scan for
     * @param mac MAC address string in format "AA:BB:CC:DD:EE:FF"
     */
    void setTargetMAC(const String& mac) { targetMAC = mac; }
    
    /**
     * @brief Get the target MAC address
     * @return MAC address string
     */
    String getTargetMAC() const { return targetMAC; }
    
    /**
     * @brief Start BLE scanning
     * @return true if scanning started successfully
     */
    bool startScanning();
    
    /**
     * @brief Stop BLE scanning
     */
    void stopScanning();
    
    /**
     * @brief Reset connection state
     */
    void reset();

private:
    /**
     * @brief Handle device found callback
     * @param device The discovered BLE device
     */
    void handleDeviceFound(BLEAdvertisedDevice device);
    
    /**
     * @brief Check if device MAC matches target
     * @param deviceMAC MAC address to check
     * @return true if MAC matches target
     */
    bool isTargetDevice(const String& deviceMAC);
};

#endif // BLE_MANAGER_H
