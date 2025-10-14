#!/bin/bash
set -e
FQBN="esp32:esp32:esp32s3:PartitionScheme=default,FlashMode=qio,FlashSize=8M,UploadSpeed=921600,DebugLevel=none"

arduino-cli compile --fqbn "$FQBN" firmware_dev/etwow_inline/01_ble_calibration
arduino-cli upload -p /dev/ttyUSB0 --fqbn \"$FQBN\" firmware_dev/etwow_inline/01_ble_calibration"
