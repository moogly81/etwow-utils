/**
 * @file utils.cpp
 * @brief Implementation of utility functions
 */

#include "utils.h"
#include <stdarg.h>

String readLineNonBlocking(HardwareSerial &s) {
    String out = "";
    while (s.available()) {
        char c = s.read();
        if (c == '\r') continue;
        if (c == '\n') break;
        out += c;
    }
    return out;
}

String macToLowercase(const String& mac) {
    String result = mac;
    result.toLowerCase();
    return result;
}

bool isValidMAC(const String& mac) {
    // Basic MAC address format validation (AA:BB:CC:DD:EE:FF)
    if (mac.length() != 17) {
        return false;
    }
    
    for (int i = 0; i < 17; i++) {
        char c = mac.charAt(i);
        if (i % 3 == 2) {
            // Should be colon
            if (c != ':') {
                return false;
            }
        } else {
            // Should be hex digit
            if (!((c >= '0' && c <= '9') || 
                  (c >= 'A' && c <= 'F') || 
                  (c >= 'a' && c <= 'f'))) {
                return false;
            }
        }
    }
    
    return true;
}

String getSystemUptime() {
    unsigned long uptime = millis();
    unsigned long seconds = uptime / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    unsigned long days = hours / 24;
    
    String result = "";
    if (days > 0) {
        result += String(days) + "d ";
    }
    if (hours % 24 > 0) {
        result += String(hours % 24) + "h ";
    }
    if (minutes % 60 > 0) {
        result += String(minutes % 60) + "m ";
    }
    result += String(seconds % 60) + "s";
    
    return result;
}

void printSystemInfo() {
    Serial.println("=== System Information ===");
    Serial.printf("Firmware Version: %s\n", FIRMWARE_VERSION);
    Serial.printf("Hardware Version: %s\n", HARDWARE_VERSION);
    Serial.printf("Build Date: %s\n", BUILD_DATE);
    Serial.printf("System Uptime: %s\n", getSystemUptime().c_str());
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Chip Model: %s\n", ESP.getChipModel());
    Serial.printf("Chip Revision: %d\n", ESP.getChipRevision());
    Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Flash Size: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
    Serial.println("==========================");
}

bool safeStringCopy(char* dest, const char* src, size_t destSize) {
    if (!dest || !src || destSize == 0) {
        return false;
    }
    
    size_t srcLen = strlen(src);
    if (srcLen >= destSize) {
        // Truncate
        strncpy(dest, src, destSize - 1);
        dest[destSize - 1] = '\0';
        return false;
    } else {
        strcpy(dest, src);
        return true;
    }
}

float rssiToDistance(int rssi) {
    // Rough estimation based on free space path loss
    // This is a simplified model and may not be accurate in all environments
    if (rssi >= -30) return 1.0;   // Very close
    if (rssi >= -40) return 2.0;   // Close
    if (rssi >= -50) return 4.0;   // Near
    if (rssi >= -60) return 8.0;   // Medium
    if (rssi >= -70) return 16.0;  // Far
    if (rssi >= -80) return 32.0;  // Very far
    if (rssi >= -90) return 64.0;  // Extremely far
    return 128.0;  // Out of range
}

bool isDebugMode() {
    return ENABLE_DEBUG_PRINTS;
}

void debugPrint(const char* format, ...) {
    if (!ENABLE_DEBUG_PRINTS) {
        return;
    }
    
    va_list args;
    va_start(args, format);
    
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    Serial.print("[DEBUG] ");
    Serial.println(buffer);
    
    va_end(args);
}

// MovingAverage class implementation
MovingAverage::MovingAverage(int bufferSize) 
    : size(bufferSize), index(0), count(0) {
    values = new int[size];
    reset();
}

MovingAverage::~MovingAverage() {
    delete[] values;
}

void MovingAverage::addValue(int value) {
    values[index] = value;
    index = (index + 1) % size;
    if (count < size) {
        count++;
    }
}

int MovingAverage::getAverage() const {
    if (count == 0) {
        return 0;
    }
    
    long sum = 0;
    for (int i = 0; i < count; i++) {
        sum += values[i];
    }
    
    return sum / count;
}

void MovingAverage::reset() {
    index = 0;
    count = 0;
    for (int i = 0; i < size; i++) {
        values[i] = 0;
    }
}

bool MovingAverage::isFull() const {
    return count == size;
}
