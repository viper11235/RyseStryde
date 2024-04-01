#include <bluefruit.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Wire.h>

#define BNO055_SAMPLERATE_DELAY_MS 1000 // Set the delay between gyroscope readings

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire); // Initialize BNO055 sensor

const char* BLEDeviceName = "RyseStride"; // Add Device Name.
BLEUart bleuart; // Initialize BLE UART service

const int motor = 9;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!bno.begin())
  {
    Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  // Initialize Bluefruit module
  Bluefruit.begin();
  Bluefruit.setTxPower(4); // Set BLE transmit power

  // Configure and start BLE UART service
  bleuart.begin();

  // Start advertising
  startAdv();

  pinMode(motor,OUTPUT);
}

// RUN THIS VOID LOOP CODE TO WORK
// void loop() {
//   sensors_event_t event;
//   bno.getEvent(&event, Adafruit_BNO055::VECTOR_GYROSCOPE);

//   // Send gyroscope data over BLE UART
//   if (Bluefruit.connected()) {
//     bleuart.print("Gyro X: ");
//     bleuart.print(event.orientation.x);
//     bleuart.print(", Y: ");
//     bleuart.print(event.orientation.y);
//     bleuart.print(", Z: ");
//     bleuart.println(event.orientation.z);
//   }

//   delay(BNO055_SAMPLERATE_DELAY_MS); // Adjust the delay according to your desired sampling rate
// }

void loop(void) {
  sensors_event_t event1, event2; // Variables to store sensor events
  double z1, z2; // Variables to store z-axis orientation values

  // Get the first sensor reading
  bno.getEvent(&event1);
  z1 = getZAxisValue(&event1);
  
  // Wait for 2 seconds
  delay(2000);

  // Get the second sensor reading
  bno.getEvent(&event2);
  z2 = getZAxisValue(&event2);
  
  // Calculate the difference between the two readings
  double deltaZ = z2 - z1;
  
  // Print the difference
  if (Bluefruit.connected()){
    bleuart.print("Change in Z-axis");
    bleuart.print(deltaZ);
}

  if (deltaZ > 30){
    digitalWrite(motor, HIGH);
  }
  else{
    digitalWrite(motor,LOW);
  }

}

double getZAxisValue(sensors_event_t* event) {
  double z = -1000000; // Initialize with a dummy value
  if (event->type == SENSOR_TYPE_ORIENTATION) {
    z = event->orientation.z;
  }
  return z;
}


void startAdv(void) {
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include BLE UART service
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
    Bluefruit.setName(BLEDeviceName);

  // Start Advertising
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}
