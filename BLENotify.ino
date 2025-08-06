/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updated by chegewara

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
   And has a characteristic of: beb5483e-36e1-4688-b7f5-ea07361b26a8

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   A connect hander associated with the server starts a background task that performs notification
   every couple of seconds.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define NUM_MESSAGES 4

char *BICycleState[] = {
  "Control Cycle Complete\0",
  "Test Cycle Complete\0",
  "Control Cycle Aborted\0",
  "Test Cycle Aborted\0",
};
uint8_t BiCycleMsgIndex = 0;


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};



void setup() {
  Serial.begin(115200);

  BiCycleMsgIndex = 0;
  while(!Serial)
  {
    delay(100);
  }
  // Setup GPIO

  Serial.println("ApolloReader App: Jatin Gandhi");
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  Serial.print("ApolloReader App: Initializing BLE Stack : ");
  Serial.flush();
  // Create the BLE Device
  BLEDevice::init("ApolloReader");
  Serial.println("Done");
  Serial.flush();
  // Create the BLE Server

  Serial.print("ApolloReader App: Creating BLE Server : ");
  Serial.flush();

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  Serial.println("Done");
  Serial.flush();

  Serial.print("ApolloReader App: Creating BLE Service and Characteristics : ");
  Serial.flush();

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  Serial.println("Done");
  Serial.flush();
  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  Serial.printf("ApolloReader App: Starting service %s with characteristics : %s   : ",SERVICE_UUID, CHARACTERISTIC_UUID);
  Serial.flush();

  // Start the service
  pService->start();

  Serial.println("Done");
  Serial.flush();
  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  digitalWrite(LED_RED,   LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, HIGH);
  Serial.print("ApolloReader App: Starting advertisement....: ");
  Serial.flush();
  BLEDevice::startAdvertising();
  Serial.println("Done");
  Serial.flush();
  Serial.println("ApolloReader App: Waiting a client connection to notify...");
}
 
void loop() {
    // notify changed value
    if (deviceConnected) {
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_BLUE, LOW);

        pCharacteristic->setValue((uint8_t *)BICycleState[BiCycleMsgIndex], strlen(BICycleState[BiCycleMsgIndex]));
        pCharacteristic->notify();
        BiCycleMsgIndex = random(0, 4); // 4 Exclusive as per Arduino documentation., 
        if(BiCycleMsgIndex >= NUM_MESSAGES )
        {
          // Reset to first message., 
          BiCycleMsgIndex = 0;
        }
        delay(random(4000, 8000)); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        Serial.println("ApolloReader App: Device Disconnected...");

        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_BLUE, HIGH);

        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        Serial.println("ApolloReader App: Device Connecting..,");
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
