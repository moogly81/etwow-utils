#!/bin/bash
set -e

# -----------------------------
# CONFIG
# -----------------------------
PROJECT_DIR=$(pwd)
TOOLS_DIR="$PROJECT_DIR/tools/arduino-cli"
ARDUINO_CLI_URL="https://github.com/arduino/arduino-cli/releases/download/v1.3.1/arduino-cli_1.3.1_macOS_ARM64.tar.gz"


# -----------------------------
# STEP 1: Download Arduino CLI locally
# -----------------------------
mkdir -p "$TOOLS_DIR"
echo "Downloading Arduino CLI..."
curl -L "$ARDUINO_CLI_URL" -o "$TOOLS_DIR/arduino-cli.tar.gz"
tar -xzf "$TOOLS_DIR/arduino-cli.tar.gz" -C "$TOOLS_DIR"
rm "$TOOLS_DIR/arduino-cli.tar.gz"
chmod +x "$TOOLS_DIR/arduino-cli"

arduino-cli() {
    "$TOOLS_DIR/arduino-cli" --config-file "$TOOLS_DIR/arduino-cli.yaml" "$@"
}


# -----------------------------
# STEP 3: Add Seeed board index and update
# -----------------------------
echo "Adding Seeed board index..."
arduino-cli config add board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
arduino-cli config set directories.data ./tools/arduino-cli/data
arduino-cli config set directories.downloads ./tools/arduino-cli/data/downloads
arduino-cli config set directories.user ./tools/arduino-cli/data/user

arduino-cli core update-index
arduino-cli core install esp32:esp32
