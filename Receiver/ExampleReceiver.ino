#include <MIDI.h>
#include <BluetoothSerial.h>

MIDI_CREATE_DEFAULT_INSTANCE();

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  
  // Initialisation Bluetooth
  if (!SerialBT.begin("MusicMotion_Receiver")) {
    Serial.println("Erreur de connexion Bluetooth !");
    while (1);
  }
  Serial.println("Récepteur Bluetooth initialisé.");
  
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  if (SerialBT.available()) {
    String data = SerialBT.readStringUntil('\n');
    Serial.println("Données reçues : " + data);
    
    // Traitement des données reçues...
    // Vous pouvez implémenter le code pour interpréter les données et envoyer des messages MIDI correspondants.
  }
}
