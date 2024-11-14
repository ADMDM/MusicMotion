// BraceletMain1.ino

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_BMP280.h>
#include <BH1750.h>
#include <DHT.h>
#include <MIDI.h>
#include <BluetoothSerial.h>

// Initialisation des capteurs
Adafruit_ADXL345_Unified adxl = Adafruit_ADXL345_Unified(12345);
Adafruit_BMP280 bmp; // I2C
BH1750 lightMeter;
DHT dht(4, DHT22); // Data pin = GPIO4

// Initialisation de la bibliothèque MIDI via Bluetooth
BluetoothSerial SerialBT;
MIDI_CREATE_INSTANCE(BluetoothSerial, SerialBT, MIDI);

// LED Pin (Optionnel)
#define LED_PIN 16

// Identifiant du Bracelet
const String BRACELET_ID = "BraceletMain1"; // Modifier pour chaque bracelet

// Variables pour les seuils et états
bool tapDetected = false;
bool freeFallDetected = false;

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
  
  // Configuration des interruptions pour le ADXL345
  setupADXL345Interrupts();
  
  if (!bmp.begin()) { // Adresse I2C par défaut pour BMP280
    Serial.println("Erreur de connexion avec le BMP280 !");
    while (1);
  }
  
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
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
  sensors_event_t accelEvent;
  adxl.getEvent(&accelEvent);
  
  // Lecture des données du BMP280
  float temperatureBMP = bmp.readTemperature();
  float pressure = bmp.readPressure(); // En Pa
  float altitude = bmp.readAltitude(1013.25); // 1013.25 hPa = pression au niveau de la mer
  
  // Lecture des données du BH1750
  float lux = lightMeter.readLightLevel();
  
  // Changement de mode du BH1750 en fonction de la luminosité
  adjustBH1750Mode(lux);
  
  // Lecture des données du DHT22
  float humidity = dht.readHumidity();
  float temperatureDHT = dht.readTemperature();
  
  // Calcul de la température moyenne
  float temperature = (temperatureBMP + temperatureDHT) / 2.0;
  
  // Affichage des données pour le débogage
  Serial.println("=== Capteurs ===");
  Serial.print("Accélération X: "); Serial.println(accelEvent.acceleration.x);
  Serial.print("Accélération Y: "); Serial.println(accelEvent.acceleration.y);
  Serial.print("Accélération Z: "); Serial.println(accelEvent.acceleration.z);
  Serial.print("Température Moyenne: "); Serial.print(temperature); Serial.println(" °C");
  Serial.print("Pression Atmosphérique: "); Serial.print(pressure / 100.0F); Serial.println(" hPa");
  Serial.print("Altitude: "); Serial.print(altitude); Serial.println(" m");
  Serial.print("Luminosité: "); Serial.print(lux); Serial.println(" lx");
  Serial.print("Humidité: "); Serial.print(humidity); Serial.println(" %");
  
  // Envoi des données via Bluetooth sous forme de JSON
  String data = "{\"id\":\"" + BRACELET_ID + "\",";
  data += "\"accel\":{\"x\":" + String(accelEvent.acceleration.x, 2) + ",";
  data += "\"y\":" + String(accelEvent.acceleration.y, 2) + ",";
  data += "\"z\":" + String(accelEvent.acceleration.z, 2) + "},";
  data += "\"temperature\":" + String(temperature, 2) + ",";
  data += "\"pressure\":" + String(pressure, 2) + ",";
  data += "\"altitude\":" + String(altitude, 2) + ",";
  data += "\"lux\":" + String(lux, 2) + ",";
  data += "\"humidity\":" + String(humidity, 2) + "}";
  
  SerialBT.println(data);
  
  // Envoi des messages MIDI basés sur les données des capteurs
  
  // Contrôle continu basé sur l'accélération
  sendAccelDataAsMIDI(accelEvent);
  
  // Utilisation de l'humidité pour moduler un effet
  int midiHumidity = map((int)humidity, 0, 100, 0, 127);
  MIDI.sendControlChange(10, midiHumidity, 1); // Contrôle 10, canal 1
  
  // Utilisation de la luminosité pour contrôler un autre paramètre
  int midiLux = map((int)lux, 0, 1000, 0, 127);
  midiLux = constrain(midiLux, 0, 127);
  MIDI.sendControlChange(11, midiLux, 1); // Contrôle 11, canal 1
  
  // Gestion des interruptions du ADXL345
  handleADXL345Interrupts();
  
  delay(50); // Petite pause avant la prochaine lecture
}

