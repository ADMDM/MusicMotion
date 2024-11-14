# MusicMotion

## Présentation

**MusicMotion** est un projet de contrôleur MIDI portable qui permet de transformer les mouvements des mains et des jambes en commandes musicales. En utilisant des microcontrôleurs ESP32 et divers capteurs (ADXL345, BH1750, BMP280, DHT22), ce projet offre une expérience musicale immersive adaptée aux performances live, à la composition musicale, et à l'éducation musicale.

## Fonctionnalités

- **Contrôle MIDI sans fil** : Utilise le Bluetooth Low Energy (BLE) pour communiquer avec un ordinateur ou un appareil mobile.
- **Détection de mouvement** : Les capteurs détectent les mouvements et les gestes pour contrôler les instruments MIDI.
- **Capteurs environnementaux** : Mesure de la luminosité, de la température, de l'humidité et de la pression atmosphérique.
- **Feedback visuel** : Une LED s'allume pour indiquer l'envoi de messages MIDI (optionnel).

## Matériel Nécessaire

### Composants (par bracelet)

- ESP32 Mini Dev Board
- ADXL345 Accéléromètre
- BH1750 Capteur de Lumière
- BMP280 Capteur de Pression
- DHT22 Capteur de Température et Humidité
- Breadboard Mini ou PCB Prototype
- Câbles Dupont
- Résistances : 10kΩ (x3), 220Ω (pour la LED)
- Condensateurs : 100nF (x4)
- LED (optionnel)
- Batterie Li-Po 3.7V
- Module de Recharge TP4056
- Interrupteur SPST
- Boîtier Bracelet
- Ruban Élastique ou Sangle

### Outils

- Ordinateur avec Arduino IDE installé
- Logiciel DAW compatible MIDI (ex. Ableton Live, FL Studio)
- Multimètre
- Fer à souder et fil à souder
- Colle forte ou silicone

## Installation

Veuillez consulter le [Guide d'Installation](docs/setup_guide.md) pour les instructions détaillées sur l'assemblage matériel et le téléversement du code.

## Utilisation

Une fois les bracelets assemblés et le code téléversé, vous pouvez les utiliser pour contrôler des instruments MIDI via votre DAW préféré. Consultez le [Manuel Utilisateur](docs/user_manual.md) pour plus de détails.

## Licence

Ce projet est sous licence MIT. Voir le fichier [LICENSE](LICENSE) pour plus d'informations.
