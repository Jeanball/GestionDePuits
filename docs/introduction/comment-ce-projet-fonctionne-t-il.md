# Comment ce projet fonctionne t-il?

À l’aide d’un capteur de pression d’eau connecté à une prise murale contrôlé par 2 [ESP32](https://www.espressif.com/en/products/socs/esp32)(Émetteur et receveur), les données du capteur sont donc reçues par le ESP Émetteur et les transmet donc via le protocol [ESP-NOW](https://www.espressif.com/en/products/software/esp-now/overview) vers le ESP Receveur. Ce dernier sera donc connecté au réseau local de la maison en Wi-Fi afin de transmettre les donneées reçues via un protocole de messagerie publish-subscribe ([Mosquitto brocker](https://mosquitto.org/)) vers [Home Assistant](https://www.home-assistant.io/).

Home Assistant traitera les données reçues grâce à [InfluxDB](https://www.influxdata.com/) et [Grafana](https://grafana.com/) afin de faire des statisques et de l'affichage des données.

Home Assistant sera connecté sur un réseau [ZeroTier](https://www.zerotier.com/) afin d'avoir accès à ce serveur n'importe où dans le monde.
