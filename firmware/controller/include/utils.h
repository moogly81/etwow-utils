/**
 * @file utils.h
 * @brief Utility functions and helper classes
 */

#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

/**
 * @brief Read a line from serial input non-blocking
 * @param s HardwareSerial reference
 * @return String containing the line, empty if no complete line available
 */
String readLineNonBlocking(HardwareSerial &s);

/**
 * @brief Convert MAC address string to lowercase
 * @param mac MAC address string
 * @return Lowercase MAC address string
 */
String macToLowercase(const String& mac);

/**
 * @brief Validate MAC address format
 * @param mac MAC address string to validate
 * @return true if MAC format is valid
 */
bool isValidMAC(const String& mac);

/**
 * @brief Get current system uptime in readable format
 * @return String with uptime information
 */
String getSystemUptime();

/**
 * @brief Print system information to serial
 */
void printSystemInfo();

/**
 * @brief Safe string copy with bounds checking
 * @param dest Destination buffer
 * @param src Source string
 * @param destSize Destination buffer size
 * @return true if copy successful, false if truncated
 */
bool safeStringCopy(char* dest, const char* src, size_t destSize);

/**
 * @brief Convert RSSI to approximate distance
 * @param rssi RSSI value in dBm
 * @return Approximate distance in meters (rough estimate)
 */
float rssiToDistance(int rssi);

/**
 * @brief Check if system is in debug mode
 * @return true if debug prints are enabled
 */
bool isDebugMode();

/**
 * @brief Print debug message if debug mode is enabled
 * @param format Printf-style format string
 * @param ... Arguments for format string
 */
void debugPrint(const char* format, ...);

/**
 * @brief Calculate moving average of RSSI readings
 */
class MovingAverage {
private:
    int* values;
    int size;
    int index;
    int count;
    
public:
    MovingAverage(int bufferSize);
    ~MovingAverage();
    
    void addValue(int value);
    int getAverage() const;
    void reset();
    bool isFull() const;
};

#endif // UTILS_H
