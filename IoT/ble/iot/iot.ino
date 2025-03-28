#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "abcd1234-abcd-1234-abcd-123456789abc"

BLECharacteristic *characteristic;
bool deviceConnected = false;

/**
 * @brief Callback for BLE connection events
 */
class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE...");

  BLEDevice::init("ESP32-BLE");
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  BLEService *service = server->createService(SERVICE_UUID);
  characteristic = service->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );

  characteristic->setValue("Hello from ESP32");
  service->start();

  server->getAdvertising()->start();
  Serial.println("BLE service started");
}

void loop() {
  if (deviceConnected) {
    int sensorValue = analogRead(A0);
    String sensorData = String(sensorValue);
    characteristic->setValue(sensorData.c_str());
    Serial.println("Sent data: " + sensorData);
    delay(1000);
  }
}
