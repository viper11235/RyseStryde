#include <bluefruit.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Wire.h>

#define BNO055_SAMPLERATE_DELAY_MS 1000 // Set the delay between gyroscope readings

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire); // Initialize BNO055 sensor

const char* BLEDeviceName = "RyseStride"; // Add Device Name.

// Service UUID
const uint8_t UUID16_SVC[] = {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x23, 0x16, 0x00, 0x00};

// Characteristic UUID
const uint8_t UUID16_CHR[] = {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x24, 0x16, 0x00, 0x00};

BLEService ryse_service(UUID16_SVC);
BLECharacteristic ryse_characteristic(UUID16_CHR, BLERead | BLENotify, sizeof(double)); // Change size to accommodate double type

BLEDis bledis;    // DIS (Device Information Service) helper class instance
BLEBas blebas;    // BAS (Battery Service) helper class instance

void setup() {
  // Start Bluetooth 
  Bluefruit.begin();
  ryse_service.begin();

  // Characteristic setup
  ryse_characteristic.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
  ryse_characteristic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  ryse_characteristic.setFixedLen(sizeof(double)); // Set length to accommodate double type
  ryse_characteristic.begin();

  // Start Bluetooth advertising
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(ryse_service);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds

  // Set BLE device name
  Bluefruit.setName(BLEDeviceName);

  Serial.begin(115200);
  while (!Serial)
    delay(10);  // will pause board until serial console opens

  Serial.println("Bluetooth is paired");

  // Initialize BNO055 sensor
  if (!bno.begin())
  {
    Serial.println("Failed to initialize BNO055 sensor!");
    while (1);
  }
  delay(1000);
}

void loop() {
  sensors_event_t event1, event2; // Variables to store sensor events
  double z1, z2; // Variables to store z-axis orientation values

  // Get the first sensor reading
  bno.getEvent(&event1);
  z1 = getZAxisValue(&event1);

  // Wait for 2 seconds
  delay(500);

  // Get the second sensor reading
  bno.getEvent(&event2);
  z2 = getZAxisValue(&event2);

  // Calculate the difference between the two readings
  double deltaZ = z2 - z1;

  Serial.println(deltaZ);

  // Send the difference over BLE
  ryse_characteristic.notify(&deltaZ, sizeof(deltaZ));
  
}

double getZAxisValue(sensors_event_t* event) {
  double z = -1000000; // Initialize with a dummy value
  if (event->type == SENSOR_TYPE_ORIENTATION) {
    z = event->orientation.z;
  }
  return z;
}
