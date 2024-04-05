#include <bluefruit.h>

const char* BLEDeviceName = "RyseStride"; // Add Device Name.

// Service UUID
const uint8_t UUID16_SVC[] = {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x23, 0x16, 0x00, 0x00};

// Characteristic UUID
const uint8_t UUID16_CHR[] = {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x24, 0x16, 0x00, 0x00};

BLEService ryse_service(UUID16_SVC);
BLECharacteristic ryse_characteristic(UUID16_CHR, BLERead | BLENotify, sizeof(double));

BLEDis bledis;    // DIS (Device Information Service) helper class instance
BLEBas blebas;    // BAS (Battery Service) helper class instance

// const int dataToSend = 25;

void setup() {
  // Start bluetooth 
  Bluefruit.begin();
  ryse_service.begin();

  // Configure characteristic
  ryse_characteristic.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
  ryse_characteristic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  ryse_characteristic.setFixedLen(sizeof(double));
  ryse_characteristic.begin();

  // Start Bluetooth advertising
  Bluefruit.Advertising.addService(ryse_service);
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
  double dataToSend = 25;
  ryse_characteristic.notify(&dataToSend, sizeof(dataToSend));
  
  Serial.print("Sent: ");
  Serial.println(dataToSend);
  
  delay(1000); // 1 sec delay
}

// specify names 