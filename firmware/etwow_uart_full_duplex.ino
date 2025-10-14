/**
 * @file etwow_passthrough_s3.ino
 * @brief Full-duplex UART passthrough for Etwow GT (ESP32-S3)
 *
 * Display <--> ESP32-S3 <--> BT module
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
 
 // Display UART
 #define DISP_RX 16  // ESP32-S3 RX from Display TX
 #define DISP_TX 17  // ESP32-S3 TX to Display RX
 
 // Bluetooth Module UART
 #define BT_RX   18  // ESP32-S3 RX from BT module TX
 #define BT_TX   19  // ESP32-S3 TX to BT module RX
 
 #define LED_PIN 2
 
 // ---------------- Globals ----------------
 HardwareSerial SerialDisplay(1);  // UART1
 HardwareSerial SerialBT(2);       // UART2
 BLEScan* pBLEScan;
 int lastRSSI = -127;
 bool isUnlocked = false;
 bool commandInProgress = false;
 unsigned long lastCommandTime = 0;
 
 // ---------------- BLE Callback ----------------
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
     Serial.println("=== Etwow Auto-lock Passthrough (ESP32-S3) ===");
 
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
     handlePassthrough();
     handleProximityLogic();
     restartBLEScanIfNeeded();
     delay(1);
 }
 
 // ---------------- UART Full-duplex Bridge ----------------
 void handlePassthrough() {
     // Display → BT
     while (SerialDisplay.available()) {
         uint8_t b = SerialDisplay.read();
         SerialBT.write(b);
 #if DEBUG_TRAFFIC
         Serial.write(b);
 #endif
     }
 
     // BT → Display
     while (SerialBT.available()) {
         uint8_t b = SerialBT.read();
         SerialDisplay.write(b);
 #if DEBUG_TRAFFIC
         Serial.write(b);
 #endif
     }
 }
 
 // ---------------- Proximity-based Logic ----------------
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
 
 // ---------------- BLE Maintenance ----------------
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
 
     // Send directly to BT module (transparent to Display)
     SerialBT.printf("%s\n", cmd);
     SerialBT.flush();
 
     delay(50);
     Serial.println("[CMD] Done, passthrough restored");
 }
 