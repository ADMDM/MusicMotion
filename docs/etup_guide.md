# Guide d'Installation

## Introduction

Ce guide vous aidera à assembler le matériel et à téléverser le code sur les bracelets MusicMotion.

## Matériel Nécessaire

(Listez les composants comme dans le README)

## Étapes d'Assemblage

### 1. Préparation des Composants

- Assurez-vous d'avoir tous les composants nécessaires.
- Vérifiez que l'ESP32 est fonctionnel en le connectant à votre ordinateur.

### 2. Schéma de Connexion

(Insérez ici le schéma de connexion ou une image du diagramme de câblage.)

### 3. Connexions

- **ADXL345** :
  - VCC -> 3V3
  - GND -> GND
  - SDA -> GPIO21
  - SCL -> GPIO22
  - CS -> GND
  - SDO -> GND

- **BH1750** :
  - VCC -> 3V3
  - GND -> GND
  - SDA -> GPIO21
  - SCL -> GPIO22

- **BMP280** :
  - VIN -> 3V3
  - GND -> GND
  - SDA -> GPIO21
  - SCL -> GPIO22

- **DHT22** :
  - VCC -> 3V3
  - GND -> GND
  - Data -> GPIO4 (avec résistance 10kΩ entre Data et VCC)

- **LED (Optionnel)** :
  - Anode -> Résistance 220Ω -> GPIO16
  - Cathode -> GND

### 4. Téléversement du Code

- Ouvrez l'Arduino IDE.
- Installez les bibliothèques nécessaires via le gestionnaire de bibliothèques :
  - Adafruit ADXL345
  - Adafruit BMP280
  - Adafruit BH1750
  - DHT sensor library
  - MIDI Library
- Sélectionnez la carte **ESP32 Dev Module** et le port correspondant.
- Ouvrez le fichier `.ino` du bracelet.
- Modifiez le `BRACELET_ID` si nécessaire.
- Cliquez sur **Téléverser**.

## Calibration et Tests

- Après le téléversement, ouvrez le moniteur série pour vérifier les données des capteurs.
- Assurez-vous que les valeurs semblent correctes.

## Problèmes Courants

- **Erreur de connexion avec un capteur** : Vérifiez les connexions et les adresses I2C.
- **Le Bluetooth ne se connecte pas** : Assurez-vous que le Bluetooth est activé sur votre appareil et que le nom du dispositif est correct.
