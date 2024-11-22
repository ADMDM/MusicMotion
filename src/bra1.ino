#include <Wire.h>
#include <Adafruit_APDS9960.h>
#include <Adafruit_ADXL345_U.h>
#include <MPU6050.h>

// Déclaration des objets pour les capteurs
Adafruit_APDS9960 apds;
Adafruit_ADXL345_Unified adxl = Adafruit_ADXL345_Unified();
MPU6050 mpu;

void setup() {
  // Initialisation de la communication série
  Serial.begin(115200);

  // Initialisation de l'I2C avec les broches GPIO21 (SDA) et GPIO22 (SCL)
  Wire.begin(21, 22);
  
  // Initialisation des capteurs
  if (!apds.begin()) {
    Serial.println("Erreur d'initialisation du APDS-9960");
    while (1);
  }
  if (!adxl.begin()) {
    Serial.println("Erreur d'initialisation du ADXL345");
    while (1);
  }
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("Erreur de connexion au MPU6050");
    while (1);
  }
  Serial.println("Capteurs initialisés avec succès !");
}

void loop() {
  // Lire les gestes du capteur APDS9960
  if (apds.isGestureAvailable()) {
    int gesture = apds.readGesture();
    if (gesture == APDS9960_UP) {
      Serial.println("Geste : Haut");
      // Envoyer ces données au DAW via Bluetooth
    } else if (gesture == APDS9960_DOWN) {
      Serial.println("Geste : Bas");
    }
    // Ajouter d'autres gestes ici...
  }

  // Lire les données de l'ADXL345 (accéléromètre)
  sensors_event_t event;
  adxl.getEvent(&event);
  Serial.print("ADXL345 - X: ");
  Serial.print(event.acceleration.x);
  Serial.print(" Y: ");
  Serial.print(event.acceleration.y);
  Serial.println(" Z: ");
  Serial.println(event.acceleration.z);

  // Lire les données du MPU6050
  Vector rawAccel = mpu.getAcceleration();
  Serial.print("MPU6050 - X: ");
  Serial.print(rawAccel.x);
  Serial.print(", Y: ");
  Serial.print(rawAccel.y);
  Serial.print(", Z: ");
  Serial.println(rawAccel.z);

  // Implémenter l'envoi Bluetooth de ces valeurs vers l'application DAW

  delay(1000);
}
