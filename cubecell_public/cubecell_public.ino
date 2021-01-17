#include <OneWire.h>
#include <DallasTemperature.h>
#include "LoRaWan_APP.h"
#include "Arduino.h"

// Data wire is plugged into GPIO5 on the CubeCell
#define ONE_WIRE_BUS GPIO5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

/*
   set LoraWan_RGB to Active,the RGB active in loraWan
   RGB red means sending;
   RGB purple means joined done;
   RGB blue means RxWindow1;
   RGB yellow means RxWindow2;
   RGB green means received done;
*/

int sensorPin = ADC;
int threshold = 0;
static unsigned int counter = 0;
static byte interval = 0;
boolean lastState = false;
unsigned long lastSend;
unsigned long sendInterval = 600000; //every 10 mins
unsigned long lastBlubb;

/* OTAA para USED*/
uint8_t devEui[] = { xxx };
uint8_t appEui[] = { xxx };
uint8_t appKey[] = { xxx };

/* ABP para NOT USED*/
uint8_t nwkSKey[] = { 0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda, 0x85 };
uint8_t appSKey[] = { 0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef, 0x67 };
uint32_t devAddr =  ( uint32_t )0x007e6ae1;

/*LoraWan channelsmask, default channels 0-7*/
uint16_t userChannelsMask[6] = { 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass = LORAWAN_CLASS;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 500;

/*OTAA or ABP*/
bool overTheAirActivation = LORAWAN_NETMODE;

/*ADR enable*/
bool loraWanAdr = LORAWAN_ADR;

/* set LORAWAN_Net_Reserve ON, the node could save the network info to flash, when node reset not need to join again */
bool keepNet = LORAWAN_NET_RESERVE;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = LORAWAN_UPLINKMODE;

/* Application port */
uint8_t appPort = 2;
/*!
  Number of trials to transmit the frame, if the LoRaMAC layer did not
  receive an acknowledgment. The MAC performs a datarate adaptation,
  according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
  to the following table:

  Transmission nb | Data Rate
  ----------------|-----------
  1 (first)       | DR
  2               | DR
  3               | max(DR-1,0)
  4               | max(DR-1,0)
  5               | max(DR-2,0)
  6               | max(DR-2,0)
  7               | max(DR-3,0)
  8               | max(DR-3,0)

  Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
  the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials = 4; //was 4

/* Prepares the payload of the frame */
static void prepareTxFrame( uint8_t port )
{
  /*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
    appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
    if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
    if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
    for example, if use REGION_CN470,
    the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
  */
  appDataSize = 4;

  appData[0] = (counter >> 8);
  appData[1] = counter;
  appData[2] = interval;
  appData[3] = getByteTemp();
}


void setup() {
  boardInitMcu();
  lastSend = millis();
  lastBlubb = millis();
  Serial.begin(115200);

  //set threshold value (stick not visible)
  int total = 0;
  for (int i = 0; i < 5; i++) {
    total = total + analogRead(sensorPin);
  }

  int average = total / 5;

  int fivePercent = average / 100 * 5;
  
  
  threshold = (total / 5) - fivePercent;

  //init dallas temp sensor
  sensors.begin();


#if(AT_SUPPORT)
  enableAt();
#endif
  deviceState = DEVICE_STATE_INIT;
  LoRaWAN.ifskipjoin();
}

void loop()
{
  blubb();

  switch ( deviceState )
  {
    case DEVICE_STATE_INIT:
      {
#if(AT_SUPPORT)
        getDevParam();
#endif
        printDevParam();
        LoRaWAN.init(loraWanClass, loraWanRegion);
        deviceState = DEVICE_STATE_JOIN;
        break;
      }
    case DEVICE_STATE_JOIN:
      {
        LoRaWAN.join();
        break;
      }
    case DEVICE_STATE_SEND:
      {
      
        if (millis() > (lastSend + sendInterval)) {
          prepareTxFrame( appPort );
          LoRaWAN.send();
          lastSend = millis();
        }
        deviceState = DEVICE_STATE_CYCLE;
        break;
      }
    case DEVICE_STATE_CYCLE:
      {
        // Schedule next packet transmission
        txDutyCycleTime = appTxDutyCycle;// + randr( 0, APP_TX_DUTYCYCLE_RND );
        LoRaWAN.cycle(txDutyCycleTime);
        deviceState = DEVICE_STATE_SLEEP;
        break;
      }
    case DEVICE_STATE_SLEEP:
      {
        LoRaWAN.sleep();
        break;
      }
    default:
      {
        deviceState = DEVICE_STATE_INIT;
        break;
      }
  }
}

void blubb() {
  boolean state = getState();
  //if the cake stick is in down position
  if (lastState != state) {

      if (state == false && (millis() + 1500) > lastBlubb)
      {
        counter = counter + 1;
        signalBlubb(counter);
        
    
        //this is just roughly ok, state=false has a 5% threshold
        //also, as we are not transmitting on every blubb, there is a natural 
        //variation here. A chart trendline will be really useful. 
        //just seconds
        float rawInterval = ((millis() - lastBlubb) / 1000);
        if (rawInterval > 255) {
            interval = 255;
        } else {
          interval = rawInterval; //reduce to byte
        }
        
        lastBlubb = millis();
        
        lastState = state;
      } else if (state == true) {
        lastState = state;
      }

      
 
    
  }

  delay(50);
}

void signalBlubb(unsigned int counter) {
  Serial.print("blubb ");
  Serial.println(counter);

  turnOnRGB(getColor ( 255, 255, 255 ), 50 );
  turnOffRGB();
}

uint32_t getColor(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

boolean getState() {
  int total = 0;
  int sensorValue = 0;
  for (byte i = 0; i<10; i++)
  {
     total = total + analogRead(sensorPin);  
     delay(5);
  }
  
  sensorValue = total / 10;
  
  Serial.print(sensorValue);
  bool state = (sensorValue < threshold) ? true : false;
  Serial.print(" ");
  Serial.print(state ? "true" : "false");
  Serial.print(" - threshold  ");
  Serial.println(threshold);
  return state;
}

byte getByteTemp() {
  byte byteTemp = (getTemp() - 5) * 10;
  Serial.print("Byte temp (div by 10 then plus 5 ) is:  ");
  Serial.println(byteTemp);
  return byteTemp;
}

float getTemp() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  float tempC = sensors.getTempCByIndex(0);
  Serial.print ("TempC is ");
  Serial.print (tempC);
  return tempC;
  
  
  
}
