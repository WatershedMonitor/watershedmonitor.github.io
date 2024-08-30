// Include libraries
#include <SPI.h>
#include <RadioLib.h>
#include "settings.h"

int powOutPin = 16;
// Create radio
SPIClass mySPI(HSPI);
RFM95 radio = new Module(15, 27, 5, 26, mySPI);

void setup()
{
  // Start serial
  Serial.begin(115200);
  Serial.println("LoRa Test");
  
    // Enable VOUT
  pinMode(powOutPin, OUTPUT);
  digitalWrite(powOutPin, LOW);

  // Start radio
  RFM95WBegin();
  delay(2000);
}

void loop()
{
  // Attempt to read data from radio
  String str;
  int state = radio.receive(str);

  // Check whether data was actually read
  if (state == RADIOLIB_ERR_NONE)
  {
    // Data read successfully!
    // Print SNR and RSSI
    Serial.println("Data received! -------------------------------------------");
    Serial.printf("SNR:  %.2f dB\n", radio.getSNR());
    Serial.printf("RSSI: %.0f dBm\n", radio.getRSSI());

    // Print received data
    Serial.println();
    Serial.println(str);
    Serial.println();
  }
  else if (state == RADIOLIB_ERR_RX_TIMEOUT)
  {
    // Timeout occurred while waiting for data, so nothing to receive yet
  }
  else if (state == RADIOLIB_ERR_CRC_MISMATCH)
  {
    // Data was received, but is malformed
    Serial.println("Receive CRC error!");
  }
  else
  {
    // Some other error occurred
    Serial.print("Receive failed, code: ");
    Serial.println(state);
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