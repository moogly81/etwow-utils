/**
 * @file uart_passthrough.ino
 * @brief UART Passthrough Example - Test UART command injection
 * 
 * This example demonstrates how to inject commands into the UART bus
 * while maintaining passthrough functionality.
 */

#include <Arduino.h>
#include "config.h"
#include "pins.h"

// UART configuration
HardwareSerial BusSerial(BUS_UART_NUM);
String commandBuffer = "";
unsigned long lastCommandTime = 0;
bool commandInProgress = false;

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    
    Serial.println("=== UART Passthrough Example ===");
    Serial.println("Testing UART command injection and passthrough");
    Serial.printf("UART: %d, RX: %d, TX: %d, Baud: %d\n", 
                 BUS_UART_NUM, BUS_UART_RX_PIN, BUS_UART_TX_PIN, UART_BAUD_RATE);
    Serial.printf("OE Pin: %d (tri-state buffer control)\n", OE_PIN);
    Serial.println("================================================");
    
    // Initialize pins
    pinMode(OE_PIN, OUTPUT);
    digitalWrite(OE_PIN, LOW);  // Start in tri-state mode
    
    // Initialize UART
    BusSerial.begin(UART_BAUD_RATE, SERIAL_8N1, BUS_UART_RX_PIN, BUS_UART_TX_PIN);
    
    Serial.println("UART passthrough ready!");
    Serial.println("Commands:");
    Serial.println("  LOCK    - Send lock command");
    Serial.println("  UNLOCK  - Send unlock command");
    Serial.println("  status  - Show system status");
    Serial.println("  help    - Show this help");
    Serial.println("================================================");
}

void loop() {
    // Handle incoming UART data (passthrough to USB)
    if (BusSerial.available()) {
        while (BusSerial.available()) {
            uint8_t data = BusSerial.read();
            Serial.write(data);  // Forward to USB serial
        }
    }
    
    // Handle USB serial commands
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        input.toUpperCase();
        
        if (input == "LOCK") {
            sendCommand("LOCK");
        } else if (input == "UNLOCK") {
            sendCommand("UNLOCK");
        } else if (input == "STATUS") {
            printStatus();
        } else if (input == "HELP") {
            printHelp();
        } else if (input.length() > 0) {
            Serial.printf("Unknown command: %s\n", input.c_str());
            Serial.println("Type 'help' for available commands.");
        }
    }
    
    // Check for command completion
    if (commandInProgress && (millis() - lastCommandTime > COMMAND_DELAY)) {
        commandInProgress = false;
        digitalWrite(OE_PIN, LOW);  // Return to tri-state
        Serial.println("Command transmission complete");
    }
    
    delay(1);
}

void sendCommand(const char* command) {
    if (commandInProgress) {
        Serial.println("ERROR: Command already in progress");
        return;
    }
    
    Serial.printf("Sending command: %s\n", command);
    
    // Enable transmission
    digitalWrite(OE_PIN, HIGH);
    delay(2);  // Small delay for buffer activation
    
    // Send command
    BusSerial.print(command);
    BusSerial.print('\n');
    BusSerial.flush();
    
    // Update state
    commandInProgress = true;
    lastCommandTime = millis();
    
    Serial.println("Command sent, returning to passthrough mode...");
}

void printStatus() {
    Serial.println("=== System Status ===");
    Serial.printf("UART Baud Rate: %d\n", UART_BAUD_RATE);
    Serial.printf("OE Pin State: %s\n", digitalRead(OE_PIN) ? "ENABLED" : "TRI-STATE");
    Serial.printf("Command In Progress: %s\n", commandInProgress ? "YES" : "NO");
    Serial.printf("Last Command: %lu ms ago\n", millis() - lastCommandTime);
    Serial.println("===================");
}

void printHelp() {
    Serial.println("=== Available Commands ===");
    Serial.println("LOCK    - Send LOCK command to scooter");
    Serial.println("UNLOCK  - Send UNLOCK command to scooter");
    Serial.println("STATUS  - Show system status information");
    Serial.println("HELP    - Show this help message");
    Serial.println("===========================");
    Serial.println("Note: Commands are sent with newline terminator");
    Serial.println("Tri-state buffer prevents bus collisions");
}
