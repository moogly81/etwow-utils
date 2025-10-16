#!/bin/bash
# Cleanup script to remove arduino-cli and all related data
# Usage: ./scripts/cleanup.sh   
brew uninstall arduino-cli

rm -rf ~/Documents/Arduino/libraries
rm -rf ~/Library/Arduino15
rm -rf ~/Library/Caches/arduino