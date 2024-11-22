#include <Wire.h>
#include <Adafruit_APDS9960.h>
#include <Adafruit_ADXL345_U.h>
#include <MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// --- Bluetooth MIDI ---
BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;
bool deviceConnected = false;

// Buffer pour messages MIDI
uint8_t midiPacket[4] = {0x80, 0x80, 0x00, 0x00};

// --- Déclarations des capteurs ---
Adafruit_APDS9960 apds;               // Capteur de gestes
Adafruit_ADXL345_Unified adxl(12345); // Accéléromètre ADXL345
MPU6050 mpu;                          // Accéléromètre et gyroscope
Adafruit_BMP085_Unified bmp(10085);   // Capteur de pression BMP180

// --- Variables pour gestion MIDI ---
int musicVolume = 50;                 // Volume MIDI initial
int musicPitch = 60;                  // Note MIDI initiale (C4)
const int motionThreshold = 5;        // Seuil de détection des mouvements

// --- Fonction d'initialisation ---
void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialisation Bluetooth MIDI
  setupBluetoothMIDI();

  // Initialisation des capteurs
  if (!apds.begin()) {
    Serial.println("Erreur : Capteur de gestes APDS9960 !");
    while (1);
  }
  apds.enableGesture(true);

  if (!adxl.begin()) {
    Serial.println("Erreur : Accéléromètre ADXL345 !");
    while (1);
  }

  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("Erreur : Capteur MPU6050 !");
    while (1);
  }

  if (!bmp.begin()) {
    Serial.println("Erreur : Capteur BMP180 !");
    while (1);
  }

  Serial.println("Tous les capteurs et Bluetooth MIDI sont prêts !");
}

void loop() {
  if (deviceConnected) {
    handleGestures();
    handleMotion();
    handlePressure();
    delay(50);
  }
}

// --- Gestion des gestes pour MIDI ---
void handleGestures() {
  int gesture = apds.readGesture();
  switch (gesture) {
    case APDS9960_UP:
      musicVolume = constrain(musicVolume + 10, 0, 127);
      sendBluetoothMIDI(0xB0, 7, musicVolume); // CC 7 pour volume
      Serial.println("Volume augmenté : " + String(musicVolume));
      break;
    case APDS9960_DOWN:
      musicVolume = constrain(musicVolume - 10, 0, 127);
      sendBluetoothMIDI(0xB0, 7, musicVolume); // CC 7 pour volume
      Serial.println("Volume diminué : " + String(musicVolume));
      break;
    case APDS9960_LEFT:
      musicPitch = constrain(musicPitch - 1, 21, 108); // Notes MIDI (A0 - C8)
      sendBluetoothMIDI(0x90, musicPitch, 100);        // Note ON
      Serial.println("Pitch diminué : " + String(musicPitch));
      break;
    case APDS9960_RIGHT:
      musicPitch = constrain(musicPitch + 1, 21, 108);
      sendBluetoothMIDI(0x90, musicPitch, 100);        // Note ON
      Serial.println("Pitch augmenté : " + String(musicPitch));
      break;
    default:
      break;
  }
}

// --- Gestion des mouvements pour effets MIDI ---
void handleMotion() {
  sensors_event_t accelEvent;
  adxl.getEvent(&accelEvent);

  float motionValue = abs(accelEvent.acceleration.x) + abs(accelEvent.acceleration.y) + abs(accelEvent.acceleration.z);
  if (motionValue > motionThreshold) {
    int effectValue = map(motionValue, 0, 20, 0, 127);
    sendBluetoothMIDI(0xB0, 91, effectValue); // CC 91 pour réverbération
    Serial.println("Réverbération ajustée : " + String(effectValue));
  }
}

// --- Lecture de la pression pour effet (optionnel) ---
void handlePressure() {
  sensors_event_t pressureEvent;
  bmp.getEvent(&pressureEvent);
  if (pressureEvent.pressure) {
    Serial.print("Pression atmosphérique : ");
    Serial.print(pressureEvent.pressure);
    Serial.println(" hPa");
  }
}

// --- Bluetooth MIDI Configuration ---
void setupBluetoothMIDI() {
  BLEDevice::init("MusicMotion");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BLEServerCallbacks());

  BLEService* pService = pServer->createService(BLEUUID((uint16_t)0x1812)); // MIDI HID Service
  pCharacteristic = pService->createCharacteristic(
      BLEUUID((uint16_t)0x2A4D),
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->start();
  Serial.println("Bluetooth MIDI prêt.");
}

// --- Gestion des connexions Bluetooth ---
class BLEServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Appareil connecté.");
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Appareil déconnecté.");
  }
};

// --- Envoi des données MIDI ---
void sendBluetoothMIDI(uint8_t status, uint8_t data1, uint8_t data2) {
  midiPacket[0] = status;
  midiPacket[1] = data1;
  midiPacket[2] = data2;
  pCharacteristic->setValue(midiPacket, sizeof(midiPacket));
  pCharacteristic->notify();
}
