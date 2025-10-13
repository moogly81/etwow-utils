# Bill of Materials (BOM) - Etwow GT Auto-lock System

## 📋 Core Electronics

| Component | Quantity | Part Number | Description | Supplier | Price (Est.) |
|-----------|----------|-------------|-------------|----------|--------------|
| ESP32-S3 Dev Board | 1 | ESP32-S3-DevKitC-1 | 3.3V logic, BLE central, 2×UART | Espressif/Digi-Key | $15-20 |
| nRF52810 Module | 1 | MDBT40-ATR | BLE badge transmitter | Raytac/Digi-Key | $8-12 |
| CR2032 Battery | 1 | CR2032 | 3V lithium coin cell | Generic | $2-3 |
| CR2032 Holder | 1 | CR2032-TH | Through-hole battery holder | Generic | $1-2 |

## ⚡ Power & Connectivity

| Component | Quantity | Part Number | Description | Supplier | Price (Est.) |
|-----------|----------|-------------|-------------|----------|--------------|
| DC-DC Buck Converter | 1 | LM2596S-5.0 | 6-60V to 5V step-down | Generic | $3-5 |
| LDO Regulator | 1 | AMS1117-3.3 | 5V to 3.3V linear regulator | Generic | $1-2 |
| JST-PH Connectors | 2 sets | JST-PH-4P | Inline UART connector set | JST/Digi-Key | $5-8 |
| Dupont Jumpers | 10 | Female-Female | Temporary inline coupling | Generic | $3-5 |
| Power Connector | 1 | JST-XH-2P | 2-pin power connector | JST | $2-3 |

## 🔧 Signal Processing

| Component | Quantity | Part Number | Description | Supplier | Price (Est.) |
|-----------|----------|-------------|-------------|----------|--------------|
| Tri-state Buffer | 1 | 74LVC1T45 | Single-bit bidirectional buffer | TI/Digi-Key | $2-3 |
| Hall Sensor | 2 | SS495A | Analog Hall-effect sensor | Honeywell | $8-10 |
| MOSFET | 1 | BSS138 | N-channel logic-level MOSFET | Infineon | $1-2 |
| Decoupling Caps | 3 | 100nF + 10µF | Ceramic + electrolytic | Generic | $2-3 |
| Resistor Pack | 1 | 0603/0805 | Assorted values (1k, 10k, etc.) | Generic | $5-8 |

## 🛠️ Programming & Debug

| Component | Quantity | Description | Supplier | Price (Est.) |
|-----------|----------|-------------|----------|--------------|
| USB-C Cable | 1 | Data cable for ESP32 programming | Generic | $3-5 |
| USB-TTL Adapter | 1 | CP2102/FT232RL | UART debugging (optional) | Generic | $5-8 |
| SWD Programmer | 1 | J-Link EDU | nRF52 programming (optional) | SEGGER | $60+ |
| Jumper Wires | 1 set | Male/Female mix | Breadboard connections | Generic | $5-8 |

## 🏠 Mechanical & Enclosure

| Component | Quantity | Description | Supplier | Price (Est.) |
|-----------|----------|-------------|----------|--------------|
| Enclosure | 1 | 100×60×30mm | Water-resistant plastic case | Generic | $5-10 |
| PCB Headers | 2 sets | 2.54mm pitch | Locking connectors | Generic | $3-5 |
| Heat Shrink | 1 | Various sizes | Wire protection | Generic | $3-5 |
| Zip Ties | 10 | Various sizes | Cable management | Generic | $2-3 |
| Double-sided Tape | 1 roll | Foam tape | Mounting | Generic | $2-3 |

## 💰 Total Cost Estimate

- **Core Electronics**: $25-35
- **Power & Connectivity**: $15-25  
- **Signal Processing**: $15-20
- **Programming & Debug**: $15-25
- **Mechanical & Enclosure**: $15-25

**Total Project Cost: $85-130**

## 📝 Notes

### Voltage Considerations
- **Scooter Battery**: Typically 36V or 48V
- **Buck Converter**: Must handle input voltage range (6-60V recommended)
- **Logic Levels**: All digital I/O at 3.3V

### Connector Compatibility
- **UART Connector**: Verify JST-PH pinout matches Etwow GT display/BT module
- **Power Connector**: JST-XH recommended for battery connections
- **Programming**: USB-C for ESP32, SWD for nRF52 (optional)

### Sourcing Recommendations
- **Primary**: Digi-Key, Mouser for reliable components
- **Secondary**: Amazon, AliExpress for generic parts
- **Local**: RadioShack, electronics stores for basic components

### Quality Considerations
- **ESP32-S3**: Get official Espressif board for best compatibility
- **nRF52810**: Raytac MDBT40 series is well-supported
- **Connectors**: JST brand recommended for reliability
- **Buck Converter**: LM2596S is proven and reliable

## 🔄 Alternatives & Substitutions

### ESP32 Alternatives
- ESP32-C3 (smaller, single-core)
- ESP32-WROOM-32 (older but cheaper)

### nRF52 Alternatives  
- nRF52811 (more features, higher cost)
- nRF52832 (larger package, more GPIO)

### Power Alternatives
- Linear regulator (simpler, less efficient)
- Switching converter with enable pin (better power management)

### Connector Alternatives
- Terminal blocks (easier wiring, less professional)
- Wire-to-wire crimp connectors (cheaper, less reliable)
