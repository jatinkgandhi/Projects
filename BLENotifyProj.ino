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
  
#endif// ./microBox.h
//-----------------------------------------
/*
  microBox.h - Library for Linux-Shell like interface for Arduino.
  Created by Sebastian Duell, 06.02.2015.
  More info under http://sebastian-duell.de
  Released under GPLv3.
*/

#ifndef _BASHCMD_H_
#define _BASHCMD_H_

#define __PROG_TYPES_COMPAT__
#include <Arduino.h>

#define MAX_CMD_NUM 20

#define MAX_CMD_BUF_SIZE 40
#define MAX_PATH_LEN 10

#define PARTYPE_INT    0x01
#define PARTYPE_DOUBLE 0x02
#define PARTYPE_STRING 0x04
#define PARTYPE_RW     0x10
#define PARTYPE_RO     0x00

#define ESC_STATE_NONE 0
#define ESC_STATE_START 1
#define ESC_STATE_CODE 2

#define TELNET_IAC 255
#define TELNET_WILL 251
#define TELNET_WONT 252
#define TELNET_DO 253
#define TELNET_DONT 254

#define TELNET_OPTION_ECHO 1
#define TELNET_OPTION_SGA 3

#define TELNET_STATE_NORMAL 0
#define TELNET_STATE_IAC 1
#define TELNET_STATE_WILL 2
#define TELNET_STATE_WONT 3
#define TELNET_STATE_DO 4
#define TELNET_STATE_DONT 5
#define TELNET_STATE_CLOSE 6

typedef struct
{
    const char *cmdName;
    void (*cmdFunc)(char **param, uint8_t parCnt);
}CMD_ENTRY;

typedef struct
{
    const char *paramName;
    void *pParam;
    uint8_t parType;
    uint8_t len;
    void (*setFunc)(uint8_t id);
    void (*getFunc)(uint8_t id);
    uint8_t id;
}PARAM_ENTRY;

class microBox
{
public:
    microBox();
    ~microBox();
    void begin(PARAM_ENTRY *pParams, const char* hostName, bool localEcho=true, char *histBuf=NULL, int historySize=0);
    void cmdParser();
    bool isTimeout(unsigned long *lastTime, unsigned long intervall);
    bool AddCommand(const char *cmdName, void (*cmdFunc)(char **param, uint8_t parCnt));

private:
    static void ListDirCB(char **pParam, uint8_t parCnt);
    static void ListLongCB(char **pParam, uint8_t parCnt);
    static void ChangeDirCB(char **pParam, uint8_t parCnt);
    static void EchoCB(char **pParam, uint8_t parCnt);
    static void CatCB(char** pParam, uint8_t parCnt);
    static void watchCB(char** pParam, uint8_t parCnt);
    static void watchcsvCB(char** pParam, uint8_t parCnt);
    static void LoadParCB(char **pParam, uint8_t parCnt);
    static void SaveParCB(char **pParam, uint8_t parCnt);

    void ListDir(char **pParam, uint8_t parCnt, bool listLong=false);
    void ChangeDir(char **pParam, uint8_t parCnt);
    void Echo(char **pParam, uint8_t parCnt);
    void Cat(char** pParam, uint8_t parCnt);
    void watch(char** pParam, uint8_t parCnt);
    void watchcsv(char** pParam, uint8_t parCnt);

private:
    void ShowPrompt();
    uint8_t ParseCmdParams(char *pParam);
    void ErrorDir(const __FlashStringHelper *cmd);
    char *GetDir(char *pParam, bool useFile);
    char *GetFile(char *pParam);
    void PrintParam(uint8_t idx);
    int8_t GetParamIdx(char* pParam, bool partStr = false, int8_t startIdx = 0);
    int8_t GetCmdIdx(char* pCmd, int8_t startIdx = 0);
    uint8_t Cat_int(char* pParam);
    void ListDirHlp(bool dir, bool rw = true, int len=4096);
    uint8_t ParCmp(uint8_t idx1, uint8_t idx2, bool cmd=false);
    void HandleTab();
    void HistoryUp();
    void HistoryDown();
    void HistoryPrintHlpr();
    void AddToHistory(char *buf);
    void ExecCommand();
    void handleTelnet(uint8_t ch);
    void sendTelnetOpt(uint8_t option, uint8_t value);
    double parseFloat(char *pBuf);
    bool HandleEscSeq(unsigned char ch);
    void ReadWriteParamEE(bool write);

private:
    char currentDir[MAX_PATH_LEN];

