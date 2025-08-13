// ./BLE.h
//-----------------------------------------
#ifndef _BLE_H
#define _BLE_H
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

extern bool DebugIsON;
/*********************************************************
 *     BLE Server related constants.                     *
 *********************************************************/
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

// #define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
// #define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// Generated new UUID from https://www.uuidgenerator.net/
#define SERVICE_UUID        "d02f8b61-cb18-4272-bedc-be8a6117d794"
#define CHARACTERISTIC_UUID "f3f36c5f-a0a1-4b7a-a214-c74bfd4d0344"
#define BLE_DEVICE_NAME     "VelocityReader"


class BLE
{
  private:
    // Flag to flash LED.., when BLE Adv. is in progress
    
    unsigned long      previousMillis;
    long               interval;  // two seconds interval
    BLEServer*         pServer;
    BLECharacteristic* pCharacteristic;
  
    
  public:
  bool               AdvInProgress;
  void init(void);
  void notifyMsg(String Message);
  void sendMsg(String Message);
  void BLEAdvertisementInProgress(LED *pLED);
  void RestartAdvertisement(void);
  void Disconnect(void);
  bool isConnected(void);
};
#endif
// ./LED.h
//-----------------------------------------
#ifndef _LED_H
#define _LED_H


class LED
{
  public:
    // momentarily blink LED
    void init(void);
    void winkLED(int delayvalue = 500);
    void turnOnLED(bool withDelay = false);
    void turnOffLED(bool withDelay = false);
};
  
#endif// ./NFC.h
//-----------------------------------------
#ifndef _NFC_H
#define _NFC_H

extern bool DebugIsON;

class NFC
{
  public:
  // Static method to access the singleton instance of NFC
  static NFC* getInstance() {
        static NFC instance; // Guaranteed to be created only once (thread-safe in C++11 and above)
        return &instance;
  }
  void init();
  bool isInitialized(void);
  void showVersionData();
  void readNFC();
  String getLastReadCard(void);

  private:
  String lastReadNFCCard;
  bool Initialized;
  uint32_t versiondata;
  uint8_t uid[10];  // Buffer to store the returned UID
  uint8_t uidLength;                       // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Make NFC singleton
  private:
  NFC()
  {
    // DEBUG: Serial.println("NFC instance is created");
  }
  //Delete copy constructor and assignment operator
  NFC(const NFC &) = delete;
  NFC& operator=(const NFC&) = delete;

};
#endif
// ./BLE.ino
//-----------------------------------------
#include "BLE.h"
#include "LED.h"


  /*********************************************************
   *     BLE Server related variables.                     *
   *********************************************************/
  bool               deviceConnected = false;
  bool oldDeviceConnected = false;
  
// Class to receive connection status callback., 
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};


 void BLE::init(void)
 {
    // Initialize object variables to default values.,
    AdvInProgress   = false;
    previousMillis  = 0;
    interval        = 1000;  // one second interval
    pServer         = NULL;
    pCharacteristic = NULL;

    // Initialize BLE stack
    if(true == DebugIsON)
    {
      Serial.print("ApolloReader App: Initializing BLE Stack : ");
      Serial.flush();
    }
    // Create the BLE Device
    BLEDevice::init(BLE_DEVICE_NAME);

    if(true == DebugIsON)
    {
      Serial.println("Done");
      Serial.flush();
    }

    if(true == DebugIsON)
    {
      Serial.print("ApolloReader App: Creating BLE Server : ");
      Serial.flush();
    }
    
    pServer = BLEDevice::createServer();
    if(NULL == pServer)
    {
        Serial.println("Failed to create BLE Server");
        while(1)
        {
          Serial.print("S");
          Serial.flush();
          delay(1000);
        }
    }

    pServer->setCallbacks(new MyServerCallbacks());

    if(true == DebugIsON)
    {
      Serial.println("Done");
      Serial.flush();
    }


    if(true == DebugIsON)
    {
      Serial.print("ApolloReader App: Creating BLE Service and Characteristics : ");
      Serial.flush();
    }

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);
    if(NULL == pService)
    {
        Serial.println("Failed to create BLE Service");
        while(1)
        {
          Serial.print("s");
          Serial.flush();
          delay(1000);
        }
    }

    // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ   |
                        BLECharacteristic::PROPERTY_NOTIFY |
                        BLECharacteristic::PROPERTY_INDICATE
                      );
    
    if(NULL == pCharacteristic)
    {
        Serial.println("Failed to create BLE Characteristics");
        while(1)
        {
          Serial.print("C");
          Serial.flush();
          delay(1000);
        }
    }
    if(true == DebugIsON)
    {
      Serial.println("Done");
      Serial.flush();
    }
    // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
    // Create a BLE Descriptor
    pCharacteristic->addDescriptor(new BLE2902());

    if(true == DebugIsON)
    {
      Serial.printf("ApolloReader App: Starting service %s with characteristics : %s   : ",SERVICE_UUID, CHARACTERISTIC_UUID);
      Serial.flush();
    }

    // Start the service
    pService->start();
    if(true == DebugIsON)
    {
      Serial.println("Done");
      Serial.flush();
    }
    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    if(NULL == pCharacteristic)
    {
        Serial.println("Failed to create BLE Characteristics");
        while(1)
        {
          Serial.print("A");
          Serial.flush();
          delay(1000);
        }
    }
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter

    if(true == DebugIsON)
    {
      Serial.print("ApolloReader App: Starting advertisement....: ");
      Serial.flush();
    }
    BLEDevice::startAdvertising();
    AdvInProgress = true;
    if(true == DebugIsON)
    {
      Serial.println("Done");
      Serial.flush();
      Serial.println("ApolloReader App: Waiting a client connection to notify...");
    }
}

