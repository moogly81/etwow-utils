#include <Arduino.h>
#include <BLEDevice.h>

const String TARGET_MAC = "aa:bb:cc:dd:ee:ff"; // BLE badge MAC

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) override {
    String mac = advertisedDevice.getAddress().toString().c_str();
    int rssi = advertisedDevice.getRSSI();
    //if (mac == TARGET_MAC) {
        Serial.printf("Found badge: %s RSSI=%d\n", mac.c_str(), rssi);
    //}
  }
};

void ensureBLEScanActive() {
    BLEScan* scan = BLEDevice::getScan();
    if (!scan->isScanning()) {
        Serial.println("BLE scan inactive, restarting...");
        scan->start(0, nullptr, false);
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(5);

    Serial.printf("BLE scanner active (MAC filter: %s)\n", TARGET_MAC.c_str());

    BLEDevice::init("");
    BLEScan* scan = BLEDevice::getScan();
    scan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), false);
    scan->setActiveScan(true);
    scan->setInterval(100);
    scan->setWindow(99);
    scan->start(0, nullptr, false);
}

void loop() {
    ensureBLEScanActive();
    delay(100);
}
