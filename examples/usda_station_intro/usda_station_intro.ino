// Include libraries
#include <Wire.h>
#include <SPI.h>
#include "Zanshin_BME680.h"
#include "SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h"
#include "ICM_20948.h"
#include <RadioLib.h>
#include "settings.h"

// Create references to peripherals
BME680_Class BME680;
NAU7802 myScale;
int powOutPin = 16;
ICM_20948_I2C myICM;
SPIClass mySPI(HSPI);
RFM95 radio = new Module(15, 27, 5, 26, mySPI);

void setup()
{
  // Start serial
  Serial.begin(115200);
  Serial.println("USDA Test Begin");

  // Enable VOUT
  pinMode(powOutPin, OUTPUT);
  digitalWrite(powOutPin, LOW);

  // Start I2C bus
  Wire.begin();

  // Call helper functions below to start all peripherals
  BME680Begin();
  NAU7802Begin();
  ICM20948Begin();
  RFM95WBegin();
}

void loop()
{
  // Create buffer to hold data and text
  char buf[250];
  // Pointer that moves through the buffer as content is added
  char* bufPtr = buf;

  // Add BME680 data to buffer
  static int32_t  temp, humidity, pressure, gas;
  BME680.getSensorData(temp, humidity, pressure, gas);
  bufPtr += sprintf(bufPtr, "STN: %s\n", "QStation001");
  bufPtr += sprintf(bufPtr, "Tem: %d.%02d\n", (int8_t)(temp / 100), (uint8_t)(temp % 100));
  bufPtr += sprintf(bufPtr, "Hum: %d.%03d\n", (int8_t)(humidity / 1000), (uint16_t)(humidity % 1000));
  bufPtr += sprintf(bufPtr, "Pre: %d.%02d\n", (int16_t)(pressure / 100), (uint8_t)(pressure % 100));
  bufPtr += sprintf(bufPtr, "Gas: %d.%02d\n", (int16_t)(gas / 100), (uint8_t)(gas % 100));

  // Add NAU7802 data to buffer
  bufPtr += sprintf(bufPtr, "Wei: %d\n", myScale.getReading());

  // Add ICM20948 data to buffer
  myICM.getAGMT(); // Update values
  bufPtr += sprintf(bufPtr, "Acc: %6d%6d%6d\n", (int)myICM.accX(), (int)myICM.accY(), (int)myICM.accZ());
  bufPtr += sprintf(bufPtr, "Gyr: %6d%6d%6d\n", (int)myICM.gyrX(), (int)myICM.gyrY(), (int)myICM.gyrZ());
  bufPtr += sprintf(bufPtr, "Mag: %6d%6d%6d\n", (int)myICM.magX(), (int)myICM.magY(), (int)myICM.magZ());

  // Send buffer
  int state = radio.startTransmit(buf);
  if (state == RADIOLIB_ERR_NONE)
  {
    // Sent successfully, print out buffer for debugging
    Serial.println("Data sent! -------------------------------------------");
    
    Serial.println();
    Serial.print(buf);
    Serial.println();
  }
  else if (state == RADIOLIB_ERR_TX_TIMEOUT)
  {
    Serial.println("Transmit timeout!");
  }
  else
  {
    Serial.print("Transmit failed, code ");
    Serial.println(state);
  }

  delay(1000);
}

void BME680Begin()
{
  if(!BME680.begin(I2C_STANDARD_MODE))
  {
    Serial.println("BME680 begin failed!");
  }

  BME680.setOversampling(TemperatureSensor, Oversample16);
  BME680.setOversampling(HumiditySensor, Oversample16);
  BME680.setOversampling(PressureSensor, Oversample16);
  BME680.setIIRFilter(IIR4);
  BME680.setGas(320, 150);
}

void NAU7802Begin()
{
  if(!myScale.begin())
  {
    Serial.println("NAU7802 begin failed!");
  }
}

void ICM20948Begin()
{
  if(myICM.begin(Wire, 1) != ICM_20948_Stat_Ok)
  {
    Serial.println("ICM20948 begin failed!");
  }
}

void RFM95WBegin()
{
  mySPI.begin();
  int state = radio.begin(915.0);
  if(state != RADIOLIB_ERR_NONE)
  {
    Serial.print(F("RFM95W begin failed, code "));
    Serial.println(state);
    return;
  }
  Serial.println(F("RFM95W init success!"));

  if (radio.setOutputPower(20) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) //Lowest setting
  {
    Serial.println(F("Output power invalid!"));
    while (1);
  }

  if (radio.setBandwidth(settings.radioBandwidth) == RADIOLIB_ERR_INVALID_BANDWIDTH)
  {
    Serial.println(F("Bandwidth invalid!"));
    while (1);
  }

  if (radio.setSpreadingFactor(settings.radioSpreadFactor) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR)
  {
    Serial.println(F("Spread factor invalid!"));
    while (1);
  }

  if (radio.setCodingRate(settings.radioCodingRate) == RADIOLIB_ERR_INVALID_CODING_RATE)
  {
    Serial.println(F("Coding rate invalid!"));
    while (1);
  }

  if (radio.setSyncWord(settings.radioSyncWord) != RADIOLIB_ERR_NONE)
  {
    Serial.println(F("Sync word failed!"));
    while (1);
  }

  if (radio.setPreambleLength(settings.radioPreambleLength) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH)
  {
    Serial.println(F("Preamble length invalid!"));
    while (1);
  }

  if (radio.setCRC(true) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION)
  {
    Serial.println(F("Enable CRC failed!"));
    while (1);
  }
}