void BLE::notifyMsg(String Message)
{
  if(Message.length() > 0)
  {
    pCharacteristic->setValue((uint8_t *)Message.c_str(), Message.length());
    pCharacteristic->notify();
  }
}


void BLE::sendMsg(String Message)
{
  if(Message.length() > 0)
  {
    // Just write the message but don't notify., let the client read it
    // explicitly.,
    pCharacteristic->setValue((uint8_t *)Message.c_str(), Message.length());
  }
}


void BLE::BLEAdvertisementInProgress(LED *pLED)
{
    unsigned long currentMillis = millis();

    if(NULL == pLED)
    {
      return; // Silent failure., TBD., fallback to default LED?
    }

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      pLED->winkLED(300);
    }
}

void BLE::RestartAdvertisement(void)
{
  pServer->startAdvertising(); // restart advertising
}


void BLE::Disconnect(void)
{
  // Disconnect and start advertising again.,
  if (pServer->getConnectedCount() > 0) {
    // Done separately, hence not needed., pServer->startAdvertising();
    pServer->disconnect(pServer->getConnId()); // Disconnect the client
  }
}

bool BLE::isConnected(void)
{
  return pServer->getConnectedCount() > 0; // Returns flase if no device are connected.,
}
// ./BLENotify.ino
//-----------------------------------------
  #include <microBox.h>
  #include <Wire.h>
  #include "LED.h"
  #include "NFC.h"
  #include "BLE.h"


  // Driver handler for BLE, LED and NFC
  LED *pLED = NULL;
  BLE *pBLE = NULL;
  NFC *pNFC  = NULL;

  // Debug message logging., 
  bool DebugIsON = false;
  String  Message;


  ///////////////////////// Begin OF COMMAND HANDLERS 
  // 0) "help"
  void help(char **param, uint8_t parCnt)
  {
    Serial.println("Supported commands are., ");
    Serial.println("help/?        : This command");
    Serial.println("disable_debug : This command disables the debug messages");
    Serial.println("enaable_debug : This command enables the debug messages");
    Serial.println("reset         : This command resets this device");
    Serial.println("disconnect    : This command disconnect and restart the advertisement");
    Serial.println("notifymsg     : Send msg to connected client via notification");
    Serial.println("sendmsg       : Write message for the client to read");
    Serial.println("blink         : Blinks onboard LED for 1 second");
    Serial.println("readnfc       : Reads NFC card reader ID if available and write to console");
    Serial.println("lastcard      : Reads last read NFC card and write to console");
    Serial.println("nfcver        : Shows NFC FW version data");
    Serial.println("notifyblenfc  : Reads last read NFC card info via BLE");
    Serial.println("scanIIC       : Scans for all the devices connected on I2C");
    Serial.println("tasklist      : Shows running tasks");
  }

  // 1) "disable_debug"
  void disable_debug(char **param, uint8_t parCnt)
  {
    DebugIsON = false;
    Serial.println("");
  }

  // 2) "enable_debug"
  void enable_debug(char **param, uint8_t parCnt)
  {
    DebugIsON = true;
    Serial.println("");
  }

  // 3) reset
  void reset_device(char **param, uint8_t parCnt)
  {
    Serial.print("Resetting in : ");
    Serial.flush();
    Serial.print("3");
    Serial.flush();
    delay(1000);
    Serial.print("\b2");
    Serial.flush();
    delay(1000);
    Serial.print("\b1");
    Serial.flush();
    ESP.restart();
    Serial.println("");
  }

  // 4) disconnect
  void BLE_disconnect(char **param, uint8_t parCnt)
  {
    if(NULL != pBLE)
    {
      pBLE->Disconnect();
      pBLE->RestartAdvertisement();
      pBLE->AdvInProgress = true;
    }
    Serial.println("");
  }

  // 5) notifymsg : sends BLE message via notification
  void notifymsg(char **param, uint8_t parCnt)
  {
    if (parCnt >= 1)
    {
      String msg = "";
      for(int i = 0; i < parCnt; i++)
      {
        msg += String(param[i]);
        msg += " ";
      }

      if( (msg.length() > 0)  && 
          (NULL != pBLE)      && 
          (true == pBLE->isConnected())
        )
      {
        if(true == DebugIsON)
        {
          Serial.printf("Notifying Message: %s\n",  msg.c_str());
          Serial.flush();
        }
        pBLE->notifyMsg(msg);
      }
    }else
    {
      Serial.println(F("Usage: notifymsg <Message>"));
    }
    Serial.println("");
  }

  // 6) "sendmsg"
  void sendmsg(char **param, uint8_t parCnt)
  {
        if (parCnt >= 1)
    {
      String msg = "";
      for(int i = 0; i < parCnt; i++)
      {
        msg += String(param[i]);
        msg += " ";
      }

      if( (msg.length() > 0)  && 
          (NULL != pBLE)      && 
          (true == pBLE->isConnected())
        )
      {
        if(true == DebugIsON)
        {
          Serial.printf("Notifying Message: %s\n",  msg.c_str());
          Serial.flush();
        }
        pBLE->sendMsg(msg);
      }
    }else
    {
      Serial.println(F("Usage: notifymsg <Message>"));
    }
    Serial.println("");
  }

  // 7) "blink"
  void blink(char **param, uint8_t parCnt)
  {
    if(NULL != pLED)
    {
      pLED->winkLED(1000);
    }
    pLED->turnOffLED();
    Serial.println("");
  }

  // 8) "readnfc"
  void readNFC(char **param, uint8_t parCnt)
  {
    if(NULL != pNFC)
    {
      pNFC->readNFC();
    }
  }

  // 9) "nfcver"
  void nfcVersion(char **param, uint8_t parCnt)
  {
    if(NULL != pNFC)
    {
      pNFC->showVersionData();
    }
  }

  // 10) lastcard
  void getLastReadCard(char **param, uint8_t parCnt)
  {
    String previousCard = "";
    if(NULL != pNFC)
    {
      previousCard = pNFC->getLastReadCard();
      if(previousCard.length() > 0)
      {
        Serial.println(previousCard);
      }
    }
  }

  // 11) "notifyblenfc"
  void notifyBLENFC(char **param, uint8_t parCnt)
  {
    String card = "";
    if(NULL != pNFC && NULL != pBLE)
    {
      card = pNFC->getLastReadCard();
      if(card.length() > 0)
      {
        pBLE->notifyMsg(card);
      }
    }
  }

  // 12 scanIIC
  void scanI2CBus(char **param, uint8_t parCnt)
  {
    Wire.begin();
    Serial.println("Scanning...");

    byte count = 0;

    for (byte address = 1; address < 127; address++) {
      Wire.beginTransmission(address);
      byte error = Wire.endTransmission();

      if (error == 0) {
        Serial.print("I2C device found at address 0x");
        if (address < 16) Serial.print("0");
        Serial.print(address, HEX);
        Serial.println(" !");
        count++;
        delay(10);
      } 
      else if (error == 4) {
        Serial.print("Unknown error at address 0x");
        if (address < 16) Serial.print("0");
        Serial.println(address, HEX);
      }
    }

    if (count == 0)
      Serial.println("No I2C devices found\n");
    else
      Serial.println("Scan complete\n");

  }

  // 12) "tasklist"
  void showTasks(char **param, uint8_t parCnt)
  {
    // Buffer to store task list (large enough to hold all tasks)
    char *taskListBuffer = (char *)malloc(1024);

    if (taskListBuffer != NULL) {
      // Get task list
      vTaskList(taskListBuffer);

      // Print task list
      Serial.println("Task Name\tState\tPrio\tStack\tNum");
      Serial.println(taskListBuffer);

      free(taskListBuffer);
    }
  }

  ///////////////////////// END OF COMMAND HANDLERS 
  extern bool deviceConnected;
  extern bool oldDeviceConnected;

