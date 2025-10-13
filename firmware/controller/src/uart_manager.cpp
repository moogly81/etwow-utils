/**
 * @file uart_manager.cpp
 * @brief Implementation of UART Manager class
 */

#include "uart_manager.h"

UARTManager::UARTManager() 
    : busSerial(nullptr)
    , initialized(false)
    , lastCommandTime(0)
    , commandInProgress(false) {
}

UARTManager::~UARTManager() {
    if (initialized && busSerial) {
        busSerial->end();
    }
}

bool UARTManager::initialize() {
    if (initialized) {
        return true;
    }
    
    // Initialize hardware serial for bus communication
    busSerial = &Serial1;  // Use UART1
    
    // Configure UART with specified parameters
    busSerial->begin(UART_BAUD_RATE, SERIAL_8N1, BUS_UART_RX_PIN, BUS_UART_TX_PIN);
    busSerial->setTimeout(UART_TIMEOUT_MS);
    
    // Initialize tri-state buffer control
    pinMode(OE_PIN, OUTPUT);
    digitalWrite(OE_PIN, LOW);  // Start in tri-state mode
    
    // Clear any existing data
    while (busSerial->available()) {
        busSerial->read();
    }
    
    initialized = true;
    Serial.println("UART Manager initialized successfully");
    Serial.printf("UART configured - Baud: %d, RX: %d, TX: %d\n", 
                 UART_BAUD_RATE, BUS_UART_RX_PIN, BUS_UART_TX_PIN);
    
    return true;
}

void UARTManager::handlePassthrough() {
    if (!initialized || !busSerial || !ENABLE_UART_FORWARDING) {
        return;
    }
    
    // Forward any received data to USB serial
    while (busSerial->available()) {
        char data = busSerial->read();
        processReceivedData(data);
        
        // Forward to USB serial for debugging
        Serial.write(data);
    }
}

bool UARTManager::sendLockCommand(const char* command) {
    if (!initialized || !busSerial) {
        Serial.println("ERROR: UART not initialized");
        return false;
    }
    
    if (!validateCommand(command)) {
        Serial.printf("ERROR: Invalid command '%s'\n", command);
        return false;
    }
    
    if (commandInProgress) {
        Serial.println("WARNING: Command already in progress, skipping");
        return false;
    }
    
    // Wait for UART to be ready
    if (!waitForReady()) {
        Serial.println("ERROR: UART not ready for transmission");
        return false;
    }
    
    commandInProgress = true;
    
    // Enable transmission
    enableTransmission();
    
    // Send command
    busSerial->print(command);
    busSerial->print('\n');  // Add newline terminator
    busSerial->flush();      // Ensure data is sent
    
    // Wait for transmission to complete
    delay(UART_FLUSH_DELAY);
    
    // Disable transmission (return to tri-state)
    disableTransmission();
    
    // Update state
    lastCommandTime = millis();
    commandInProgress = false;
    
    if (ENABLE_DEBUG_PRINTS) {
        Serial.printf("Command sent successfully: %s\n", command);
    }
    
    return true;
}

void UARTManager::reset() {
    commandInProgress = false;
    rxBuffer = "";
    lastCommandTime = 0;
    
    if (initialized && busSerial) {
        // Clear any pending data
        while (busSerial->available()) {
            busSerial->read();
        }
    }
}

void UARTManager::enableTransmission() {
    digitalWrite(OE_PIN, HIGH);
    delay(2);  // Small delay for buffer to activate
}

void UARTManager::disableTransmission() {
    delay(5);  // Small delay for transmission to complete
    digitalWrite(OE_PIN, LOW);
}

void UARTManager::processReceivedData(char data) {
    // Handle received data (could be responses, status, etc.)
    if (data == '\n' || data == '\r') {
        if (rxBuffer.length() > 0) {
            // Process complete line
            if (ENABLE_DEBUG_PRINTS) {
                Serial.printf("Received: %s\n", rxBuffer.c_str());
            }
            rxBuffer = "";
        }
    } else {
        rxBuffer += data;
        
        // Prevent buffer overflow
        if (rxBuffer.length() > 256) {
            rxBuffer = "";
        }
    }
}

bool UARTManager::validateCommand(const char* command) {
    if (!command || strlen(command) == 0) {
        return false;
    }
    
    // Check against known valid commands
    return (strcmp(command, LOCK_COMMAND) == 0 || 
            strcmp(command, UNLOCK_COMMAND) == 0);
}

bool UARTManager::waitForReady(unsigned long timeoutMs) {
    unsigned long startTime = millis();
    
    while (millis() - startTime < timeoutMs) {
        if (!commandInProgress) {
            return true;
        }
        delay(1);
    }
    
    return false;
}
