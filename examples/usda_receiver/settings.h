#define AES_KEY_BYTES   16  //Number of bytes in the encryption key

//Select the operating mode
typedef enum
{
  MODE_DATAGRAM = 0,
  MODE_POINT_TO_POINT,
  MODE_VIRTUAL_CIRCUIT,
} OPERATING_MODE;

typedef enum
{
  STR_1W = 0,
  STR_FACET,
} ProductVariant;
ProductVariant productVariant = STR_1W;

typedef enum
{
  RADIO_REGION_US = 0,
  RADIO_REGION_EU433,
  RADIO_REGION_EU863,
  RADIO_REGION_AU915,
  RADIO_REGION_US902,
  RADIO_REGION_IN865,
  RADIO_REGION_DEV,
} RadioRegion;

//These are all the settings that can be set on Serial Terminal Radio. It's recorded to NVM.
typedef struct struct_settings {
  uint16_t sizeOfSettings = 0; //sizeOfSettings **must** be the first entry and must be int
  //uint16_t strIdentifier = LRS_IDENTIFIER; // strIdentifier **must** be the second entry

  uint32_t serialSpeed = 57600; //Default to 57600bps to match RTK Surveyor default firmware
  uint32_t airSpeed = 4800; //Default to ~523 bytes per second to support RTCM. Overrides spread, bandwidth, and coding
  uint8_t netID = 192; //Both radios must share a network ID
  uint8_t operatingMode = MODE_POINT_TO_POINT; //Receiving unit will check netID and ACK. If set to false, receiving unit doesn't check netID or ACK.
  bool encryptData = true; //AES encrypt each packet
  uint8_t encryptionKey[AES_KEY_BYTES] = { 0x37, 0x78, 0x21, 0x41, 0xA6, 0x65, 0x73, 0x4E, 0x44, 0x75, 0x67, 0x2A, 0xE6, 0x30, 0x83, 0x08 };
  bool dataScrambling = false; //Use IBM Data Whitening to reduce DC bias
  uint8_t radioBroadcastPower_dbm = 30; //Transmit power in dBm. Max is 30dBm (1W), min is 14dBm (25mW).
  float frequencyMin = 902.0; //MHz
  float frequencyMax = 928.0; //MHz
  uint8_t numberOfChannels = 50; //Divide the min/max freq band into this number of channels and hop between.
  bool frequencyHop = true; //Hop between frequencies to avoid dwelling on any one channel for too long
  uint16_t maxDwellTime = 400; //Max number of ms before hopping (if enabled). Useful for configuring radio to be within regulator limits (FCC = 400ms max)
  float radioBandwidth = 500.0; //kHz 125/250/500 generally. We need 500kHz for higher data.
  uint8_t radioSpreadFactor = 9; //6 to 12. Use higher factor for longer range.
  uint8_t radioCodingRate = 8; //5 to 8. Higher coding rates ensure less packets dropped.
  uint8_t radioSyncWord = 18; //18 = 0x12 is default for custom/private networks. Different sync words does *not* guarantee a remote radio will not get packet.
  uint16_t radioPreambleLength = 8; //Number of symbols. Different lengths does *not* guarantee a remote radio privacy. 8 to 11 works. 8 to 15 drops some. 8 to 20 is silent.
  uint16_t serialTimeoutBeforeSendingFrame_ms = 50; //Send partial buffer if time expires
  bool debug = false; //Print basic events: ie, radio state changes
  bool echo = false; //Print locally inputted serial
  uint16_t heartbeatTimeout = 5000; //ms before sending ping to see if link is active
  bool flowControl = false; //Enable the use of CTS/RTS flow control signals
  bool autoTuneFrequency = false; //Based on the last packets frequency error, adjust our next transaction frequency
  bool displayPacketQuality = false; //Print RSSI, SNR, and freqError for received packets
  uint8_t maxResends = 0; //Attempt resends up to this number, 0 = infinite retries
  bool sortParametersByName = false; //Sort the parameter list (ATI0) by parameter name
  bool printParameterName = false; //Print the parameter name in the ATSx? response
  bool printFrequency = false; //Print the updated frequency
  bool debugRadio = false; //Print radio info
  bool debugStates = false; //Print state changes
  bool debugTraining = false; //Print training info
  bool debugTrigger = false; //Print triggers
  bool usbSerialWait = false; //Wait for USB serial initialization
  bool printRfData = false; //Print RX and TX data
  bool printPktData = false; //Print data, before encryption and after decryption
  bool verifyRxNetID = false; //Verify RX netID value when not operating in point-to-point mode
  uint8_t triggerWidth = 25; //Trigger width in microSeconds or multipler for trigger width
  bool triggerWidthIsMultiplier = true; //Use the trigger width as a multiplier
  uint32_t triggerEnable = 0xffffffff; //Determine which triggers are enabled: 31 - 0
  uint32_t triggerEnable2 = 0xffffffff; //Determine which triggers are enabled: 63 - 32
  bool debugReceive = false; //Print receive processing
  bool debugTransmit = false; //Print transmit processing
  bool printTxErrors = false; //Print any transmit errors
  uint8_t radioProtocolVersion = 2; //Select the radio protocol
  bool printTimestamp = false; //Print a timestamp: days hours:minutes:seconds.milliseconds
  bool debugDatagrams = false; //Print the datagrams
  uint16_t overheadTime = 10; ////ms added to ack and datagram times before ACK timeout occurs
  bool enableCRC16 = false; //Append CRC-16 to packet, check CRC-16 upon receive
  bool displayRealMillis = false; //true = Display the millis value without offset, false = use offset
  bool trainingServer = false; //Default to being a client
  uint8_t clientPingRetryInterval = 3; //Number of seconds before retransmiting the client PING
  bool copyDebug = true; //Copy the debug parameters to the training client
  bool copySerial = true; //Copy the serial parameters to the training client
  bool copyTriggers = true; //Copy the trigger parameters to the training client
  uint8_t trainingKey[AES_KEY_BYTES] = { 0x53, 0x70, 0x61, 0x72, 0x6b, 0x46, 0x75, 0x6E, 0x54, 0x72, 0x61, 0x69, 0x6e, 0x69, 0x6e, 0x67 };
  bool printLinkUpDown = false; //Print the link up and link down messages
  bool invertCts = false; //Invert the input of CTS
  bool invertRts = false; //Invert the output of RTS
  bool alternateLedUsage = false; //Enable alternate LED usage

  //Add new parameters immediately before this line
  //-- Add commands to set the parameters
  //-- Add parameters to routine updateRadioParameters
} Settings;
Settings settings;

//Monitor which devices on the device are on or offline.
struct struct_online {
  bool radio = false;
  bool eeprom = false;
} online;
