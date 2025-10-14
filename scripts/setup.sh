#!/bin/bash
set -e
echo "=== Etwow GT Auto-lock Setup ==="

# OS detection
case "$OSTYPE" in darwin*) OS="macos";; linux-gnu*) OS="linux";; msys*|cygwin*) OS="windows";; *) echo "[ERROR] Unsupported OS: $OSTYPE"; exit 1;; esac
echo "[INFO] Detected OS: $OS"

# Arduino CLI check/install
if ! command -v arduino-cli &>/dev/null; then
  echo "[WARN] Arduino CLI not found."
  if [ "$OS" = "macos" ] && command -v brew &>/dev/null; then brew install arduino-cli
  elif [ "$OS" = "linux" ]; then curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh; sudo mv bin/arduino-cli /usr/local/bin/; rm -rf bin/
  else echo "[ERROR] Install Arduino CLI manually"; exit 1; fi
fi


# Arduino CLI config
cat > arduino-cli.yaml <<EOF
board_manager:
  additional_urls:
    - https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
    - https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
library:
  enable_unsafe_install: false
  update_index: true
sketch: { always_export_binaries: true }
daemon: { port: 50051 }
EOF


CONFIG="--config-file arduino-cli.yaml"
arduino-cli core update-index $CONFIG

install_core() { core="$1"; arduino-cli core list $CONFIG | grep -q "^$core" || arduino-cli core install "$core" $CONFIG; }
install_core "esp32:esp32"
install_core "adafruit:nrf52"

# Libraries
arduino-cli lib install "ESP32 BLE Arduino" $CONFIG
arduino-cli core install adafruit:nrf52 --config-file arduino-cli.yaml
arduino-cli lib install "adafruit nrf52" $CONFIG

# Scripts
mkdir -p scripts
cat > scripts/build_controller.sh <<'EOF'
#!/bin/bash
set -e
FQBN="esp32:esp32:esp32s3:PartitionScheme=default,FlashMode=qio,FlashSize=8M,UploadSpeed=921600,DebugLevel=none"
arduino-cli compile --fqbn "$FQBN" firmware/controller/
echo "Upload: arduino-cli upload -p /dev/ttyUSB0 --fqbn \"$FQBN\" firmware/controller/"
EOF
cat > scripts/build_badge.sh <<'EOF'
#!/bin/bash
set -e
FQBN="adafruit:nrf52:nrf52840:softdevice=s140v6,debug=l0"
arduino-cli compile --fqbn "$FQBN" firmware/badge/
echo "Upload: arduino-cli upload -p /dev/ttyACM0 --fqbn \"$FQBN\" firmware/badge/"
EOF
chmod +x scripts/*.sh

# Validation
cat > scripts/validate.sh <<'EOF'
#!/bin/bash
set -e
for d in firmware docs scripts; do [ -d "$d" ] && echo "✓ $d exists" || { echo "✗ $d missing"; exit 1; }; done
for f in firmware/controller/uart_mim_esp32.ino firmware/badge/ble_badge_nrf52.ino firmware/controller/include/config.h firmware/controller/include/pins.h; do [ -f "$f" ] && echo "✓ $f exists" || { echo "✗ $f missing"; exit 1; }; done
[ -f "docs/README.md" ] && [ -f "docs/bom.md" ] && echo "✓ Docs exist" || { echo "✗ Docs missing"; exit 1; }
echo "✓ Validation complete!"
EOF
chmod +x scripts/validate.sh

# Env
cat > .env.example <<EOF
TARGET_BADGE_MAC=AA:BB:CC:DD:EE:FF
UNLOCK_RSSI_THRESHOLD=-60
LOCK_RSSI_THRESHOLD=-80
RSSI_HYSTERESIS=5
UART_BAUD_RATE=115200
BUS_UART_RX_PIN=16
BUS_UART_TX_PIN=17
OE_PIN=25
LED_PIN=2
ENABLE_DEBUG_PRINTS=true
ENABLE_UART_FORWARDING=true
EOF

# Git ignore
cat > .gitignore <<EOF
*.ino.bak *.ino.orig build/ *.bin *.elf *.hex .vscode/ .idea/ *.swp *.swo .DS_Store Thumb.db .env *.log *.tmp *.temp
EOF

echo "[INFO] Setup complete! Copy .env.example to .env and configure."
