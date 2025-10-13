# Quick Start Guide - Etwow GT Auto-lock System

## 🚀 Get Running in 30 Minutes

This guide will get your auto-lock system operational quickly. For detailed information, see the [full documentation](./README.md).

## 📋 Prerequisites

- **Hardware**: All components from [BOM](./bom.md)
- **Software**: Arduino IDE 2.0+ with ESP32 and nRF52 support
- **Tools**: Soldering iron, multimeter, wire strippers
- **Time**: 30-60 minutes for software setup

## ⚡ Quick Setup

### 1. Install Arduino IDE & Libraries

```bash
# Download Arduino IDE 2.0+ from https://www.arduino.cc/en/software
# Install ESP32 board support:
# File → Preferences → Additional Board Manager URLs:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

# Install nRF52 board support:
# File → Preferences → Additional Board Manager URLs:
https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
```

### 2. Flash the BLE Badge

```bash
# 1. Open firmware/badge/ble_badge_nrf52.ino
# 2. Select Board: "Adafruit Bluefruit nRF52 Feather"
# 3. Select Port: Your nRF52 device
# 4. Upload the code
# 5. Note the MAC address from Serial Monitor
```

### 3. Configure ESP32 Controller

```bash
# 1. Open firmware/controller/uart_mim_esp32.ino
# 2. Update targetAddress in code with your badge MAC
# 3. Select Board: "ESP32S3 Dev Module"
# 4. Configure pins in include/pins.h for your setup
# 5. Upload the code
```

### 4. Test Basic Functionality

```bash
# 1. Power on both devices
# 2. Open Serial Monitor (115200 baud)
# 3. Bring badge close to ESP32
# 4. Verify RSSI readings appear
# 5. Test lock/unlock commands manually
```

## 🔧 Hardware Installation

### Minimal Test Setup

1. **Power ESP32** from USB (for testing)
2. **Connect UART** to test points (not inline yet)
3. **Monitor Serial** output for debugging
4. **Test BLE scanning** and RSSI detection

### Production Installation

1. **Mount ESP32** in waterproof enclosure
2. **Connect power** from scooter battery via buck converter
3. **Install inline** between display and BT module
4. **Test lock/unlock** functionality
5. **Calibrate RSSI thresholds** for your environment

## ⚙️ Quick Configuration

### RSSI Thresholds (Default)
```cpp
#define UNLOCK_THRESHOLD -60  // dBm - close proximity
#define LOCK_THRESHOLD   -80  // dBm - far away
#define HYSTERESIS       5    // dBm - prevents rapid toggling
```

### Pin Configuration
Edit `include/pins.h` for your hardware:
```cpp
#define BUS_UART_RX_PIN 16    // UART RX from display
#define BUS_UART_TX_PIN 17    // UART TX to BT module  
#define OE_PIN 25             // Output enable for tri-state buffer
#define LED_PIN 2             // Status LED
```

## 🧪 Testing Checklist

- [ ] Badge advertises and is detected
- [ ] ESP32 shows RSSI readings in Serial Monitor
- [ ] Lock/unlock commands are sent when thresholds crossed
- [ ] UART passthrough works (no interference)
- [ ] System powers on/off cleanly
- [ ] Enclosure is weatherproof

## 🆘 Quick Troubleshooting

### Badge Not Detected
- Check MAC address configuration
- Verify badge is powered and advertising
- Test with BLE scanner app

### No RSSI Readings
- Check ESP32 BLE initialization
- Verify target MAC address format
- Test with BLE scanning example

### Lock/Unlock Not Working
- Check UART connection and baud rate
- Verify tri-state buffer operation
- Test with UART passthrough example

### Power Issues
- Check buck converter output voltage (5V)
- Verify ESP32 power requirements
- Test with USB power first

## 📞 Next Steps

- **Full Installation**: See [Hardware Assembly Guide](./hardware/assembly.md)
- **Advanced Config**: See [Configuration Guide](./configuration/parameters.md)
- **Troubleshooting**: See [Common Issues](./troubleshooting/common-issues.md)
- **Development**: See [Development Guide](./development/setup.md)

## ⚠️ Safety Notes

- **Test thoroughly** before permanent installation
- **Use proper fusing** for battery connections
- **Ensure waterproofing** of all connections
- **Follow local laws** regarding scooter modifications
- **Backup original system** before modifications