char historyBuf[100];
char hostname[] = "VR ";

PARAM_ENTRY Params[]=
{
  {"hostname", hostname, PARTYPE_STRING | PARTYPE_RW, sizeof(hostname), NULL, NULL, 0},
  {NULL, NULL}
};


  // Initialization function
  
  void setup() {

    Serial.begin(9600);

    if(true == DebugIsON)
    {
      while(!Serial)
      {
        delay(1000);
      }
    }


    // Register the message handlers in shell like interface.,
    microbox.begin(&Params[0], hostname, true, historyBuf, 100);
    
    microbox.AddCommand("help"          , help);
    microbox.AddCommand("?"             , help);
    microbox.AddCommand("disable_debug" , disable_debug);
    microbox.AddCommand("enable_debug"  , enable_debug);
    microbox.AddCommand("scanIIC"       , scanI2CBus);
    microbox.AddCommand("reset"         , reset_device);
    microbox.AddCommand("disconnect"    , BLE_disconnect);
    microbox.AddCommand("notifymsg"     , notifymsg);
    microbox.AddCommand("readnfc"       , readNFC);
    microbox.AddCommand("lastcard"      , getLastReadCard);
    microbox.AddCommand("sendmsg"       , sendmsg);
    microbox.AddCommand("blink"         , blink);
    microbox.AddCommand("nfcver"        , nfcVersion);
    microbox.AddCommand("notifyblenfc"  , notifyBLENFC);
    microbox.AddCommand("tasklist"      , showTasks);

    if(true == DebugIsON)
    {
      Serial.println("ApolloReader App: Jatin Gandhi");
    }

    pLED = new LED();
    if(NULL == pLED)
    {
      Serial.println("LED Initialization Failed..,");
      while(1)
      {
        delay(1000);
      }
    }else
    {
      pLED->init();
      // Blink LED 5 times to indicate initialization.,
      for(int i = 0; i < 5; i++)
      {
        pLED->winkLED(500);
      }      
    }


    // Setup GPIO
    pBLE = new BLE();
    if(NULL == pLED)
    {
      Serial.println("BLE Initialization Failed..,");
      while(1)
      {
        delay(1000);
      }
    }
    pBLE->init();

    pNFC = NFC::getInstance();

    if(NULL != pNFC)
    {
      if(true == DebugIsON)
      {
        Serial.println("We got instance., ");
      }
      pNFC->init();
    }

  }


