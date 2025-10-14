#!/bin/bash
set -e
for d in firmware docs scripts; do
  [ -d "$d" ] && echo "✓ $d exists" || { echo "✗ $d missing"; exit 1; }
done
for f in firmware/controller/uart_mim_esp32.ino firmware/badge/ble_badge_nrf52.ino firmware/controller/include/config.h firmware/controller/include/pins.h; do
  [ -f "$f" ] && echo "✓ $f exists" || { echo "✗ $f missing"; exit 1; }
done
[ -f "docs/README.md" ] && [ -f "docs/bom.md" ] && echo "✓ Docs exist" || { echo "✗ Docs missing"; exit 1; }
echo "✓ Validation complete!"
