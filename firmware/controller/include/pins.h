/**
 * @file pins.h
 * @brief Pin definitions for ESP32 controller hardware
 * 
 * Update these pin assignments to match your specific hardware configuration.
 * This file centralizes all pin definitions for easy modification.
 */

#ifndef PINS_H
#define PINS_H

// UART Configuration
#define BUS_UART_NUM 1                        // UART peripheral number
#define BUS_UART_RX_PIN 16                    // RX pin for bus communication
#define BUS_UART_TX_PIN 17                    // TX pin for bus communication

// Tri-state Buffer Control
#define OE_PIN 25                             // Output Enable pin for tri-state buffer

// Hall Effect Sensors (Analog Input)
#define HALL_SENSOR_1_PIN 34                  // First hall sensor (throttle)
#define HALL_SENSOR_2_PIN 35                  // Second hall sensor (brake)

// Status and Control Pins
#define LED_PIN 2                             // Status LED pin
#define POWER_ENABLE_PIN 4                    // Power enable pin (optional)
#define DEBUG_PIN 5                           // Debug output pin (optional)

// I2C Pins (if using I2C peripherals)
#define I2C_SDA_PIN 21                        // I2C Data pin
#define I2C_SCL_PIN 22                        // I2C Clock pin

// SPI Pins (if using SPI peripherals)
#define SPI_MOSI_PIN 23                       // SPI Master Out Slave In
#define SPI_MISO_PIN 19                       // SPI Master In Slave Out
#define SPI_SCK_PIN 18                        // SPI Clock
#define SPI_CS_PIN 5                          // SPI Chip Select

// Analog Input Configuration
#define ANALOG_READ_RESOLUTION 12             // 12-bit ADC resolution
#define ANALOG_READ_SAMPLES 8                 // Number of samples for averaging

// Digital Input Configuration
#define INPUT_PULLUP_ENABLED true             // Enable pull-up resistors for inputs

// Pin Mode Definitions
#define OUTPUT_PINS {OE_PIN, LED_PIN, POWER_ENABLE_PIN, DEBUG_PIN}
#define INPUT_PINS {HALL_SENSOR_1_PIN, HALL_SENSOR_2_PIN}
#define ANALOG_PINS {HALL_SENSOR_1_PIN, HALL_SENSOR_2_PIN}

// Pin State Definitions
#define PIN_STATE_HIGH HIGH
#define PIN_STATE_LOW LOW
#define PIN_STATE_TRI_STATE LOW               // Tri-state buffer disabled

#endif // PINS_H
