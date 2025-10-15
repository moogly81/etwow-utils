# Quick Start — arduino-cli only (no fuss)

Follow these minimal steps to build, flash and monitor the badge and controller using arduino-cli on macOS / Linux.

1) Install arduino-cli (if you don't have it)

```bash
# macOS (Homebrew)
brew install arduino-cli

# Linux (example using install script)
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
sudo mv bin/arduino-cli /usr/local/bin/
```

2) Add board URLs and install cores + libraries

```bash
arduino-cli config init
arduino-cli config set board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json,https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
arduino-cli core update-index
arduino-cli core install esp32:esp32
arduino-cli core install adafruit:nrf52
arduino-cli lib update-index
arduino-cli lib install "Adafruit BusIO"
arduino-cli lib install "Adafruit nRFCrypto"
```

3) Build sketches (sketch folder name must match the .ino file)

```bash
# Build controller (ESP32-S3)
arduino-cli compile --fqbn "esp32:esp32:esp32s3:PartitionScheme=default,FlashMode=qio,FlashSize=8M,UploadSpeed=921600,DebugLevel=none" sketches/controller

# Build badge (nRF52840)
arduino-cli compile --fqbn "adafruit:nrf52:nrf52840:softdevice=s140v6,debug=l0" sketches/badge
```

4) Upload (replace PORT with your device)

```bash
# Upload controller (example port)
arduino-cli upload -p /dev/tty.usbserial-XXXX --fqbn "esp32:esp32:esp32s3:PartitionScheme=default,FlashMode=qio,FlashSize=8M,UploadSpeed=921600,DebugLevel=none" sketches/controller

# Upload badge (example port)
arduino-cli upload -p /dev/tty.usbmodem-XXXX --fqbn "adafruit:nrf52:nrf52840:softdevice=s140v6,debug=l0" sketches/badge
```

5) Open serial monitor (115200)

```bash
arduino-cli monitor -p /dev/tty.usbserial-XXXX -b 115200
```

6) Quick notes
- Edit `sketches/controller/etwow.ino` to set your badge MAC address (TARGET_MAC) and pin defines.
- Sketch folder name must match the top-level .ino filename for arduino-cli to work (we placed sketches in `sketches/controller` and `sketches/badge`).
- If you prefer GUI, the same sketches can be opened in the Arduino IDE (File → Open → select the sketch folder).

That's it — minimal commands to get you building and flashing with arduino-cli.