    char cmdBuf[MAX_CMD_BUF_SIZE];
    char dirBuf[15];
    char *ParmPtr[10];
    uint8_t bufPos;
    bool watchMode;
    bool csvMode;
    uint8_t escSeq;
    unsigned long watchTimeout;
    const char* machName;
    int historyBufSize;
    char *historyBuf;
    int historyWrPos;
    int historyCursorPos;
    bool locEcho;
    uint8_t stateTelnet;

    static CMD_ENTRY Cmds[MAX_CMD_NUM];
    PARAM_ENTRY *Params;
    static const char dirList[][5] PROGMEM;
};

extern microBox microbox;

#endif
// ./NFC.h
//-----------------------------------------
#ifndef _NFC_H
#define _NFC_H

extern bool DebugIsON;

#define DEFAULT_SWSERIAL_TXPIN  10
#define DEFAULT_SWSERIAL_RXPIN  9
class NFC
{
  public:
  void init();
  void showVersionData();
  void readNFC();

  private:
  bool Initialized;
  uint32_t versiondata;
  uint8_t uid[7];  // Buffer to store the returned UID
  uint8_t uidLength;                       // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

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
  #include "LED.h"
  #include "NFC.h"
  #include "BLE.h"


  // Driver handler for BLE, LED and NFC
  LED *pLED = NULL;
  BLE *pBLE = NULL;
  NFC *pNFC  = NULL;

  // Debug message logging., 
  bool DebugIsON = true;
  String  Message;


