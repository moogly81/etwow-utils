# ESP32 BLE Scanner

Self-contained Arduino project with bundled Arduino CLI.



## Configure

Edit `env` file:
```bash
FQBN="esp32:esp32:m5stack_stamp_s3"  # Change board
PORT="/dev/cu.usbmodem2101"          # Change port
```


## Setup

```bash
./setup.sh          # Downloads Arduino CLI + ESP32 support
source env           # Loads compile/send/monitor functions
```



## Build & Upload

```bash
compile     # Build sketch
send        # Upload to board  
monitor     # Serial output (115200)

# Or: compile && send && monitor
```

## VS Code

- `Cmd+Alt+R` - Compile
- `Cmd+Alt+U` - Upload
- `Cmd+Alt+S` - Monitor