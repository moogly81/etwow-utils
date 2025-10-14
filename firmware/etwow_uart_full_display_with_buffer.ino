/**
 * @file etwow_passthrough_s3_buffered.ino
 * @brief Full-duplex UART passthrough with circular buffers (ESP32-S3)
 *
 * Reliable bidirectional bridge between Display and BT module.
 * BLE badge controls automatic LOCK/UNLOCK injection.
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// ---------------- Configuration ----------------
#define TARGET_MAC  "AA:BB:CC:DD:EE:FF"
#define UNLOCK_RSSI -60
#define LOCK_RSSI   -80
#define UART_BAUD   115200

#define DISP_RX 16
#define DISP_TX 17
#define BT_RX   18
#define BT_TX   19
#define LED_PIN 2

// ---------------- Circular buffer config ----------------
#define BUFFER_SIZE 1024  // bytes per direction

// ---------------- Globals ----------------
HardwareSerial SerialDisplay(1);  // UART1
HardwareSerial SerialBT(2);       // UART2
BLEScan* pBLEScan;
int lastRSSI = -127;
bool isUnlocked = false;
bool commandInProgress = false;
unsigned long lastCommandTime = 0;

// Circular buffers
uint8_t bufDisp2BT[BUFFER_SIZE];
uint8_t bufBT2Disp[BUFFER_SIZE];
volatile size_t headDisp2BT = 0, tailDisp2BT = 0;
volatile size_t headBT2Disp = 0, tailBT2Disp = 0;

inline bool bufferFull(size_t head, size_t tail) {
    return ((head + 1) % BUFFER_SIZE) == tail;
}
inline bool bufferEmpty(size_t head, size_t tail) {
    return head == tail;
}
inline void bufferPush(uint8_t* buf, size_t& head, size_t& tail, uint8_t data) {
    if (!bufferFull(head, tail)) {
        buf[head] = data;
        head = (head + 1) % BUFFER_SIZE;
    }
}
inline int bufferPop(uint8_t* buf, size_t& head, size_t& tail) {
    if (bufferEmpty(head, tail)) return -1;
    uint8_t data = buf[tail];
    tail = (tail + 1) % BUFFER_SIZE;
    return data;
}

// ---------------- BLE Callbacks ----------------
class MyCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice device) override {
        if (device.getAddress().toString() == String(TARGET_MAC).toLowerCase()) {
            lastRSSI = device.getRSSI();
            Serial.printf("[BLE] Badge RSSI=%d dBm\n", lastRSSI);
        }
    }
};

// ---------------- Setup ----------------
void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.println("=== Etwow Auto-lock Passthrough (ESP32-S3, Buffered) ===");

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // UARTs configuration
    SerialDisplay.begin(UART_BAUD, SERIAL_8N1, DISP_RX, DISP_TX);
    SerialBT.begin(UART_BAUD, SERIAL_8N1, BT_RX, BT_TX);
    Serial.println("UART1 (Display) and UART2 (BT) initialized.");

    // BLE initialization
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
    pBLEScan->start(0, nullptr, false);

    Serial.printf("Looking for BLE badge: %s\n", TARGET_MAC);
}

// ---------------- Main Loop ----------------
void loop() {
    readUARTsToBuffers();
    flushBuffersToUARTs();
    handleProximityLogic();
    restartBLEScanIfNeeded();
    delay(1);
}

// ---------------- Buffered UART Bridge ----------------
void readUARTsToBuffers() {
    // Read from Display → buffer
    while (SerialDisplay.available()) {
        uint8_t b = SerialDisplay.read();
        bufferPush(bufDisp2BT, headDisp2BT, tailDisp2BT, b);
#if DEBUG_TRAFFIC
        Serial.write(b);
#endif
    }

    // Read from BT → buffer
    while (SerialBT.available()) {
        uint8_t b = SerialBT.read();
        bufferPush(bufBT2Disp, headBT2Disp, tailBT2Disp, b);
#if DEBUG_TRAFFIC
        Serial.write(b);
#endif
    }
}

void flushBuffersToUARTs() {
    int b;

    // Flush Display → BT
    while ((b = bufferPop(bufDisp2BT, headDisp2BT, tailDisp2BT)) != -1) {
        SerialBT.write((uint8_t)b);
    }

    // Flush BT → Display
    while ((b = bufferPop(bufBT2Disp, headBT2Disp, tailBT2Disp)) != -1) {
        SerialDisplay.write((uint8_t)b);
    }
}

// ---------------- BLE Proximity ----------------
void handleProximityLogic() {
    if (commandInProgress && millis() - lastCommandTime > 500) {
        commandInProgress = false;
    }

    if (lastRSSI > UNLOCK_RSSI && !isUnlocked && !commandInProgress) {
        sendCommand("UNLOCK");
        isUnlocked = true;
        digitalWrite(LED_PIN, HIGH);
        Serial.println("[STATE] UNLOCKED (badge nearby)");
    } else if (lastRSSI < LOCK_RSSI && isUnlocked && !commandInProgress) {
        sendCommand("LOCK");
        isUnlocked = false;
        digitalWrite(LED_PIN, LOW);
        Serial.println("[STATE] LOCKED (badge far)");
    }
}

// ---------------- BLE Scan Maintenance ----------------
void restartBLEScanIfNeeded() {
    if (!pBLEScan->isScanning()) {
        pBLEScan->start(0, nullptr, false);
    }
}

// ---------------- Command Injection ----------------
void sendCommand(const char* cmd) {
    commandInProgress = true;
    lastCommandTime = millis();
    Serial.printf("[CMD] Injecting command: %s\n", cmd);

    // Temporarily pause Display→BT transfer to avoid collision
    noInterrupts();
    size_t oldHead = headDisp2BT, oldTail = tailDisp2BT;
    interrupts();

    SerialBT.printf("%s\n", cmd);
    SerialBT.flush();
    delay(50);

    // Restore buffer pointers if needed
    noInterrupts();
    headDisp2BT = oldHead;
    tailDisp2BT = oldTail;
    interrupts();

    Serial.println("[CMD] Done, passthrough resumed");
}
