#include <bluefruit.h>

const char* BLEDeviceName = "RyseStride"; // Add Device Name.

// Service UUID
const uint8_t UUID16_SVC[] = {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x23, 0x16, 0x00, 0x00};

// Characteristic UUID
const uint8_t UUID16_CHR[] = {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x24, 0x16, 0x00, 0x00};

BLEService service(UUID16_SVC);
BLECharacteristic characteristic(UUID16_CHR, BLERead, sizeof(uint8_t));

BLEDis bledis;    // DIS (Device Information Service) helper class instance
BLEBas blebas;    // BAS (Battery Service) helper class instance

int dataToSend = 25;

void setup() {
  // Start bluetooth 
  Bluefruit.begin();
  characteristic.begin();

  // Configure characteristic
  characteristic.setProperties(CHR_PROPS_READ);
  characteristic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  characteristic.begin();

  // Start Bluetooth advertising
  Bluefruit.Advertising.addService(service);
  Bluefruit.Advertising.start(0); // 0 means it will advertise forever
  // Secondary Scan Response packet (optional)
  Bluefruit.ScanResponse.addName();
   Bluefruit.setName(BLEDeviceName);
   
  Serial.begin(115200);
  while (!Serial)
    delay(10);  // will pause board until serial console opens

  Serial.println("Bluetooth is paired");
}

void loop() {
  characteristic.write(&dataToSend, sizeof(dataToSend));
  
  Serial.print("Sent: ");
  Serial.println(dataToSend);
  
  delay(1000); // 1 sec delay
}