// Fonction pour configurer les interruptions du ADXL345
void setupADXL345Interrupts() {
  // Mettre tous les bits d'interruption à 0
  adxl.writeRegister(ADXL345_INT_ENABLE, 0);
  
  // Configurer la détection de tapotement
  adxl.writeRegister(ADXL345_THRESH_TAP, 50); // Seuil de tapotement
  adxl.writeRegister(ADXL345_DUR, 15); // Durée du tapotement
  adxl.writeRegister(ADXL345_LATENT, 80); // Latence
  adxl.writeRegister(ADXL345_WINDOW, 200); // Fenêtre
  adxl.writeRegister(ADXL345_TAP_AXES, 0x07); // Activer les axes X, Y, Z
  
  // Configurer la détection de chute libre
  adxl.writeRegister(ADXL345_THRESH_FF, 10); // Seuil de chute libre
  adxl.writeRegister(ADXL345_TIME_FF, 20); // Durée de chute libre
  
  // Activer les interruptions pour le tapotement et la chute libre
  adxl.writeRegister(ADXL345_INT_ENABLE, ADXL345_SINGLE_TAP | ADXL345_DOUBLE_TAP | ADXL345_FREE_FALL);
}

// Fonction pour gérer les interruptions du ADXL345
void handleADXL345Interrupts() {
  uint8_t interrupts = adxl.getInterruptSource();
  
  if (interrupts & ADXL345_SINGLE_TAP) {
    Serial.println("Tapotement simple détecté !");
    MIDI.sendNoteOn(60, 127, 1); // Note C4
    MIDI.sendNoteOff(60, 0, 1);
    
    // Feedback LED
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
  }
  
  if (interrupts & ADXL345_DOUBLE_TAP) {
    Serial.println("Tapotement double détecté !");
    MIDI.sendNoteOn(62, 127, 1); // Note D4
    MIDI.sendNoteOff(62, 0, 1);
    
    // Feedback LED
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
  }
  
  if (interrupts & ADXL345_FREE_FALL) {
    Serial.println("Chute libre détectée !");
    MIDI.sendControlChange(20, 127, 1); // Contrôle 20, canal 1
    
    // Feedback LED
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
  }
}

// Fonction pour envoyer les données d'accélération en tant que messages MIDI
void sendAccelDataAsMIDI(sensors_event_t event) {
  int midiValueX = map((int)(event.acceleration.x * 10), -160, 160, 0, 127);
  int midiValueY = map((int)(event.acceleration.y * 10), -160, 160, 0, 127);
  int midiValueZ = map((int)(event.acceleration.z * 10), -160, 160, 0, 127);
  
  midiValueX = constrain(midiValueX, 0, 127);
  midiValueY = constrain(midiValueY, 0, 127);
  midiValueZ = constrain(midiValueZ, 0, 127);
  
  MIDI.sendControlChange(1, midiValueX, 1); // Contrôle 1, canal 1
  MIDI.sendControlChange(2, midiValueY, 1); // Contrôle 2, canal 1
  MIDI.sendControlChange(3, midiValueZ, 1); // Contrôle 3, canal 1
}

// Fonction pour ajuster le mode du BH1750 en fonction de la luminosité
void adjustBH1750Mode(float lux) {
  if (lux < 10) {
    lightMeter.configure(BH1750::CONTINUOUS_HIGH_RES_MODE_2); // Mode haute sensibilité
  } else if (lux > 40000) {
    lightMeter.configure(BH1750::CONTINUOUS_LOW_RES_MODE); // Mode basse sensibilité
  } else {
    lightMeter.configure(BH1750::CONTINUOUS_HIGH_RES_MODE); // Mode normal
  }
}
