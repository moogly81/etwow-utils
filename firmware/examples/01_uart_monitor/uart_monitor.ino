/**
 * @file uart_monitor.ino
 * @brief UART Monitor Example - Monitor UART communication between display and BT module
 * 
 * This example demonstrates how to monitor UART traffic between the scooter's
 * display and Bluetooth module for debugging and analysis.
 */

#include <Arduino.h>
#include "config.h"
#include "pins.h"

// UART configuration
HardwareSerial BusSerial(BUS_UART_NUM);
unsigned long lastDataTime = 0;
unsigned long dataCount = 0;

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    
    Serial.println("=== UART Monitor Example ===");
    Serial.println("Monitoring UART traffic between display and BT module");
    Serial.printf("UART: %d, RX: %d, TX: %d, Baud: %d\n", 
                 BUS_UART_NUM, BUS_UART_RX_PIN, BUS_UART_TX_PIN, UART_BAUD_RATE);
    Serial.println("Press any key to start monitoring...");
    Serial.println("================================================");
    
    // Wait for user input
    while (!Serial.available()) {
        delay(100);
    }
    Serial.read(); // Clear input buffer
    
    // Initialize UART
    BusSerial.begin(UART_BAUD_RATE, SERIAL_8N1, BUS_UART_RX_PIN, BUS_UART_TX_PIN);
    
    Serial.println("UART monitoring started!");
    Serial.println("Format: [timestamp] [hex] [ascii]");
    Serial.println("================================================");
    
    lastDataTime = millis();
}

void loop() {
    // Check for incoming data
    if (BusSerial.available()) {
        while (BusSerial.available()) {
            uint8_t data = BusSerial.read();
            dataCount++;
            
            // Print timestamp and data
            unsigned long timestamp = millis();
            Serial.printf("[%08lu] 0x%02X ", timestamp, data);
            
            // Print ASCII representation
            if (data >= 32 && data <= 126) {
                Serial.printf("'%c'", data);
            } else {
                Serial.print("'.'");
            }
            
            // Print data count
            Serial.printf(" (#%lu)", dataCount);
            Serial.println();
            
            lastDataTime = timestamp;
        }
    }
    
    // Check for inactivity
    if (millis() - lastDataTime > 5000) {
        if (dataCount == 0) {
            Serial.println("No UART data detected. Check connections and baud rate.");
        } else {
            Serial.printf("UART inactive for 5 seconds. Total data: %lu bytes\n", dataCount);
        }
        lastDataTime = millis();
    }
    
    // Handle USB serial commands
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (command == "reset") {
            dataCount = 0;
            Serial.println("Data counter reset");
        } else if (command == "status") {
            Serial.printf("Status: %lu bytes received, last data %lu ms ago\n", 
                         dataCount, millis() - lastDataTime);
        } else if (command == "help") {
            Serial.println("Commands:");
            Serial.println("  reset  - Reset data counter");
            Serial.println("  status - Show monitoring status");
            Serial.println("  help   - Show this help");
        } else {
            Serial.println("Unknown command. Type 'help' for available commands.");
        }
    }
    
    delay(1);
}
