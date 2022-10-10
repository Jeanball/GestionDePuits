# ESP-32

_Les cartes ESP32 sont des cartes très polyvalentes orientées IOT. Elles intègrent la gestion du wifi, du Bluetooth, du « touch » et une gestion des LiPo sur certaines cartes._

## Dans notre cas, nous utiliserons deux ESP32.

* **Le premier** sera placer près du puit du client et servira à capter les données du capteur de niveau d'eau qui y seras\
  connecter. Il utilisera un protocole nommé ESP\_NOW pour envoyer les données au deuxième ESP.
* **Le deuxième** sera à proximité du réseau wifi, recevra les données du premier ESP et les transmettra au serveur MQTT du client.\
  _\*Ce ESP devra être configurer seulement la première fois pour se connecter au réseau wifi_

### Language de programmation

* C++

### Bibliothèques utilisés

* WiFiManager
* ESP\_NOW
* PubSubClient
* Wire
* Spiffs
