#!/bin/bash

# Etwow GT Auto-lock System Build Script
# This script builds all firmware components

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${GREEN}[BUILD]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Default values
BUILD_CONTROLLER=true
BUILD_BADGE=true
UPLOAD=false
PORT=""
VERBOSE=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --controller-only)
            BUILD_CONTROLLER=true
            BUILD_BADGE=false
            shift
            ;;
        --badge-only)
            BUILD_CONTROLLER=false
            BUILD_BADGE=true
            shift
            ;;
        --upload)
            UPLOAD=true
            shift
            ;;
        --port)
            PORT="$2"
            shift 2
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  --controller-only    Build only the ESP32 controller"
            echo "  --badge-only        Build only the nRF52 badge"
            echo "  --upload            Upload after building"
            echo "  --port PORT         Serial port for upload"
            echo "  --verbose           Enable verbose output"
            echo "  --help              Show this help"
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Set verbose flag for arduino-cli
ARDUINO_CLI_VERBOSE=""
if [ "$VERBOSE" = true ]; then
    ARDUINO_CLI_VERBOSE="--verbose"
fi

print_status "Starting build process..."

# Function to build ESP32 controller
build_controller() {
    print_status "Building ESP32 controller..."
    
    # ESP32-S3 configuration
    CONTROLLER_FQBN="esp32:esp32:esp32s3:PartitionScheme=default,FlashMode=qio,FlashSize=8M,UploadSpeed=921600,DebugLevel=none"
    
    # Compile controller
    if arduino-cli compile $ARDUINO_CLI_VERBOSE --fqbn "$CONTROLLER_FQBN" firmware/controller/; then
        print_status "Controller build successful!"
        
        if [ "$UPLOAD" = true ]; then
            if [ -z "$PORT" ]; then
                print_warning "No port specified for upload. Please specify with --port"
            else
                print_status "Uploading controller to $PORT..."
                if arduino-cli upload $ARDUINO_CLI_VERBOSE -p "$PORT" --fqbn "$CONTROLLER_FQBN" firmware/controller/; then
                    print_status "Controller upload successful!"
                else
                    print_error "Controller upload failed!"
                    exit 1
                fi
            fi
        fi
    else
        print_error "Controller build failed!"
        exit 1
    fi
}

# Function to build nRF52 badge
build_badge() {
    print_status "Building nRF52 badge..."
    
    # nRF52840 configuration (adjust for your specific nRF52 module)
    BADGE_FQBN="adafruit:adafruit:nrf52840:softdevice=s140v6,debug=l0"
    
    # Compile badge
    if arduino-cli compile $ARDUINO_CLI_VERBOSE --fqbn "$BADGE_FQBN" firmware/badge/; then
        print_status "Badge build successful!"
        
        if [ "$UPLOAD" = true ]; then
            if [ -z "$PORT" ]; then
                print_warning "No port specified for upload. Please specify with --port"
            else
                print_status "Uploading badge to $PORT..."
                if arduino-cli upload $ARDUINO_CLI_VERBOSE -p "$PORT" --fqbn "$BADGE_FQBN" firmware/badge/; then
                    print_status "Badge upload successful!"
                else
                    print_error "Badge upload failed!"
                    exit 1
                fi
            fi
        fi
    else
        print_error "Badge build failed!"
        exit 1
    fi
}

# Build components based on flags
if [ "$BUILD_CONTROLLER" = true ]; then
    build_controller
fi

if [ "$BUILD_BADGE" = true ]; then
    build_badge
fi

print_status "Build process complete!"

# Show build artifacts
echo ""
print_status "Build artifacts:"
if [ "$BUILD_CONTROLLER" = true ]; then
    echo "  Controller: firmware/controller/build/"
fi
if [ "$BUILD_BADGE" = true ]; then
    echo "  Badge: firmware/badge/build/"
fi

echo ""
echo "Next steps:"
echo "1. Flash the badge firmware to your nRF52 module"
echo "2. Note the badge's MAC address from Serial Monitor"
echo "3. Update TARGET_BADGE_MAC in config.h with your badge's MAC"
echo "4. Flash the controller firmware to your ESP32"
echo "5. Test the system with the examples in firmware/examples/"
echo ""
echo "For detailed instructions, see docs/quick-start.md"