void loop()
{
  microbox.cmdParser();
  
  if (true == pBLE->AdvInProgress)
  {
    pBLE->BLEAdvertisementInProgress(pLED);
  }

  if (deviceConnected)
  {
    pBLE->AdvInProgress = false;
  }
  if (!deviceConnected && oldDeviceConnected)
  {
        delay(500); // give the bluetooth stack the chance to get things ready
        if(true == DebugIsON)
        {
          Serial.println("ApolloReader App: Device Disconnected...");
        }

        pLED->turnOffLED();

        pBLE->RestartAdvertisement(); // restart advertising
        pBLE->AdvInProgress = true;
        if(true == DebugIsON)
        {
          Serial.println("start advertising");
        }
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        pLED->turnOffLED();
        if(true == DebugIsON)
        {
          Serial.println("ApolloReader App: Device Connecting..,");
        }
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
// ./LED.ino
//-----------------------------------------
#include "LED.h"


  // momentarily blink LED
  void LED::winkLED(int delayvalue)
  {
    if(delayvalue > 1000)
    {
      delayvalue = 1000;
    }
    digitalWrite(LED_BUILTIN, HIGH);
    delay(delayvalue);
    digitalWrite(LED_BUILTIN, LOW);
    delay(delayvalue);
  }

  void LED::init(void)
  {
    pinMode(LED_BUILTIN, OUTPUT);
  }

  void LED::turnOnLED(bool withDelay)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    if(true == withDelay)
    {
      delay(500);
    }
  }
  
  void LED::turnOffLED(bool withDelay)
  {
    digitalWrite(LED_BUILTIN, LOW);
    if(true == withDelay)
    {
      delay(500);
    }
  }

// ./NFC.ino
//-----------------------------------------
/*

* PN532 NFC RFID Module (v3)

* Quick Test Code v2

* Based on an example code from PN532 Arduino Library

* Tailored for HSUART & Arduino Uno

* T.K.Hareendran/2019

* www.edn.com

*/

  #include <Wire.h>
  #include <PN532_I2C.h>
  #include <PN532.h>
  #include <NfcAdapter.h>
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  #include "freertos/semphr.h"
 
 /*********************************************************
  *     NFC Reader related variables.                     *
  *********************************************************/
   
  PN532_I2C pn532i2c(Wire);
  PN532 nfc(pn532i2c);

  bool Initialized = false;
  extern bool DebugIsON;

  

  static void NFCReaderTask(void *param);

  void NFC::init(void) {

   
    Initialized = false;
    memset(uid,0, sizeof(uid)/sizeof(uid[0]));
    uidLength = 0;
    lastReadNFCCard = "0";

    // Set I2C Speed!!
    Wire.setClock(40000);  // 40 kHz

    if(true == DebugIsON)
    {
      Serial.print("Initializing NFC Reader : ");
      Serial.flush();
    }

    nfc.begin();

    versiondata = 0;
    versiondata = nfc.getFirmwareVersion();

    if (versiondata)
    {
      Initialized = true;
      if(true == DebugIsON)
      {
        Serial.println("Done");
      }
    }else
    {
      if(false == DebugIsON)
      {
        Serial.print("Initializing NFC Reader : ");
      }
      Serial.println("Failed");
      Initialized = false;
      return;
    }
    
    if(true == Initialized)
    {
      // Set the max number of retry attempts to read from a card
      // This prevents us from waiting forever for a card, which is
      // the default behaviour of the PN532.
      // Tweak this retry as needed. low retry means it don't wait at all!!
      nfc.setPassiveActivationRetries(0xFF); 

      // Configure board to read RFID tags
      nfc.SAMConfig();
      if(true == DebugIsON)
      {
        Serial.println("Waiting for an ID Card");
      }
    }

    // Create a task to read NFC card when interrupt occurs.,
    if(true == DebugIsON)
    {
      Serial.println("Starting NFC reader task., ");
    }
    #ifdef CREATE_THREAD_DEFAULT_CORE
    // Create task for Arduino led 
    xTaskCreate(NFCReaderTask, // Task function
                "NFCReader", // Task name
                8192, // Stack size 
                NULL, // None
                0, // Priority
                (tskTaskControlBlock** )NULL);
    #else
    // Tools -> Events Run on Core X then give Y core., 
    xTaskCreatePinnedToCore(NFCReaderTask, // Task function
                            "NFCReader", // Task name
                            8192, // Stack size 
                            NULL, // None
                            1,
                            (tskTaskControlBlock** )NULL,
                            0
    );
    #endif

    // Suppress I2C warnings., 
    esp_log_level_set("i2c", ESP_LOG_NONE);
  }

  void NFC::showVersionData(void)
  {
    if( versiondata > 0)
    {
      // Got valid data, print it out!
      Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
      Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
      Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
    }
  }

  void NFC::readNFC(void) {

    bool success;
    if(false == Initialized)
    {
      Serial.println("0");
      return;
    }
    memset(uid,0, sizeof(uid)/sizeof(uid[0]));
    uidLength = 0;
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
    if (success) {
      lastReadNFCCard = "";
      if(true == DebugIsON)
      {
        Serial.println("Found A Card!");
        Serial.print("UID Length: ");
        Serial.print(uidLength, DEC);
        Serial.println(" bytes");
        Serial.print("UID Value: ");
      }
      if(true == DebugIsON)
      {
        Serial.print("0x");
      }

      lastReadNFCCard += "0x";

      for (uint8_t i=0; i < uidLength; i++)
      {
        if(true == DebugIsON)
        {
          Serial.print(uid[i], HEX);
        }
        lastReadNFCCard += String(uid[i], HEX);
      }
      if(true == DebugIsON)
      {
        Serial.println(lastReadNFCCard);
        Serial.println("");
      }
      delay(100);
    }
  }


String NFC::getLastReadCard(void)
{
  String prevcard;
  prevcard = "";
  prevcard = lastReadNFCCard;
  lastReadNFCCard = "0"; // Destroy the read value once returned., 
  return prevcard;
}

static void NFCReaderTask(void *param)
{
    NFC *pstNFC = nullptr;

    pstNFC = NFC::getInstance();
    if(NULL == pstNFC)
    {
      Serial.println("Could not get the NFC instance");
      while(1)
      {
        Serial.println("#");
        vTaskDelay(10000 / portTICK_PERIOD_MS);  
      }
    }
    // Dummy inf. loop for debugging., 
    for(;;)
    {
      // DEBUG: Serial.printf("Free heap: %d\n", esp_get_free_heap_size());
      if( true == pstNFC->isInitialized())
      {
        // Read the NFC ID from device to lastReadNFCCard.
        if(true == DebugIsON)
        {
          Serial.println("Reading card..");
        }
          pstNFC->readNFC();
      }
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }

}

bool NFC::isInitialized(void)
{
  return (true == Initialized);
}
