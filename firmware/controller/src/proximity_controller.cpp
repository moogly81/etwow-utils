/**
 * @file proximity_controller.cpp
 * @brief Implementation of Proximity Controller class
 */

#include "proximity_controller.h"

ProximityController::ProximityController() 
    : initialized(false)
    , unlocked(false)
    , lastDecisionValid(false)
    , lastRSSI(RSSI_MIN_VALID)
    , lastDecisionTime(0)
    , lastUnlockTime(0)
    , lastLockTime(0)
    , unlockThreshold(UNLOCK_RSSI_THRESHOLD)
    , lockThreshold(LOCK_RSSI_THRESHOLD)
    , hysteresis(RSSI_HYSTERESIS) {
}

ProximityController::~ProximityController() {
}

bool ProximityController::initialize() {
    if (initialized) {
        return true;
    }
    
    // Initialize with default locked state
    unlocked = false;
    lastDecisionValid = false;
    lastRSSI = RSSI_MIN_VALID;
    lastDecisionTime = 0;
    lastUnlockTime = 0;
    lastLockTime = millis();
    
    initialized = true;
    Serial.println("Proximity Controller initialized successfully");
    Serial.printf("Thresholds - Unlock: %d dBm, Lock: %d dBm, Hysteresis: %d dBm\n",
                 unlockThreshold, lockThreshold, hysteresis);
    
    return true;
}

void ProximityController::update(int rssi, unsigned long currentTime) {
    if (!initialized) {
        return;
    }
    
    // Update RSSI reading
    lastRSSI = rssi;
    
    // Check if RSSI is valid
    if (!isValidRSSI(rssi)) {
        if (ENABLE_DEBUG_PRINTS) {
            Serial.printf("Invalid RSSI reading: %d dBm\n", rssi);
        }
        return;
    }
    
    // Check for emergency lock conditions
    if (shouldEmergencyLock()) {
        if (unlocked) {
            if (ENABLE_DEBUG_PRINTS) {
                Serial.println("Emergency lock triggered - timeout exceeded");
            }
            lastDecisionValid = true;
            lastDecisionTime = currentTime;
            lastLockTime = currentTime;
        }
        return;
    }
    
    // Make decision based on current state and RSSI
    if (shouldChangeState(rssi, currentTime)) {
        lastDecisionValid = true;
        lastDecisionTime = currentTime;
        
        if (unlocked) {
            // Should lock
            lastLockTime = currentTime;
            if (ENABLE_DEBUG_PRINTS) {
                Serial.printf("Decision: LOCK (RSSI: %d dBm)\n", rssi);
            }
        } else {
            // Should unlock
            lastUnlockTime = currentTime;
            if (ENABLE_DEBUG_PRINTS) {
                Serial.printf("Decision: UNLOCK (RSSI: %d dBm)\n", rssi);
            }
        }
    }
}

bool ProximityController::shouldUnlock() const {
    if (!initialized || !lastDecisionValid || unlocked) {
        return false;
    }
    
    // Check if we should unlock based on last decision
    return (lastRSSI > getEffectiveThreshold(false));
}

bool ProximityController::shouldLock() const {
    if (!initialized || !lastDecisionValid || !unlocked) {
        return false;
    }
    
    // Check if we should lock based on last decision
    return (lastRSSI < getEffectiveThreshold(true));
}

unsigned long ProximityController::getTimeSinceLastDecision() const {
    if (!lastDecisionValid) {
        return 0;
    }
    return millis() - lastDecisionTime;
}

unsigned long ProximityController::getTimeSinceLastUnlock() const {
    if (lastUnlockTime == 0) {
        return 0;
    }
    return millis() - lastUnlockTime;
}

bool ProximityController::shouldEmergencyLock() const {
    if (!unlocked) {
        return false;
    }
    
    // Check if unlocked for too long
    unsigned long unlockDuration = getTimeSinceLastUnlock();
    return (unlockDuration > MAX_UNLOCK_DURATION);
}

void ProximityController::reset() {
    unlocked = false;
    lastDecisionValid = false;
    lastRSSI = RSSI_MIN_VALID;
    lastDecisionTime = 0;
    lastUnlockTime = 0;
    lastLockTime = millis();
}

void ProximityController::setThresholds(int unlockThresh, int lockThresh, int hyst) {
    unlockThreshold = unlockThresh;
    lockThreshold = lockThresh;
    hysteresis = hyst;
    
    if (ENABLE_DEBUG_PRINTS) {
        Serial.printf("Thresholds updated - Unlock: %d dBm, Lock: %d dBm, Hysteresis: %d dBm\n",
                     unlockThreshold, lockThreshold, hysteresis);
    }
}

bool ProximityController::isValidRSSI(int rssi) const {
    return (rssi >= RSSI_MIN_VALID && rssi <= 0);
}

int ProximityController::getEffectiveThreshold(bool currentState) const {
    if (currentState) {
        // Currently unlocked - use lock threshold with hysteresis
        return lockThreshold + hysteresis;
    } else {
        // Currently locked - use unlock threshold with hysteresis
        return unlockThreshold - hysteresis;
    }
}

bool ProximityController::shouldChangeState(int rssi, unsigned long currentTime) const {
    if (!isValidRSSI(rssi)) {
        return false;
    }
    
    // Get effective threshold for current state
    int effectiveThreshold = getEffectiveThreshold(unlocked);
    
    if (unlocked) {
        // Currently unlocked - check if should lock
        return (rssi < effectiveThreshold);
    } else {
        // Currently locked - check if should unlock
        return (rssi > effectiveThreshold);
    }
}
