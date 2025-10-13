/**
 * @file config.h
 * @brief Configuration parameters for the Etwow GT Auto-lock System
 */

#ifndef CONFIG_H
#define CONFIG_H

// BLE Configuration
#define TARGET_BADGE_MAC "AA:BB:CC:DD:EE:FF"  // Update this with your badge MAC
#define BLE_SCAN_INTERVAL 100                  // ms
#define BLE_SCAN_WINDOW 99                     // ms
#define BLE_SCAN_TIMEOUT 0                     // 0 = continuous scanning

// RSSI Thresholds (dBm)
#define UNLOCK_RSSI_THRESHOLD -60             // Unlock when RSSI > this value
#define LOCK_RSSI_THRESHOLD -80               // Lock when RSSI < this value
#define RSSI_HYSTERESIS 5                     // Prevent rapid toggling
#define RSSI_MIN_VALID -120                   // Minimum valid RSSI reading

// Timing Configuration
#define HEARTBEAT_INTERVAL 5000               // ms - status heartbeat interval
#define STATUS_LED_INTERVAL 500               // ms - LED blink interval
#define COMMAND_DELAY 50                      // ms - delay after sending command
#define UART_FLUSH_DELAY 10                   // ms - delay after UART flush

// UART Configuration
#define UART_BAUD_RATE 115200
#define UART_TIMEOUT_MS 1000

// System Configuration
#define ENABLE_DEBUG_PRINTS true              // Enable debug serial output
#define ENABLE_UART_FORWARDING true           // Forward UART traffic to USB
#define ENABLE_STATUS_LED true                // Enable status LED indication

// Safety Configuration
#define MAX_UNLOCK_DURATION 300000            // ms - max time unlocked (5 min)
#define EMERGENCY_LOCK_TIMEOUT 60000          // ms - emergency lock timeout (1 min)

// Command Strings
#define LOCK_COMMAND "LOCK"
#define UNLOCK_COMMAND "UNLOCK"

// Version Information
#define FIRMWARE_VERSION "1.0.0"
#define HARDWARE_VERSION "1.0"
#define BUILD_DATE __DATE__ " " __TIME__

#endif // CONFIG_H
