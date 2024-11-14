#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BH1750.h>
#include <DHT.h>
#include <MIDI.h>
#include <BluetoothSerial.h>

// Initialisation des capteurs
Adafruit_ADXL345_Unified adxl = Adafruit_ADXL345_Unified(12345);
Adafruit_BMP280 bmp; // I2C
Adafruit_BH1750 lightMeter;
DHT dht(4, DHT22); // Data pin = GPIO4

// Initialisation de la bibliothèque MIDI via Bluetooth
MIDI_CREATE_INSTANCE(HardwareSerial, SerialBT, MIDI);

BluetoothSerial SerialBT;

// LED Pin (Optionnel)
#define LED_PIN 16

// Identifiant du Bracelet
const String BRACELET_ID = "BraceletMain1"; // Modifier pour chaque bracelet

void setup() {
  // Initialisation de la communication série pour le débogage
  Serial.begin(115200);
  
  // Initialisation Bluetooth
  if (!SerialBT.begin("MusicMotion_" + BRACELET_ID)) {
    Serial.println("Erreur de connexion Bluetooth !");
    while (1);
  }
  Serial.println("Bluetooth initialisé.");
  
  // Initialisation de la communication MIDI
  MIDI.begin(MIDI_CHANNEL_OMNI);
  
  // Initialisation des capteurs
  if (!adxl.begin()) {
    Serial.println("Erreur de connexion avec l'ADXL345 !");
    while (1);
  }
  adxl.setRange(ADXL345_RANGE_16_G);
  
  if (!bmp.begin(0x76)) { // Adresse I2C par défaut pour BMP280
    Serial.println("Erreur de connexion avec le BMP280 !");
    while (1);
  }
  
  if (!lightMeter.begin()) {
    Serial.println("Erreur de connexion avec le BH1750 !");
    while (1);
  }
  
  dht.begin();
  
  // Initialisation de la LED (Optionnel)
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("Bracelet MIDI Ready!");
}

void loop() {
  // Lecture des données du ADXL345
  sensors_event_t event;
  adxl.getEvent(&event);
  
  // Lecture des données du BMP280
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0F; // Convertir en hPa
  
  // Lecture des données du BH1750
  float lux = lightMeter.readLightLevel();
  
  // Lecture des données du DHT22
  float humidity = dht.readHumidity();
  float tempDHT = dht.readTemperature();
  
  // Affichage des données pour le débogage
  Serial.println("=== Capteurs ===");
  Serial.print("Accélération X: "); Serial.println(event.acceleration.x);
  Serial.print("Accélération Y: "); Serial.println(event.acceleration.y);
  Serial.print("Accélération Z: "); Serial.println(event.acceleration.z);
  Serial.print("Température (BMP280): "); Serial.print(temperature); Serial.println(" °C");
  Serial.print("Pression Atmosphérique: "); Serial.print(pressure); Serial.println(" hPa");
  Serial.print("Luminosité: "); Serial.print(lux); Serial.println(" lx");
  Serial.print("Humidité (DHT22): "); Serial.print(humidity); Serial.println(" %");
  Serial.print("Température (DHT22): "); Serial.print(tempDHT); Serial.println(" °C");
  
  // Envoi des données via Bluetooth
  String data = BRACELET_ID + ",";
  data += String(event.acceleration.x) + ",";
  data += String(event.acceleration.y) + ",";
  data += String(event.acceleration.z) + ",";
  data += String(temperature) + ",";
  data += String(pressure) + ",";
  data += String(lux) + ",";
  data += String(humidity) + ",";
  data += String(tempDHT);
  
  SerialBT.println(data);
  
  // Envoi des messages MIDI basés sur l'accélération
  int midiValueX = map((int)event.acceleration.x, -10, 10, 0, 127);
  int midiValueY = map((int)event.acceleration.y, -10, 10, 0, 127);
  int midiValueZ = map((int)event.acceleration.z, -10, 10, 0, 127);
  
  MIDI.sendControlChange(1, midiValueX, 1); // Contrôle 1, canal 1
  MIDI.sendControlChange(2, midiValueY, 1); // Contrôle 2, canal 1
  MIDI.sendControlChange(3, midiValueZ, 1); // Contrôle 3, canal 1
  
  Serial.println("MIDI Control Change envoyé.");
  
  // Feedback LED
  digitalWrite(LED_PIN, HIGH);
  delay(50);
  digitalWrite(LED_PIN, LOW);
  
  delay(100); // Pause avant la prochaine lecture
}
