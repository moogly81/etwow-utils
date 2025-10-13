/**
 * @file proximity_controller.h
 * @brief Proximity Controller class for managing lock/unlock logic with hysteresis
 */

#ifndef PROXIMITY_CONTROLLER_H
#define PROXIMITY_CONTROLLER_H

#include <Arduino.h>
#include "config.h"

class ProximityController {
private:
    bool initialized;
    bool unlocked;
    bool lastDecisionValid;
    int lastRSSI;
    unsigned long lastDecisionTime;
    unsigned long lastUnlockTime;
    unsigned long lastLockTime;
    
    // Hysteresis state
    int unlockThreshold;
    int lockThreshold;
    int hysteresis;
    
public:
    ProximityController();
    ~ProximityController();
    
    /**
     * @brief Initialize proximity controller
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Update proximity logic with new RSSI reading
     * @param rssi Current RSSI reading in dBm
     * @param currentTime Current system time in milliseconds
     */
    void update(int rssi, unsigned long currentTime);
    
    /**
     * @brief Check if system should unlock
     * @return true if unlock command should be sent
     */
    bool shouldUnlock() const;
    
    /**
     * @brief Check if system should lock
     * @return true if lock command should be sent
     */
    bool shouldLock() const;
    
    /**
     * @brief Get current lock state
     * @return true if currently unlocked, false if locked
     */
    bool isUnlocked() const { return unlocked; }
    
    /**
     * @brief Set lock state (called after sending command)
     * @param state true for unlocked, false for locked
     */
    void setUnlocked(bool state) { unlocked = state; }
    
    /**
     * @brief Get last RSSI reading
     * @return RSSI value in dBm
     */
    int getLastRSSI() const { return lastRSSI; }
    
    /**
     * @brief Get time since last decision
     * @return milliseconds since last state change
     */
    unsigned long getTimeSinceLastDecision() const;
    
    /**
     * @brief Get time since last unlock
     * @return milliseconds since last unlock
     */
    unsigned long getTimeSinceLastUnlock() const;
    
    /**
     * @brief Check if emergency lock should be triggered
     * @return true if emergency lock conditions are met
     */
    bool shouldEmergencyLock() const;
    
    /**
     * @brief Reset controller state
     */
    void reset();
    
    /**
     * @brief Set RSSI thresholds
     * @param unlockThreshold RSSI threshold for unlocking (dBm)
     * @param lockThreshold RSSI threshold for locking (dBm)
     * @param hysteresisValue Hysteresis value (dBm)
     */
    void setThresholds(int unlockThreshold, int lockThreshold, int hysteresisValue);

private:
    /**
     * @brief Check if RSSI reading is valid
     * @param rssi RSSI value to check
     * @return true if RSSI is within valid range
     */
    bool isValidRSSI(int rssi) const;
    
    /**
     * @brief Calculate effective thresholds with hysteresis
     * @param currentState Current lock state
     * @return effective threshold for current state
     */
    int getEffectiveThreshold(bool currentState) const;
    
    /**
     * @brief Make lock/unlock decision based on RSSI and state
     * @param rssi Current RSSI reading
     * @param currentTime Current system time
     * @return true if state should change
     */
    bool shouldChangeState(int rssi, unsigned long currentTime) const;
};

#endif // PROXIMITY_CONTROLLER_H
