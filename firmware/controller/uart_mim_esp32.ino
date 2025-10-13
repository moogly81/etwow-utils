/**
 * @file uart_mim_esp32.ino
 * @brief Main ESP32 controller for Etwow GT Auto-lock System
 * @author Etwow Utils Project
 * @version 1.0.0
 * 
 * This firmware implements a BLE central device that automatically locks/unlocks
 * an Etwow GT electric scooter based on proximity detection of a BLE badge.
 * 
 * Features:
 * - BLE scanning with RSSI-based proximity detection
 * - UART interception and command injection
 * - Hysteresis logic to prevent rapid toggling
 * - Non-destructive inline installation
 * - Tri-state buffer control for UART bus management
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "config.h"
#include "pins.h"
#include "ble_manager.h"
#include "uart_manager.h"
#include "proximity_controller.h"
#include "utils.h"

// Global managers
BLEManager bleManager;
UARTManager uartManager;
ProximityController proximityController;

// System state
bool systemInitialized = false;
unsigned long lastHeartbeat = 0;
unsigned long lastStatusUpdate = 0;

/**
 * @brief Initialize all system components
 */
void initializeSystem() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    
    Serial.println("=== Etwow GT Auto-lock System ===");
    Serial.println("Initializing system components...");
    
    // Initialize pin configurations
    pinMode(OE_PIN, OUTPUT);
    digitalWrite(OE_PIN, LOW);  // Tri-state by default
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    // Initialize managers
    if (!bleManager.initialize()) {
        Serial.println("ERROR: BLE initialization failed!");
        return;
    }
    
    if (!uartManager.initialize()) {
        Serial.println("ERROR: UART initialization failed!");
        return;
    }
    
    if (!proximityController.initialize()) {
        Serial.println("ERROR: Proximity controller initialization failed!");
        return;
    }
    
    systemInitialized = true;
    Serial.println("System initialized successfully!");
    Serial.printf("Target badge MAC: %s\n", TARGET_BADGE_MAC);
    Serial.printf("Unlock threshold: %d dBm\n", UNLOCK_RSSI_THRESHOLD);
    Serial.printf("Lock threshold: %d dBm\n", LOCK_RSSI_THRESHOLD);
    Serial.printf("Hysteresis: %d dBm\n", RSSI_HYSTERESIS);
}

/**
 * @brief Main setup function
 */
void setup() {
    initializeSystem();
}

/**
 * @brief Main loop function
 */
void loop() {
    if (!systemInitialized) {
        delay(1000);
        return;
    }
    
    unsigned long currentTime = millis();
    
    // Update BLE scanning
    bleManager.update();
    
    // Check for proximity changes
    proximityController.update(bleManager.getLastRSSI(), currentTime);
    
    // Handle UART passthrough
    uartManager.handlePassthrough();
    
    // Send lock/unlock commands if needed
    if (proximityController.shouldUnlock()) {
        uartManager.sendLockCommand("UNLOCK");
        proximityController.setUnlocked(true);
        digitalWrite(LED_PIN, HIGH);  // LED on when unlocked
        Serial.println("UNLOCK command sent");
    } else if (proximityController.shouldLock()) {
        uartManager.sendLockCommand("LOCK");
        proximityController.setUnlocked(false);
        digitalWrite(LED_PIN, LOW);   // LED off when locked
        Serial.println("LOCK command sent");
    }
    
    // Heartbeat and status updates
    if (currentTime - lastHeartbeat > HEARTBEAT_INTERVAL) {
        lastHeartbeat = currentTime;
        Serial.printf("Heartbeat - RSSI: %d, State: %s\n", 
                     bleManager.getLastRSSI(),
                     proximityController.isUnlocked() ? "UNLOCKED" : "LOCKED");
    }
    
    // Status LED blink
    if (currentTime - lastStatusUpdate > STATUS_LED_INTERVAL) {
        lastStatusUpdate = currentTime;
        if (bleManager.isConnected()) {
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));  // Blink when connected
        }
    }
    
    // Small delay to prevent overwhelming the system
    delay(10);
}
