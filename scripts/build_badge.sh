#!/bin/bash
set -e
FQBN="adafruit:nrf52:feather52832:debug=l0"
arduino-cli compile --fqbn "$FQBN" "firmware_dev/badge/01_badge_toggle_led"