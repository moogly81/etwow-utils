#!/bin/bash

# Etwow GT Auto-lock System Validation Script
# This script validates the project structure and configuration

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${GREEN}[VALIDATE]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Validation counters
TOTAL_CHECKS=0
PASSED_CHECKS=0
FAILED_CHECKS=0

# Function to check and report
check_item() {
    local description="$1"
    local check_command="$2"
    
    TOTAL_CHECKS=$((TOTAL_CHECKS + 1))
    
    if eval "$check_command" > /dev/null 2>&1; then
        print_status "✓ $description"
        PASSED_CHECKS=$((PASSED_CHECKS + 1))
        return 0
    else
        print_error "✗ $description"
        FAILED_CHECKS=$((FAILED_CHECKS + 1))
        return 1
    fi
}

echo "=== Etwow GT Auto-lock System Validation ==="
echo ""

# Check project structure
echo "Checking project structure..."
check_item "Root directory exists" "test -d ."
check_item "Firmware directory exists" "test -d firmware"
check_item "Documentation directory exists" "test -d docs"
check_item "Scripts directory exists" "test -d scripts"

# Check firmware structure
echo ""
echo "Checking firmware structure..."
check_item "Controller directory exists" "test -d firmware/controller"
check_item "Badge directory exists" "test -d firmware/badge"
check_item "Examples directory exists" "test -d firmware/examples"

# Check main firmware files
echo ""
echo "Checking main firmware files..."
check_item "Controller main file exists" "test -f firmware/controller/uart_mim_esp32.ino"
check_item "Badge main file exists" "test -f firmware/badge/ble_badge_nrf52.ino"

# Check include files
echo ""
echo "Checking include files..."
check_item "Config header exists" "test -f firmware/controller/include/config.h"
check_item "Pins header exists" "test -f firmware/controller/include/pins.h"
check_item "BLE manager header exists" "test -f firmware/controller/include/ble_manager.h"
check_item "UART manager header exists" "test -f firmware/controller/include/uart_manager.h"
check_item "Proximity controller header exists" "test -f firmware/controller/include/proximity_controller.h"
check_item "Utils header exists" "test -f firmware/controller/include/utils.h"

# Check source files
echo ""
echo "Checking source files..."
check_item "BLE manager source exists" "test -f firmware/controller/src/ble_manager.cpp"
check_item "UART manager source exists" "test -f firmware/controller/src/uart_manager.cpp"
check_item "Proximity controller source exists" "test -f firmware/controller/src/proximity_controller.cpp"
check_item "Utils source exists" "test -f firmware/controller/src/utils.cpp"

# Check example files
echo ""
echo "Checking example files..."
check_item "UART monitor example exists" "test -f firmware/examples/01_uart_monitor/uart_monitor.ino"
check_item "UART passthrough example exists" "test -f firmware/examples/02_uart_passthrough/uart_passthrough.ino"
check_item "BLE scanner example exists" "test -f firmware/examples/03_ble_scanner/ble_scanner.ino"

# Check documentation files
echo ""
echo "Checking documentation files..."
check_item "Main README exists" "test -f README.md"
check_item "Documentation README exists" "test -f docs/README.md"
check_item "BOM exists" "test -f docs/bom.md"
check_item "Quick start guide exists" "test -f docs/quick-start.md"
check_item "Installation guide exists" "test -f docs/installation/README.md"

# Check script files
echo ""
echo "Checking script files..."
check_item "Setup script exists" "test -f scripts/setup.sh"
check_item "Build script exists" "test -f scripts/build.sh"
check_item "Test script exists" "test -f scripts/test.sh"
check_item "Validate script exists" "test -f scripts/validate.sh"

# Check script permissions
echo ""
echo "Checking script permissions..."
check_item "Setup script is executable" "test -x scripts/setup.sh"
check_item "Build script is executable" "test -x scripts/build.sh"
check_item "Test script is executable" "test -x scripts/test.sh"
check_item "Validate script is executable" "test -x scripts/validate.sh"

# Check configuration content
echo ""
echo "Checking configuration content..."
check_item "Config file contains target MAC" "grep -q 'TARGET_BADGE_MAC' firmware/controller/include/config.h"
check_item "Config file contains RSSI thresholds" "grep -q 'UNLOCK_RSSI_THRESHOLD' firmware/controller/include/config.h"
check_item "Pins file contains UART pins" "grep -q 'BUS_UART_RX_PIN' firmware/controller/include/pins.h"
check_item "Pins file contains OE pin" "grep -q 'OE_PIN' firmware/controller/include/pins.h"

# Check for required dependencies in code
echo ""
echo "Checking code dependencies..."
check_item "Controller includes BLE libraries" "grep -q 'BLEDevice.h' firmware/controller/uart_mim_esp32.ino"
check_item "Badge includes Bluefruit library" "grep -q 'bluefruit.h' firmware/badge/ble_badge_nrf52.ino"
check_item "Controller includes config" "grep -q 'config.h' firmware/controller/uart_mim_esp32.ino"

# Check file sizes (basic sanity check)
echo ""
echo "Checking file sizes..."
check_item "Controller main file has content" "test -s firmware/controller/uart_mim_esp32.ino"
check_item "Badge main file has content" "test -s firmware/badge/ble_badge_nrf52.ino"
check_item "Config file has content" "test -s firmware/controller/include/config.h"
check_item "Pins file has content" "test -s firmware/controller/include/pins.h"

# Check for common issues
echo ""
echo "Checking for common issues..."
check_item "No TODO comments in main files" "! grep -q 'TODO\\|FIXME\\|XXX' firmware/controller/uart_mim_esp32.ino firmware/badge/ble_badge_nrf52.ino"
check_item "No hardcoded MAC addresses" "! grep -q 'AA:BB:CC:DD:EE:FF' firmware/controller/uart_mim_esp32.ino"
check_item "No syntax errors in headers" "! grep -q 'error\\|Error\\|ERROR' firmware/controller/include/*.h"

# Generate validation report
echo ""
echo "=== Validation Summary ==="
echo "Total Checks: $TOTAL_CHECKS"
echo "Passed: $PASSED_CHECKS"
echo "Failed: $FAILED_CHECKS"

if [ $FAILED_CHECKS -eq 0 ]; then
    print_status "✓ All validation checks passed!"
    echo ""
    echo "Project structure is valid and ready for development."
    exit 0
else
    print_error "✗ $FAILED_CHECKS validation check(s) failed"
    echo ""
    echo "Please fix the failed checks before proceeding."
    exit 1
fi
