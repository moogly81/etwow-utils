#!/bin/bash

# Etwow GT Auto-lock System Validation Script (Simple Only)
# This script validates the project structure for the simple sketches only

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
check_item "Firmware directory exists" "test -d firmware"

# Check main firmware files
echo ""
echo "Checking main firmware files..."
check_item "Controller sketch exists" "test -f firmware/etwow.ino"
check_item "Badge sketch exists" "test -f firmware/badge.ino"

echo ""
echo "Skipping complex include files (not used in simple mode)"

echo ""
echo "Skipping complex source files (not used in simple mode)"

echo ""
echo "Skipping examples (removed in simple mode)"

# Check documentation files
echo ""
echo "Checking documentation files..."
check_item "Main README exists" "test -f README.md"
check_item "Documentation README exists" "test -f docs/README.md"
check_item "BOM exists" "test -f docs/bom.md"
check_item "Quick start guide exists" "test -f docs/quick-start.md"

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

echo ""
echo "Checking sketch content..."
check_item "Controller defines TARGET_MAC" "grep -q 'TARGET_MAC' firmware/etwow.ino"
check_item "Controller defines thresholds" "grep -q 'UNLOCK_RSSI' firmware/etwow.ino"

echo ""
echo "Checking code dependencies..."
check_item "Controller includes BLE libraries" "grep -q 'BLEDevice.h' firmware/etwow.ino"
check_item "Badge includes Bluefruit library" "grep -q 'bluefruit.h' firmware/badge.ino"

echo ""
echo "Checking file sizes..."
check_item "Controller sketch has content" "test -s firmware/etwow.ino"
check_item "Badge sketch has content" "test -s firmware/badge.ino"

echo ""
echo "Checking for common issues..."
check_item "No TODO comments in sketches" "! grep -q 'TODO\\|FIXME\\|XXX' firmware/etwow.ino firmware/badge.ino"
check_item "No placeholder MAC address left" "! grep -q 'AA:BB:CC:DD:EE:FF' firmware/etwow.ino"

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
