/**
 * @file uart_manager.h
 * @brief UART Manager class for handling UART communication and command injection
 */

#ifndef UART_MANAGER_H
#define UART_MANAGER_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include "config.h"
#include "pins.h"

class UARTManager {
private:
    HardwareSerial* busSerial;
    bool initialized;
    String rxBuffer;
    unsigned long lastCommandTime;
    bool commandInProgress;
    
public:
    UARTManager();
    ~UARTManager();
    
    /**
     * @brief Initialize UART communication
     * @return true if initialization successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Handle UART passthrough (forward traffic to USB serial)
     */
    void handlePassthrough();
    
    /**
     * @brief Send lock/unlock command to the scooter
     * @param command Command string ("LOCK" or "UNLOCK")
     * @return true if command sent successfully
     */
    bool sendLockCommand(const char* command);
    
    /**
     * @brief Check if UART is ready for transmission
     * @return true if ready, false if busy or not initialized
     */
    bool isReady() const { return initialized && !commandInProgress; }
    
    /**
     * @brief Get the last command time
     * @return timestamp of last command sent
     */
    unsigned long getLastCommandTime() const { return lastCommandTime; }
    
    /**
     * @brief Check if a command is currently in progress
     * @return true if command is being sent
     */
    bool isCommandInProgress() const { return commandInProgress; }
    
    /**
     * @brief Reset UART manager state
     */
    void reset();
    
    /**
     * @brief Enable tri-state buffer for transmission
     */
    void enableTransmission();
    
    /**
     * @brief Disable tri-state buffer (tri-state mode)
     */
    void disableTransmission();

private:
    /**
     * @brief Process received UART data
     * @param data Character received from UART
     */
    void processReceivedData(char data);
    
    /**
     * @brief Validate command before sending
     * @param command Command string to validate
     * @return true if command is valid
     */
    bool validateCommand(const char* command);
    
    /**
     * @brief Wait for UART to be ready
     * @param timeoutMs Maximum time to wait in milliseconds
     * @return true if UART becomes ready within timeout
     */
    bool waitForReady(unsigned long timeoutMs = UART_TIMEOUT_MS);
};

#endif // UART_MANAGER_H
