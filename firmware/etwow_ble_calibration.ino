#include <Arduino.h>
#include <BLEDevice.h>

const String TARGET_MAC = "aa:bb:cc:dd:ee:ff"; // BLE badge MAC (minuscule)

void handleBLEDevice(BLEAdvertisedDevice d) {
    String mac = d.getAddress().toString().toLowerCase();
    int rssi = d.getRSSI();
   // if (mac == TARGET_MAC) {
        Serial.println("Badge [" + mac + "] Name: " + " RSSI=" + String(rssi));
   // }
}

// --- Nouvelle fonction pour relancer le scan si nécessaire ---
void ensureBLEScanActive() {
    BLEScan* scan = BLEDevice::getScan();
    if (!scan->isScanning()) {
        Serial.println("BLE scan inactif, redémarrage...");
        scan->start(0, nullptr, false); // scan infini, non bloquant
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(5);

    Serial.printf("BLE scanner actif (filtre MAC: %s)\n", TARGET_MAC.c_str());

    BLEDevice::init("");
    BLEScan* scan = BLEDevice::getScan();
    scan->setAdvertisedDeviceCallbacks(handleBLEDevice, false);
    scan->setActiveScan(true);
    scan->setInterval(100);
    scan->setWindow(99);
    scan->start(0, nullptr, false);
}

void loop() {
    ensureBLEScanActive(); // s’assure que le scan continue
    delay(100);            // laisse le temps aux callbacks BLE
}
