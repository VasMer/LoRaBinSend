#include <Arduino.h>
#include <heltec_unofficial.h>
#include <mbedtls/sha1.h>

#include <HashUtil.h>
#include <Packet.h>

// LoRa frequency 433.125 869.525
#define FREQUENCY 869.525

// LoRa bandwidth. Keep the decimal point to designate float.
// Allowed values are 7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0, 250.0 and 500.0 kHz.
// Lower bandwidth longer range, speed lower.
#define BANDWIDTH 250.0

// Number from 5 to 12. Higher means slower but higher "processor gain",
// meaning (in nutshell) longer range and more robust against interference.
#define SPREADING_FACTOR 10

// Coding Rate 5 to 8, default 7
//  A coding rate of 4/5 means there are 4 information bits and 5 bits in total per symbol
//  5: 4/5
//  6: 4/6
//  7: 4/7
//  8: 4/8
#define CODING_RATE 5

// Transmit power in dBm. 0 dBm = 1 mW, enough for tabletop-testing. This value can be
// set anywhere between -9 dBm (0.125 mW) to 22 dBm (158 mW). Note that the maximum ERP
// (which is what your antenna maximally radiates) on the EU ISM band is 25 mW, and that
// transmissting without an antenna can damage your hardware.
#define TRANSMIT_POWER -9

volatile bool _packageAvailable = false;

void radioSetup();
void dataSend();
void lora_receive();
void receivedDataDisplay();
void dataPrint(uint8_t *buffer);

void setup()
{
  // Heltec board setup
  heltec_setup();
  // Put screen on on v3.2
  // heltec_ve(true);
  // Setup radio
  radioSetup();
}

void loop()
{
  // Button functionality
  heltec_loop();
  // Button click
  if (button.isSingleClick())
  {
    dataSend();
  }
  if (_packageAvailable)
  {
    receivedDataDisplay();
  }
}

void radioSetup()
{
  RADIOLIB_OR_HALT(radio.begin());
  // Set the callback function for received packets
  radio.setDio1Action(lora_receive);
  // Set radio parameters
  both.printf("Frequency: %.3f MHz\n", FREQUENCY);
  RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));
  both.printf("Bandwidth: %.1f kHz\n", BANDWIDTH);
  RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));
  both.printf("Spreading Factor: %i\n", SPREADING_FACTOR);
  RADIOLIB_OR_HALT(radio.setSpreadingFactor(SPREADING_FACTOR));
  both.printf("Coding rate: 4/%i\n", CODING_RATE);
  RADIOLIB_OR_HALT(radio.setCodingRate(CODING_RATE));
  both.printf("TX power: %i dBm\n", TRANSMIT_POWER);
  RADIOLIB_OR_HALT(radio.setOutputPower(TRANSMIT_POWER));
  // Start receiving
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
}

// LoRa receive collback
void lora_receive()
{
  _packageAvailable = true;
}

void dataSend()
{
  int state;
  uint64_t tx_time;
  uint8_t data[13];
  Packet packet(PacketType::DATA);

  packet.to = 0;

  packet.toBytes(data);

  dataPrint(data);

  both.printf("Sending %i b...", sizeof(data));
  tx_time = millis();
  heltec_led(50);
  // Stop listening
  radio.clearDio1Action();
  // Transmit
  state = radio.transmit(data, sizeof(data));
  heltec_led(0);
  tx_time = millis() - tx_time;

  if (state == RADIOLIB_ERR_NONE)
  {
    both.printf(" ok(%i ms)\n", (int)tx_time);
  }
  else
  {
    both.printf("Error while sending data, code: %i\n", state);
  }
  // Start listening
  radio.setDio1Action(lora_receive);
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
}

void receivedDataDisplay()
{
  uint8_t receivedData[256]; // Буфер для хранения принятого пакета
  int packetSize = radio.getPacketLength();

  heltec_led(10);
  _packageAvailable = false;
  radio.readData(receivedData, packetSize);
  if (_radiolib_status == RADIOLIB_ERR_NONE)
  {
    both.printf("RX %d bytes [%d]\n", packetSize, receivedData[19]);
    dataPrint(receivedData);
    both.printf("RSSI: %ddBm, ", (int)round(radio.getRSSI()));
    both.printf("SNR: %.1fdB\n", radio.getSNR());

    Packet packet(PacketType::ACK);

    packet.fromBytes(receivedData);
    both.printf("TO: %.d\n", packet.to);
    both.printf("FROM: %.d\n", packet.from);
    both.printf("Type: %.d\n", packet.type);
  }
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  heltec_led(0);
}

void dataPrint(uint8_t *buffer)
{

  for (int i = 0; i < 13; i++)
  {
    both.printf("%02X", buffer[i]);
  }
  both.printf("\n");
}