  ///////////////////////// Begin OF COMMAND HANDLERS 
  // 0) "help"
  void help(char **param, uint8_t parCnt)
  {
    Serial.println("Supported commands are., ");
    Serial.println("help          : This command");
    Serial.println("disable_debug : This command disables the debug messages");
    Serial.println("enaable_debug : This command enables the debug messages");
    Serial.println("reset         : This command resets this device");
    Serial.println("disconnect    : This command disconnect and restart the advertisement");
    Serial.println("notifymsg     : Send msg to connected client via notification");
    Serial.println("sendmsg       : Write message for the client to read");
    Serial.println("blink         : Blinks onboard LED for 1 second");
    Serial.println("readnfc       : Reads NFC card reader ID if available and write to console");
    Serial.println("nfcver        : Shows NFC FW version data");
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
    if (parCnt > 1)
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
    if (1 == parCnt)
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
          Serial.printf("Writing Message: %s\n",  msg.c_str());
          Serial.flush();
        }
        pBLE->sendMsg(msg);
      }
    }else
    {
      Serial.println(F("Usage: sendmsg <Message>"));
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
    microbox.AddCommand("disable_debug" , disable_debug);
    microbox.AddCommand("enable_debug"  , enable_debug);
    microbox.AddCommand("reset"         , reset_device);
    microbox.AddCommand("disconnect"    ,BLE_disconnect);
    microbox.AddCommand("notifymsg"     , notifymsg);
    microbox.AddCommand("readnfc"       , readNFC);
    microbox.AddCommand("sendmsg"       , sendmsg);
    microbox.AddCommand("blink"         , blink);
    microbox.AddCommand("nfcver"        , nfcVersion);
    

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

    pNFC = new NFC();

    if(NULL != pNFC)
    {
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

#if 0

  // Main thread/task/work loop
  void loop() {

      if (true == pBLE->AdvInProgress)
      {
        pBLE->BLEAdvertisementInProgress(pLED);
      }

      // Normal state to enable or disable message.,
      if(!deviceConnected)
      {
        if(Serial.available())
        {
          Message = "";
          Message = Serial.readStringUntil('\n'); // Read until newline

          if (Message.length() > 1)
          {
            if(true == DebugIsON)
            {
              Serial.print("Received: ");
              Serial.println(Message);
            }
            if(ENABLE_DEBUG_STR == Message)
            {
              DebugIsON = true;
              Serial.println("Enabled Debug Mode\n");
            }else if (DISBALE_DEBUG_STR == Message)
            {
              DebugIsON = false;
              Serial.println("Disabled Debug Mode\n");
            }
          }
        }
      }
      // notify changed value
      if (deviceConnected)
      {
          pBLE->AdvInProgress = false;
          if(Serial.available())
          {
            Message = "";
            Message = Serial.readStringUntil('\n'); // Read until newline

            if (Message.length() > 1)
            {
              if(true == DebugIsON)
              {
                Serial.print("Received: ");
                Serial.println(Message);
              }
              if(ENABLE_DEBUG_STR == Message)
              {
                DebugIsON = true;
                Serial.println("Enabled Debug Mode\n");
              }else if (DISBALE_DEBUG_STR == Message)
              {
                DebugIsON = false;
                Serial.println("Disabled Debug Mode\n");
              }else
              {
                // It is a message to be sent.,
                if(true == DebugIsON)
                {
                  Serial.printf("Notifying Message: %s\n",  Message.c_str());
                  Serial.flush();
                }
                for(int i = 0; i < 2; i++)
                {
                  pLED->winkLED(200);
                }
                pBLE->notifyMsg(Message);
              }
            }
            delay(1000);
          }
      }

      // disconnecting
      if (!deviceConnected && oldDeviceConnected) {
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


#endif
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

  #include <PN532_HSU.h>
  #include <PN532.h>

 /*********************************************************
  *     NFC Reader related variables.                     *
  *********************************************************/
  
  PN532_HSU pn532hsu( Serial2 );
  PN532 nfc( pn532hsu );
  bool Initialized = false;
  extern bool DebugIsON;

  void NFC::init(void) {

    Serial.println("Hello Maker!"); // DEBUG only
    
    Initialized = false;
    memset(uid,0, sizeof(uid)/sizeof(uid[0]));
    uidLength = 0;

    Serial2.begin(9600, SERIAL_8N1, 10, 9); // Serial2 at 9600 baud( 10, 9 ); // RX, TX

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

    // Configure board to read RFID tags
    nfc.SAMConfig();
    if(true == DebugIsON)
    {
      Serial.println("Waiting for an ID Card");
    }

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

      if(true == DebugIsON)
      {
        Serial.println("Found A Card!");
        Serial.print("UID Length: ");
        Serial.print(uidLength, DEC);
        Serial.println(" bytes");
        Serial.print("UID Value: ");
      }

      for (uint8_t i=0; i < uidLength; i++)
      {
        Serial.print("0x");
        Serial.print(uid[i], HEX);
      }
      Serial.println("");

      // 1 second halt
      delay(1000);
    }
  }
// ./microBox.cpp
//-----------------------------------------
/*
  microBox.cpp - Library for Linux-Shell like interface for Arduino.
  Created by Sebastian Duell, 06.02.2015.
  More info under http://sebastian-duell.de
  Released under GPLv3.
*/

#include <microBox.h>
#ifndef ESP32
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#endif

microBox microbox;
const prog_char fileDate[] PROGMEM = __DATE__;

CMD_ENTRY microBox::Cmds[] =
{
    {"cat", microBox::CatCB},
    {"cd", microBox::ChangeDirCB},
    {"echo", microBox::EchoCB},
#ifndef ESP32
    {"loadpar", microBox::LoadParCB},
#endif
    {"ll", microBox::ListLongCB},
    {"ls", microBox::ListDirCB},
#ifndef ESP32
    {"savepar", microBox::SaveParCB},
#endif
    {"watch", microBox::watchCB},
    {"watchcsv", microBox::watchcsvCB},
    {NULL, NULL}
};

const char microBox::dirList[][5] PROGMEM =
{
    "bin", "dev", "etc", "proc", "sbin", "var", "lib", "sys", "tmp", "usr", ""
};

microBox::microBox()
{
    bufPos = 0;
    watchMode = false;
    csvMode = false;
    locEcho = false;
    watchTimeout = 0;
    escSeq = 0;
    historyWrPos = 0;
    historyBufSize = 0;
    historyCursorPos = -1;
    stateTelnet = TELNET_STATE_NORMAL;
}

microBox::~microBox()
{
}

void microBox::begin(PARAM_ENTRY *pParams, const char* hostName, bool localEcho, char *histBuf, int historySize)
{
    historyBuf = histBuf;
    if(historyBuf != NULL && historySize != 0)
    {
        historyBufSize = historySize;
        historyBuf[0] = 0;
        historyBuf[1] = 0;
    }

    locEcho = localEcho;
    Params = pParams;
    machName = hostName;
    ParmPtr[0] = NULL;
    strcpy(currentDir, "/");
    ShowPrompt();
}

bool microBox::AddCommand(const char *cmdName, void (*cmdFunc)(char **param, uint8_t parCnt))
{
    uint8_t idx = 0;

    while((Cmds[idx].cmdFunc != NULL) && (idx < (MAX_CMD_NUM-1)))
    {
        idx++;
    }
    if(idx < (MAX_CMD_NUM-1))
    {
        Cmds[idx].cmdName = cmdName;
        Cmds[idx].cmdFunc = cmdFunc;
        idx++;
        Cmds[idx].cmdFunc = NULL;
        Cmds[idx].cmdName = NULL;
        return true;
    }
    return false;
}

bool microBox::isTimeout(unsigned long *lastTime, unsigned long intervall)
{
    unsigned long m;

    m = millis();
    if(((m - *lastTime) >= intervall) || (*lastTime > m))
    {
        *lastTime = m;
        return true;
    }
    return false;
}

void microBox::ShowPrompt()
{
    Serial.print(F("root@"));
    Serial.print(machName);
    Serial.print(F(":"));
    Serial.print(currentDir);
    Serial.print(F(">"));
}

uint8_t microBox::ParseCmdParams(char *pParam)
{
    uint8_t idx = 0;

    ParmPtr[idx] = pParam;
    if(pParam != NULL)
    {
        idx++;
        while((pParam = strchr(pParam, ' ')) != NULL)
        {
            pParam[0] = 0;
            pParam++;
            ParmPtr[idx++] = pParam;
        }
    }
    return idx;
}

void microBox::ExecCommand()
{
    bool found = false;
    Serial.println();
    if(bufPos > 0)
    {
        uint8_t i=0;
        uint8_t dstlen;
        uint8_t srclen;
        char *pParam;

        cmdBuf[bufPos] = 0;
        pParam = strchr(cmdBuf, ' ');
        if(pParam != NULL)
        {
            pParam++;
            srclen = pParam - cmdBuf - 1;
        }
        else
            srclen = bufPos;

        AddToHistory(cmdBuf);
        historyCursorPos = -1;

        while(Cmds[i].cmdName != NULL && found == false)
        {
            dstlen = strlen(Cmds[i].cmdName);
            if(dstlen == srclen)
            {
                if(strncmp(cmdBuf, Cmds[i].cmdName, dstlen) == 0)
                {
                    (*Cmds[i].cmdFunc)(ParmPtr, ParseCmdParams(pParam));
                    found = true;
                    bufPos = 0;
                    ShowPrompt();
                }
            }
            i++;
        }
        if(!found)
        {
            bufPos = 0;
            ErrorDir(F("/bin/sh"));
            ShowPrompt();
        }
    }
    else
        ShowPrompt();
}

void microBox::cmdParser()
{
    if(watchMode)
    {
        if(Serial.available())
        {
            watchMode = false;
            csvMode = false;
        }
        else
        {
            if(isTimeout(&watchTimeout, 500))
                Cat_int(cmdBuf);

            return;
        }
    }
    while(Serial.available())
    {
        uint8_t ch;
        ch = Serial.read();
        if(ch == TELNET_IAC || stateTelnet != TELNET_STATE_NORMAL)
        {
            handleTelnet(ch);
            continue;
        }

        if(HandleEscSeq(ch))
            continue;

        if(ch == 0x7F || ch == 0x08)
        {
            if(bufPos > 0)
            {
                bufPos--;
                cmdBuf[bufPos] = 0;
                Serial.write(ch);
                Serial.print(F(" \x1B[1D"));
            }
            else
            {
                Serial.print(F("\a"));
            }
        }
        else if(ch == '\t')
        {
            HandleTab();
        }
        else if(ch != '\r' && bufPos < (MAX_CMD_BUF_SIZE-1))
        {
            if(ch != '\n')
            {
                if(locEcho)
                    Serial.write(ch);
                cmdBuf[bufPos++] = ch;
                cmdBuf[bufPos] = 0;
            }
        }
        else
        {
            ExecCommand();
        }
    }
}

bool microBox::HandleEscSeq(unsigned char ch)
{
    bool ret = false;

    if(ch == 27)
    {
        escSeq = ESC_STATE_START;
        ret = true;
    }
    else if(escSeq == ESC_STATE_START)
    {
        if(ch == 0x5B)
        {
            escSeq = ESC_STATE_CODE;
            ret = true;
        }
        else
            escSeq = ESC_STATE_NONE;
    }
    else if(escSeq == ESC_STATE_CODE)
    {
        if(ch == 0x41) // Cursor Up
        {
            HistoryUp();
        }
        else if(ch == 0x42) // Cursor Down
        {
            HistoryDown();
        }
        else if(ch == 0x43) // Cursor Right
        {
        }
        else if(ch == 0x44) // Cursor Left
        {
        }
        escSeq = ESC_STATE_NONE;
        ret = true;
    }
    return ret;
}

uint8_t microBox::ParCmp(uint8_t idx1, uint8_t idx2, bool cmd)
{
    uint8_t i=0;

    const char *pName1;
    const char *pName2;

    if(cmd)
    {
        pName1 = Cmds[idx1].cmdName;
        pName2 = Cmds[idx2].cmdName;
    }
    else
    {
        pName1 = Params[idx1].paramName;
        pName2 = Params[idx2].paramName;
    }

    while(pName1[i] != 0 && pName2[i] != 0)
    {
        if(pName1[i] != pName2[i])
            return i;
        i++;
    }
    return i;
}

int8_t microBox::GetCmdIdx(char* pCmd, int8_t startIdx)
{
    while(Cmds[startIdx].cmdName != NULL)
    {
        if(strncmp(Cmds[startIdx].cmdName, pCmd, strlen(pCmd)) == 0)
        {
            return startIdx;
        }
        startIdx++;
    }
    return -1;
}

void microBox::HandleTab()
{
    int8_t idx, idx2;
    char *pParam = NULL;
    uint8_t i, len = 0;
    uint8_t parlen, matchlen, inlen;

    for(i=0;i<bufPos;i++)
    {
        if(cmdBuf[i] == ' ')
            pParam = cmdBuf+i;
    }
    if(pParam != NULL)
    {
        pParam++;
        if(*pParam != 0)
        {
            idx = GetParamIdx(pParam, true, 0);
            if(idx >= 0)
            {
                parlen = strlen(Params[idx].paramName);
                matchlen = parlen;
                idx2=idx;
                while((idx2=GetParamIdx(pParam, true, idx2+1))!= -1)
                {
                    matchlen = ParCmp(idx, idx2);
                    if(matchlen < parlen)
                        parlen = matchlen;
                }
                pParam = GetFile(pParam);
                inlen = strlen(pParam);
                if(matchlen > inlen)
                {
                    len = matchlen - inlen;
                    if((bufPos + len) < MAX_CMD_BUF_SIZE)
                    {
                        strncat(cmdBuf, Params[idx].paramName + inlen, len);
                        bufPos += len;
                    }
                    else
                        len = 0;
                }
            }
        }
    }
    else if(bufPos)
    {
        pParam = cmdBuf;

        idx = GetCmdIdx(pParam);
        if(idx >= 0)
        {
            parlen = strlen(Cmds[idx].cmdName);
            matchlen = parlen;
            idx2=idx;
            while((idx2=GetCmdIdx(pParam, idx2+1))!= -1)
            {
                matchlen = ParCmp(idx, idx2, true);
                if(matchlen < parlen)
                    parlen = matchlen;
            }
            inlen = strlen(pParam);
            if(matchlen > inlen)
            {
                len = matchlen - inlen;
                if((bufPos + len) < MAX_CMD_BUF_SIZE)
                {
                    strncat(cmdBuf, Cmds[idx].cmdName + inlen, len);
                    bufPos += len;
                }
                else
                    len = 0;
            }
        }
    }
    if(len > 0)
    {
        Serial.print(pParam + inlen);
    }
}

void microBox::HistoryUp()
{
    if(historyBufSize == 0 || historyWrPos == 0)
        return;

    if(historyCursorPos == -1)
        historyCursorPos = historyWrPos-2;

    while(historyBuf[historyCursorPos] != 0 && historyCursorPos > 0)
    {
        historyCursorPos--;
    }
    if(historyCursorPos > 0)
        historyCursorPos++;

    strcpy(cmdBuf, historyBuf+historyCursorPos);
    HistoryPrintHlpr();
    if(historyCursorPos > 1)
        historyCursorPos -= 2;
}

void microBox::HistoryDown()
{
    int pos;
    if(historyCursorPos != -1 && historyCursorPos != historyWrPos-2)
    {
        pos = historyCursorPos+2;
        pos += strlen(historyBuf+pos) + 1;

        strcpy(cmdBuf, historyBuf+pos);
        HistoryPrintHlpr();
        historyCursorPos = pos - 2;
    }
}

void microBox::HistoryPrintHlpr()
{
    uint8_t i;
    uint8_t len;

    len = strlen(cmdBuf);
    for(i=0;i<bufPos;i++)
        Serial.print('\b');
    Serial.print(cmdBuf);
    if(len<bufPos)
    {
        Serial.print(F("\x1B[K"));
    }
    bufPos = len;
}

void microBox::AddToHistory(char *buf)
{
    uint8_t len;
    int blockStart = 0;

    len = strlen(buf);
    if(historyBufSize > 0)
    {
        if(historyWrPos+len+1 >= historyBufSize)
        {
            while(historyWrPos+len-blockStart >= historyBufSize)
            {
                blockStart += strlen(historyBuf + blockStart) + 1;
            }
            memmove(historyBuf, historyBuf+blockStart, historyWrPos-blockStart);
            historyWrPos -= blockStart;
        }
        strcpy(historyBuf+historyWrPos, buf);
        historyWrPos += len+1;
        historyBuf[historyWrPos] = 0;
    }
}

// 2 telnet methods derived from https://github.com/nekromant/esp8266-frankenstein/blob/master/src/telnet.c
void microBox::sendTelnetOpt(uint8_t option, uint8_t value)
{
    uint8_t tmp[4];
    tmp[0] = TELNET_IAC;
    tmp[1] = option;
    tmp[2] = value;
    tmp[3] = 0;
    Serial.write(tmp, 4);
}

void microBox::handleTelnet(uint8_t ch)
{
    switch (stateTelnet)
    {
    case TELNET_STATE_IAC:
        if(ch == TELNET_IAC)
        {
            stateTelnet = TELNET_STATE_NORMAL;
        }
        else
        {
            switch(ch)
            {
            case TELNET_WILL:
                stateTelnet = TELNET_STATE_WILL;
                break;
            case TELNET_WONT:
                stateTelnet = TELNET_STATE_WONT;
                break;
            case TELNET_DO:
                stateTelnet = TELNET_STATE_DO;
                break;
            case TELNET_DONT:
                stateTelnet = TELNET_STATE_DONT;
                break;
            default:
                stateTelnet = TELNET_STATE_NORMAL;
                break;
            }
        }
        break;
    case TELNET_STATE_WILL:
        sendTelnetOpt(TELNET_DONT, ch);
        stateTelnet = TELNET_STATE_NORMAL;
        break;
    case TELNET_STATE_WONT:
        sendTelnetOpt(TELNET_DONT, ch);
        stateTelnet = TELNET_STATE_NORMAL;
        break;
    case TELNET_STATE_DO:
        if(ch == TELNET_OPTION_ECHO)
        {
            sendTelnetOpt(TELNET_WILL, ch);
            sendTelnetOpt(TELNET_DO, ch);
            locEcho = true;
        }
        else if(ch == TELNET_OPTION_SGA)
            sendTelnetOpt(TELNET_WILL, ch);
        else
            sendTelnetOpt(TELNET_WONT, ch);
        stateTelnet = TELNET_STATE_NORMAL;
        break;
    case TELNET_STATE_DONT:
        sendTelnetOpt(TELNET_WONT, ch);
        stateTelnet = TELNET_STATE_NORMAL;
        break;
    case TELNET_STATE_NORMAL:
        if(ch == TELNET_IAC)
        {
            stateTelnet = TELNET_STATE_IAC;
        }
        break;
    }
}


void microBox::ErrorDir(const __FlashStringHelper *cmd)
{
    Serial.print(cmd);
    Serial.println(F(": File or directory not found\n"));
}

char *microBox::GetDir(char *pParam, bool useFile)
{
    uint8_t i=0;
    uint8_t len;
    char *tmp;

    dirBuf[0] = 0;
    if(pParam != NULL)
    {
        if(currentDir[1] != 0)
        {
            if(pParam[0] != '/')
            {
                if(!(pParam[0] == '.' && pParam[1] == '.'))
                {
                    return NULL;
                }
                else
                {
                    pParam += 2;
                    if(pParam[0] == 0)
                    {
                        dirBuf[0] = '/';
                        dirBuf[1] = 0;
                    }
                    else if(pParam[0] != '/')
                        return NULL;
                }
            }
        }
        if(pParam[0] == '/')
        {
            if(pParam[1] == 0)
            {
                dirBuf[0] = '/';
                dirBuf[1] = 0;
            }
            pParam++;
        }

        if((tmp=strchr(pParam, '/')) != 0)
        {
            len = tmp-pParam;
        }
        else
            len = strlen(pParam);
        if(len > 0)
        {
            while(pgm_read_byte_near(&dirList[i][0]) != 0)
            {
                if(strncmp_P(pParam, dirList[i], len) == 0)
                {
                    if(strlen_P(dirList[i]) == len)
                    {
                        dirBuf[0] = '/';
                        dirBuf[1] = 0;
                        strcat_P(dirBuf, dirList[i]);
                        return dirBuf;
                    }
                }
                i++;
            }
        }
    }
    if(dirBuf[0] != 0)
        return dirBuf;
    return NULL;
}

char *microBox::GetFile(char *pParam)
{
    char *file;
    char *t;

    file = pParam;
    while((t=strchr(file, '/')) != NULL)
    {
        file = t+1;
    }
    return file;
}

void microBox::ListDirHlp(bool dir, bool rw, int len)
{
    cmdBuf[1] = 'r';
    cmdBuf[3] = 0;
    if(dir)
        cmdBuf[0] = 'd';
    else
        cmdBuf[0] = '-';

    if(rw)
        cmdBuf[2] = 'w';
    else
        cmdBuf[2] = '-';

    Serial.print(cmdBuf);
    cmdBuf[0] = 0;

    Serial.print(F("xr-xr-x\t2 root\troot\t"));
    Serial.print(len);
    Serial.print(F(" "));
    Serial.print((const __FlashStringHelper*)fileDate);
    Serial.print(F(" "));
}

void microBox::ListDir(char **pParam, uint8_t parCnt, bool listLong)
{
    uint8_t i=0;
    char *dir;

    if(parCnt != 0)
    {
        dir = GetDir(pParam[0], false);
        if(dir == NULL)
        {
            if(listLong)
                ErrorDir(F("ll"));
            else
                ErrorDir(F("ls"));
            return;
        }
    }
    else
    {
        dir = currentDir;
    }

    if(dir[1] == 0)
    {
        while(pgm_read_byte_near(&dirList[i][0]) != 0)
        {
            if(listLong)
            {
                ListDirHlp(true);
            }
            Serial.print((__FlashStringHelper*)dirList[i]);
            if(listLong)
                Serial.println();
            else
                Serial.print(F("\t"));
            i++;
        }
        Serial.println();
    }
    else if(strcmp_P(dir, PSTR("/bin")) == 0)
    {
        while(Cmds[i].cmdName != NULL)
        {
            if(listLong)
            {
                ListDirHlp(false);
            }
            Serial.println(Cmds[i].cmdName);
            i++;
        }
    }
    else if(strcmp_P(dir, PSTR("/dev")) == 0)
    {
        while(Params[i].paramName != NULL)
        {
            if(listLong)
            {
                uint8_t size;
                if(Params[i].parType&PARTYPE_INT)
                    size=sizeof(int);
                else if(Params[i].parType&PARTYPE_DOUBLE)
                    size = sizeof(double);
                else
                    size = Params[i].len;

                ListDirHlp(false, Params[i].parType&PARTYPE_RW, size);
            }
            Serial.println(Params[i].paramName);
            i++;
        }
    }
}

void microBox::ChangeDir(char **pParam, uint8_t parCnt)
{
    char *dir;

    if(pParam[0] != NULL)
    {
        dir = GetDir(pParam[0], false);
        if(dir != NULL)
        {
            strcpy(currentDir, dir);
            return;
        }
    }
    ErrorDir(F("cd"));
}

void microBox::PrintParam(uint8_t idx)
{
    if(Params[idx].getFunc != NULL)
        (*Params[idx].getFunc)(Params[idx].id);

    if(Params[idx].parType&PARTYPE_INT)
        Serial.print(*((int*)Params[idx].pParam));
    else if(Params[idx].parType&PARTYPE_DOUBLE)
        Serial.print(*((double*)Params[idx].pParam), 8);
    else
        Serial.print(((char*)Params[idx].pParam));

    if(csvMode)
        Serial.print(F(";"));
    else
        Serial.println();
}

int8_t microBox::GetParamIdx(char* pParam, bool partStr, int8_t startIdx)
{
    int8_t i=startIdx;
    char *dir;
    char *file;

    if(pParam != NULL)
    {
        dir = GetDir(pParam, true);
        if(dir == NULL)
            dir = currentDir;
        if(dir != NULL)
        {
            if(strcmp_P(dir, PSTR("/dev")) == 0)
            {
                file = GetFile(pParam);
                if(file != NULL)
                {
                    while(Params[i].paramName != NULL)
                    {
                        if(partStr)
                        {
                            if(strncmp(Params[i].paramName, file, strlen(file))== 0)
                            {
                                return i;
                            }
                        }
                        else
                        {
                            if(strcmp(Params[i].paramName, file)== 0)
                            {
                                return i;
                            }
                        }
                        i++;
                    }
                }
            }
        }
    }
    return -1;
}

// Taken from Stream.cpp
double microBox::parseFloat(char *pBuf)
{
    boolean isNegative = false;
    boolean isFraction = false;
    long value = 0;
    unsigned char c;
    double fraction = 1.0;
    uint8_t idx = 0;

    c = pBuf[idx++];
    // ignore non numeric leading characters
    if(c > 127)
        return 0; // zero returned if timeout

    do{
        if(c == '-')
            isNegative = true;
        else if (c == '.')
            isFraction = true;
        else if(c >= '0' && c <= '9')  {      // is c a digit?
            value = value * 10 + c - '0';
            if(isFraction)
                fraction *= 0.1;
        }
        c = pBuf[idx++];
    }
    while( (c >= '0' && c <= '9')  || c == '.');

    if(isNegative)
        value = -value;
    if(isFraction)
        return value * fraction;
    else
        return value;
}

// echo 82.00 > /dev/param
void microBox::Echo(char **pParam, uint8_t parCnt)
{
    uint8_t idx;

    if((parCnt == 3) && (strcmp_P(pParam[1], PSTR(">")) == 0))
    {
        idx = GetParamIdx(pParam[2]);
        if(idx != -1)
        {
            if(Params[idx].parType & PARTYPE_RW)
            {
                if(Params[idx].parType & PARTYPE_INT)
                {
                    int val;

                    val = atoi(pParam[0]);
                    *((int*)Params[idx].pParam) = val;
                }
                else if(Params[idx].parType & PARTYPE_DOUBLE)
                {
                    double val;

                    val = parseFloat(pParam[0]);
                    *((double*)Params[idx].pParam) = val;
                }
                else
                {
                    if(strlen(pParam[0]) < Params[idx].len)
                        strcpy((char*)Params[idx].pParam, pParam[0]);
                }
                if(Params[idx].setFunc != NULL)
                    (*Params[idx].setFunc)(Params[idx].id);
            }
            else
                Serial.println(F("echo: File readonly"));
        }
        else
        {
            ErrorDir(F("echo"));
        }
    }
    else
    {
        for(idx=0;idx<parCnt;idx++)
        {
            Serial.print(pParam[idx]);
            Serial.print(F(" "));
        }
        Serial.println();
    }
}

void microBox::Cat(char** pParam, uint8_t parCnt)
{
    Cat_int(pParam[0]);
}

uint8_t microBox::Cat_int(char* pParam)
{
    int8_t idx;

    idx = GetParamIdx(pParam);
    if(idx != -1)
    {
        PrintParam(idx);
        return 1;
    }
    else
        ErrorDir(F("cat"));
    
    return 0;
}

void microBox::watch(char** pParam, uint8_t parCnt)
{
    if(parCnt == 2)
    {
        if(strncmp_P(pParam[0], PSTR("cat"), 3) == 0)
        {
            if(Cat_int(pParam[1]))
            {
                strcpy(cmdBuf, pParam[1]);
                watchMode = true;
            }
        }
    }
}

void microBox::watchcsv(char** pParam, uint8_t parCnt)
{
    watch(pParam, parCnt);
    if(watchMode)
        csvMode = true;
}

#ifndef ESP32
void microBox::ReadWriteParamEE(bool write)
{
    uint8_t i=0;
    uint8_t psize;
    int pos=0;

    while(Params[i].paramName != NULL)
    {
        if(Params[i].parType&PARTYPE_INT)
            psize = sizeof(uint16_t);
        else if(Params[i].parType&PARTYPE_DOUBLE)
            psize = sizeof(double);
        else
            psize = Params[i].len;

        if(write)
            eeprom_write_block(Params[i].pParam, (void*)pos, psize);
        else
            eeprom_read_block(Params[i].pParam, (void*)pos, psize);
        pos += psize;
        i++;
    }
}
#endif

void microBox::ListDirCB(char **pParam, uint8_t parCnt)
{
    microbox.ListDir(pParam, parCnt);
}

void microBox::ListLongCB(char **pParam, uint8_t parCnt)
{
    microbox.ListDir(pParam, parCnt, true);
}

void microBox::ChangeDirCB(char **pParam, uint8_t parCnt)
{
    microbox.ChangeDir(pParam, parCnt);
}

void microBox::EchoCB(char **pParam, uint8_t parCnt)
{
    microbox.Echo(pParam, parCnt);
}

void microBox::CatCB(char** pParam, uint8_t parCnt)
{
    microbox.Cat(pParam, parCnt);
}

void microBox::watchCB(char** pParam, uint8_t parCnt)
{
    microbox.watch(pParam, parCnt);
}

void microBox::watchcsvCB(char** pParam, uint8_t parCnt)
{
    microbox.watchcsv(pParam, parCnt);
}
#ifndef ESP32
void microBox::LoadParCB(char **pParam, uint8_t parCnt)
{
    microbox.ReadWriteParamEE(false);
}

void microBox::SaveParCB(char **pParam, uint8_t parCnt)
{
    microbox.ReadWriteParamEE(true);
}

#endif
