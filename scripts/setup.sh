#!/bin/bash

# Etwow GT Auto-lock System Setup Script
# This script sets up the development environment for the project

set -e  # Exit on any error

echo "=== Etwow GT Auto-lock System Setup ==="
echo "Setting up development environment..."

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running on macOS, Linux, or Windows
if [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    OS="windows"
else
    print_error "Unsupported operating system: $OSTYPE"
    exit 1
fi

print_status "Detected OS: $OS"

# Check for required tools
check_command() {
    if command -v $1 &> /dev/null; then
        print_status "$1 is installed"
        return 0
    else
        print_warning "$1 is not installed"
        return 1
    fi
}

# Check for Arduino CLI
if ! check_command "arduino-cli"; then
    print_warning "Arduino CLI not found. Installing..."
    
    if [ "$OS" == "macos" ]; then
        if command -v brew &> /dev/null; then
            brew install arduino-cli
        else
            print_error "Homebrew not found. Please install Homebrew first or install Arduino CLI manually."
            exit 1
        fi
    elif [ "$OS" == "linux" ]; then
        # Install Arduino CLI on Linux
        curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
        sudo mv bin/arduino-cli /usr/local/bin/
        rm -rf bin/
    elif [ "$OS" == "windows" ]; then
        print_error "Please install Arduino CLI manually on Windows from: https://arduino.github.io/arduino-cli/"
        exit 1
    fi
fi

# Initialize Arduino CLI
print_status "Initializing Arduino CLI..."
arduino-cli core update-index

# Install ESP32 core
print_status "Installing ESP32 core..."
arduino-cli core install esp32:esp32

# Install nRF52 core (Adafruit)
print_status "Installing Adafruit nRF52 core..."
arduino-cli core install adafruit:adafruit

# Install required libraries
print_status "Installing required libraries..."

# ESP32 libraries
arduino-cli lib install "ESP32 BLE Arduino"

# nRF52 libraries
arduino-cli lib install "Adafruit Bluefruit nRF52 Libraries"

print_status "Library installation complete"

# Create Arduino IDE configuration
print_status "Setting up Arduino IDE configuration..."

# Create arduino-cli.yaml config
cat > arduino-cli.yaml << EOF
board_manager:
  additional_urls:
    - https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
    - https://adafruit.github.io/arduino-board-index/package_adafruit_index.json

library:
  enable_unsafe_install: false
  update_index: true

sketch:
  always_export_binaries: true

daemon:
  port: 50051
EOF

print_status "Arduino CLI configuration created"

# Create build script
print_status "Creating build scripts..."

# Controller build script
cat > scripts/build_controller.sh << 'EOF'
#!/bin/bash
# Build script for ESP32 controller

set -e

echo "Building ESP32 controller..."

# Set build parameters
BOARD="esp32:esp32:esp32s3"
FQBN="esp32:esp32:esp32s3:PartitionScheme=default,FlashMode=qio,FlashSize=8M,UploadSpeed=921600,DebugLevel=none"

# Build the project
arduino-cli compile --fqbn "$FQBN" firmware/controller/

echo "Controller build complete!"
echo "To upload: arduino-cli upload -p /dev/ttyUSB0 --fqbn \"$FQBN\" firmware/controller/"
EOF

# Badge build script
cat > scripts/build_badge.sh << 'EOF'
#!/bin/bash
# Build script for nRF52 badge

set -e

echo "Building nRF52 badge..."

# Set build parameters
BOARD="adafruit:adafruit:nrf52840"
FQBN="adafruit:adafruit:nrf52840:softdevice=s140v6,debug=l0"

# Build the project
arduino-cli compile --fqbn "$FQBN" firmware/badge/

echo "Badge build complete!"
echo "To upload: arduino-cli upload -p /dev/ttyACM0 --fqbn \"$FQBN\" firmware/badge/"
EOF

# Make scripts executable
chmod +x scripts/build_controller.sh
chmod +x scripts/build_badge.sh

print_status "Build scripts created"

# Create validation script
cat > scripts/validate.sh << 'EOF'
#!/bin/bash
# Validation script for the project

set -e

echo "=== Project Validation ==="

# Check project structure
echo "Checking project structure..."
required_dirs=("firmware" "docs" "scripts")
for dir in "${required_dirs[@]}"; do
    if [ -d "$dir" ]; then
        echo "✓ $dir directory exists"
    else
        echo "✗ $dir directory missing"
        exit 1
    fi
done

# Check firmware files
echo "Checking firmware files..."
required_files=(
    "firmware/controller/uart_mim_esp32.ino"
    "firmware/badge/ble_badge_nrf52.ino"
    "firmware/controller/include/config.h"
    "firmware/controller/include/pins.h"
)

for file in "${required_files[@]}"; do
    if [ -f "$file" ]; then
        echo "✓ $file exists"
    else
        echo "✗ $file missing"
        exit 1
    fi
done

# Check documentation
echo "Checking documentation..."
if [ -f "docs/README.md" ] && [ -f "docs/bom.md" ]; then
    echo "✓ Documentation files exist"
else
    echo "✗ Documentation files missing"
    exit 1
fi

echo "✓ Project validation complete!"
EOF

chmod +x scripts/validate.sh

print_status "Validation script created"

# Create development environment file
cat > .env.example << EOF
# Etwow GT Auto-lock System Environment Configuration

# Target badge MAC address (update with your badge's MAC)
TARGET_BADGE_MAC=AA:BB:CC:DD:EE:FF

# RSSI thresholds (dBm)
UNLOCK_RSSI_THRESHOLD=-60
LOCK_RSSI_THRESHOLD=-80
RSSI_HYSTERESIS=5

# UART configuration
UART_BAUD_RATE=115200

# Pin assignments (update for your hardware)
BUS_UART_RX_PIN=16
BUS_UART_TX_PIN=17
OE_PIN=25
LED_PIN=2

# Debug settings
ENABLE_DEBUG_PRINTS=true
ENABLE_UART_FORWARDING=true
EOF

print_status "Environment configuration template created"

# Create gitignore
cat > .gitignore << EOF
# Arduino IDE
*.ino.bak
*.ino.orig

# Build outputs
build/
*.bin
*.elf
*.hex

# IDE files
.vscode/
.idea/
*.swp
*.swo

# OS files
.DS_Store
Thumbs.db

# Environment files
.env

# Logs
*.log

# Temporary files
*.tmp
*.temp
EOF

print_status "Git ignore file created"

# Final status
echo ""
print_status "Setup complete!"
echo ""
echo "Next steps:"
echo "1. Copy .env.example to .env and configure your settings"
echo "2. Update TARGET_BADGE_MAC in firmware/controller/include/config.h"
echo "3. Run 'scripts/validate.sh' to verify the setup"
echo "4. Build the firmware with 'scripts/build_controller.sh' and 'scripts/build_badge.sh'"
echo ""
echo "For detailed instructions, see docs/quick-start.md"
echo ""
