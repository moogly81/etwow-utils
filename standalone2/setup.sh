#!/bin/bash
set -e

# -----------------------------
# CONFIG
# -----------------------------
PROJECT_DIR=$(pwd)
TOOLS_DIR="$PROJECT_DIR/tools/arduino-cli"
DATA_DIR="$TOOLS_DIR/data"
SKETCH_DIR="$PROJECT_DIR/controller"
FQBN="Seeeduino:nrf52:xiaonRF52840Sense"
PORT="/dev/tty.usbmodemXXXX"   # <- change this to your actual port if uploading
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

# -----------------------------
# STEP 2: Initialize local config
# -----------------------------
echo "Initializing Arduino CLI config..."
"$TOOLS_DIR/arduino-cli" config init --overwrite --config-file arduino-cli.yaml
"$TOOLS_DIR/arduino-cli" config set data_dir "$DATA_DIR" --config-file arduino-cli.yaml

# -----------------------------
# STEP 3: Add Seeed board index and update
# -----------------------------
echo "Adding Seeed board index..."
"$TOOLS_DIR/arduino-cli" config add board_manager.additional_urls https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
"$TOOLS_DIR/arduino-cli" core update-index

# -----------------------------
# STEP 4: Install Seeed nRF52 core
# -----------------------------
echo "Installing Seeed nRF52 core..."
"$TOOLS_DIR/arduino-cli" core install Seeeduino:nrf52

# -----------------------------
# STEP 5: Fix permissions for adafruit-nrfutil
# -----------------------------
NRFUTIL_PATH="$DATA_DIR/packages/Seeeduino/hardware/nrf52/1.1.10/tools/adafruit-nrfutil/macos/adafruit-nrfutil"
if [ -f "$NRFUTIL_PATH" ]; then
    echo "Fixing permissions for adafruit-nrfutil..."
    chmod +x "$NRFUTIL_PATH"
fi

# -----------------------------
# STEP 6: Compile sketch
# -----------------------------
echo "Compiling sketch..."
"$TOOLS_DIR/arduino-cli" compile --fqbn "$FQBN" "$SKETCH_DIR"

# -----------------------------
# STEP 7 (optional): Upload sketch
# -----------------------------
if [ "$PORT" != "NONE" ]; then
    echo "Uploading sketch to $PORT..."
    "$TOOLS_DIR/arduino-cli" upload -p "$PORT" --fqbn "$FQBN" "$SKETCH_DIR"
fi

echo "Done!"
