#!/bin/bash

# Etwow GT Auto-lock System Test Script 
# This script runs tests for the simple sketches only

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${GREEN}[TEST]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

# Test configuration
TEST_RESULTS_DIR="test_results"
TEST_LOG="$TEST_RESULTS_DIR/test_results.log"

# Create test results directory
mkdir -p "$TEST_RESULTS_DIR"

# Initialize test log
echo "=== Etwow GT Auto-lock System Test Results ===" > "$TEST_LOG"
echo "Test Date: $(date)" >> "$TEST_LOG"
echo "Test Host: $(hostname)" >> "$TEST_LOG"
echo "==============================================" >> "$TEST_LOG"
echo "" >> "$TEST_LOG"

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Function to run a test
run_test() {
    local test_name="$1"
    local test_command="$2"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    print_info "Running test: $test_name"
    
    echo "Test: $test_name" >> "$TEST_LOG"
    echo "Command: $test_command" >> "$TEST_LOG"
    
    if eval "$test_command" >> "$TEST_LOG" 2>&1; then
        print_status "✓ $test_name PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        echo "Result: PASSED" >> "$TEST_LOG"
    else
        print_error "✗ $test_name FAILED"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo "Result: FAILED" >> "$TEST_LOG"
    fi
    
    echo "" >> "$TEST_LOG"
}

# Test 1: Project structure validation
run_test "Project Structure" "scripts/validate.sh"

# Test 2: Code compilation (controller)
run_test "Controller Compilation" "arduino-cli compile --fqbn esp32:esp32:esp32s3 firmware/etwow.ino"

# Test 3: Code compilation (badge)
run_test "Badge Compilation" "arduino-cli compile --fqbn adafruit:adafruit:nrf52840 firmware/badge.ino"

# Test 4: Documentation validation
run_test "Documentation Check" "
    test -f docs/README.md && \
    test -f docs/bom.md && \
    test -f docs/quick-start.md && \
    test -f README.md
"

# Test 5: Script permissions
run_test "Script Permissions" "
    test -x scripts/setup.sh && \
    test -x scripts/build.sh && \
    test -x scripts/test.sh
"

# Test 6: Library dependencies check
run_test "Library Dependencies" "
    arduino-cli lib list | grep -q 'ESP32 BLE Arduino' && \
    arduino-cli lib list | grep -q 'Adafruit Bluefruit nRF52 Libraries'
"

# Test 7: Board support check
run_test "Board Support" "
    arduino-cli core list | grep -q 'esp32:esp32' && \
    arduino-cli core list | grep -q 'adafruit:adafruit'
"

# Test 8: Code quality checks (sketches only)
run_test "Code Quality" "
    grep -l 'TODO\\|FIXME\\|XXX' firmware/*.ino | wc -l | grep -q '^0$'
"

# Generate test report
echo ""
print_status "=== Test Summary ==="
print_status "Total Tests: $TOTAL_TESTS"
print_status "Passed: $PASSED_TESTS"
print_status "Failed: $FAILED_TESTS"

if [ $FAILED_TESTS -eq 0 ]; then
    print_status "✓ All tests passed!"
    echo "Result: ALL TESTS PASSED" >> "$TEST_LOG"
    exit 0
else
    print_error "✗ $FAILED_TESTS test(s) failed"
    echo "Result: $FAILED_TESTS TEST(S) FAILED" >> "$TEST_LOG"
    exit 1
fi
