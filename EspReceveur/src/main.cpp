// Projet Puits ESPReceveur V1.0 




// Besoin d'être importé avant les autres bibliothèques
#define ESP_DRD_USE_SPIFFS true

//#define RESET_MODE ;

// Fichier JSON
#define JSON_CONFIG_FILE "/config.json"

// Bibliothèques
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <Arduino.h>
#include <WiFiManager.h>
#include "esp_now.h"
#include "PubSubClient.h"
#include <ArduinoJson.h>

esp_now_peer_info_t peerInfo;
int32_t channel = 10;
int nbErreur = 0;
uint8_t AdresseMacSensorInt[6] = {0x12, 0x34, 0x56, 0x78, 0x90, 0x12};

int lastMsg = 0;

typedef struct struct_message2
{
  String message;
  String type;
  String longeur;
  String largeur;
  String hauteur;
  bool restart = false;
} struct_message2;
struct_message2 myData2;

unsigned long currentMillis;
long m_derniereIncrementation;
const long interval = 500;

bool initialConfig = false;

String ssid;
String password;

// Déclarations des fonctions et callback

String getCustomParamValue(WiFiManager *myWiFiManager, String name);
void saveConfigFile();
bool loadConfigFile();
void saveConfigCallback();
void ConnexionMQTT();
void OnDataRecv(const uint8_t *mac, const uint8_t *donneeRecu, int len);
void callbackMQTT(char *topic, byte *payload, unsigned int length);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void sendData();
void saveFile();
void InitialiserDonneesSensor();
void InitialiserSPIFF();
void InitWiFiParameters();
void InitCustomWiFiManagerParameter();
void InitConnexion();
void InitESPNOW();
void MacAddressSetup();
void MQTTFirstConnexion();

// Variable servant a publier les données au serveur mqtt
char donneeTempMQTT[80];

// Variables pour la configuration des connexions
bool shouldSaveConfig = false;

// Variables pour la forme du puit
int formatDuPuit = 0; // 0 == Monday etc
char *choixDuFormat[] = {"Circulaire", "Rectangulaire"};

// Variables de paramètre personnalisé
char MQTTIpAddress[50];
char MQTTPort[50];
char MQTTUser[50];
char MQTTPassword[50];
char MacAdresseSensorString[50];
char largeurDuPuit[50];
char longueurDuPuit[50];
char hauteurDuPuit[50];
char bufferStr[700];
char valeurConvertis[6];

WiFiManagerParameter MQTTIpAddressConfig("Adresse_IP_Serveur_MQTT", "Adresse IP du serveur MQTT", MQTTIpAddress, 50);         // 50 == max length
WiFiManagerParameter MQTTPortConfig("Port_Serveur_MQTT", "Port du serveur MQTT", MQTTPort, 50);                               // 50 == max length
WiFiManagerParameter MQTTUserConfig("Nom_utilisateur", "Nom d'utilisateur MQTT", MQTTUser, 50);                               // 50 == max length
WiFiManagerParameter MQTTPasswordConfig("MotDePasse", "Mot de passe MQTT", MQTTPassword, 50);                                 // 50 == max length
WiFiManagerParameter MacAdresseEmetteurConfig("MacAdresseEmetteur", "Mac adresse de l'émetteur", MacAdresseSensorString, 50); // 50 == max length
WiFiManagerParameter LargeurWell("Largeur", "Largeur (metres)", largeurDuPuit, 50);                                           // 50 == max length
WiFiManagerParameter LongueurWell("Longueur", "Longueur (metres)", longueurDuPuit, 50);
WiFiManagerParameter HauteurWell("Hauteur", "Hauteur (metres, rectangle seulement)", hauteurDuPuit, 50);
WiFiManagerParameter custom_hidden("key_custom", "Will be hidden", valeurConvertis, 2);
WiFiManagerParameter custom_field(bufferStr);

// Variables pour WIFI
WiFiClient espClient;

// Variables pour la connexion au serveur MQTT
int nombreEssaieConnexion = 0;
int nombreEssaieConnexionMax = 0;
bool connexionMQTT = false;
int mqttPort;
PubSubClient mqttClient(MQTTIpAddress, mqttPort, callbackMQTT, espClient);

// Variables des MAC address
uint8_t AdressMacEmetteurESP[6];
uint8_t AdresseMacReceveurESP[6];

// Structure pour la réception de données ESP_NOW
typedef struct struct_message
{
  float hauteur;
  float profondeur;
  float pression;
  bool initialized;
} struct_message;
struct_message donnees;
WiFiManager wm;
float myTimer;
//********************************************************************************************************************************************************************
void setup()
{
  Serial.begin(115200);

  // Initialisation des données pas défaut
  InitialiserDonneesSensor();

  // Initialisation de SPIFFS
  InitialiserSPIFF();

  // Basic WiFi Parameters Initialisation
  InitWiFiParameters();

  // Custom WiFiManager Parameters Initialisation
  InitCustomWiFiManagerParameter();

  // Init Mac Address variable
  peerInfo = {};

  // Démarrage de WifiManager, ESP_NOW & MQTT
  InitConnexion();
}

//***********************************************************************************************************************************************************
void loop()
{

  float startTime = millis();

  if (millis() - myTimer > 1000 && initialConfig == true)
  {
    myTimer = millis();
    lastMsg++;
    Serial.println("Dernier message : " + String(lastMsg) + " secondes");
  }

  if (lastMsg > 50 && initialConfig == true)
  {
    ESP.restart();
  }

  if (initialConfig == false)
  {
    currentMillis = millis();
    if (currentMillis - m_derniereIncrementation >= interval)
    {

      myData2.message = "INITIALIZED";
      myData2.type = formatDuPuit;
      myData2.largeur = largeurDuPuit;
      myData2.longeur = longueurDuPuit;
      myData2.hauteur = hauteurDuPuit;

      esp_err_t result = esp_now_send(AdresseMacSensorInt, (uint8_t *)&myData2, sizeof(myData2));
      if (result == ESP_OK)
      {
      }
      else
      {
        Serial.println("Error sending the data");
      }

      m_derniereIncrementation = currentMillis;
    }
  }

  if (WiFi.isConnected())
  {
    mqttClient.loop();
    if (mqttClient.connected() == false)
    {
      ConnexionMQTT();
      mqttClient.subscribe("wifiManager");
    }
  }
}

// Callback

void callbackMQTT(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Callback - ");
  Serial.println("Message:");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }

  // put the message in a string
  String messageTemp;
  for (int i = 0; i < length; i++)
  {
    messageTemp += (char)payload[i];
  }

  // if message is "reset" then reset the ESP and start the configuration portal
  if (messageTemp == "reset")
  {
    Serial.println("Resetting ESP");
    delay(25);
    wm.startConfigPortal("Puits_Portail", "Puits2022");
    ESP.restart();
  }
  if (messageTemp == "restart")
  {
    Serial.println("Restarting ESP");

    myData2.restart = true;
    esp_err_t result = esp_now_send(AdresseMacSensorInt, (uint8_t *)&myData2, sizeof(myData2));
    delay(5000);
    ESP.restart();
  }
}
void saveConfigFile()
{
  Serial.println(F("Saving config"));
  StaticJsonDocument<512> json;

  json["MQTTIpAddress"] = MQTTIpAddress;
  json["MQTTPort"] = MQTTPort;
  json["MQTTUser"] = MQTTUser;
  json["MQTTPassword"] = MQTTPassword;
  json["MacAdresseEmetteur"] = MacAdresseSensorString;
  json["Longeur"] = longueurDuPuit;
  json["Largeur"] = largeurDuPuit;
  json["Hauteur"] = hauteurDuPuit;
  json["format"] = formatDuPuit;
  File configFile = SPIFFS.open(JSON_CONFIG_FILE, "w");
  if (!configFile)
  {
    Serial.println("failed to open config file for writing");
  }

  // serializeJsonPretty(json, Serial);
  if (serializeJson(json, configFile) == 0)
  {
    Serial.println(F("Failed to write to file"));
  }
  configFile.close();
}
// callback
// Enregistre les données du SPIFF vers nos variables globales (variables utilisables par PubSubClient)
bool loadConfigFile()
{
  // clean FS, for testing
  //  SPIFFS.format();

  // read configuration from FS json
  Serial.println("mounting File system...");

  if (SPIFFS.begin(false) || SPIFFS.begin(true))
  {
    Serial.println("mounted file system");
    if (SPIFFS.exists(JSON_CONFIG_FILE))
    {
      // file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open(JSON_CONFIG_FILE, "r");
      if (configFile)
      {
        // Serial.println("opened config file");
        StaticJsonDocument<512> json;
        DeserializationError error = deserializeJson(json, configFile);
        // serializeJsonPretty(json, Serial);
        if (!error)
        {
          // Serial.println("\nparsed json");
          strcpy(MQTTIpAddress, json["MQTTIpAddress"]);
          strcpy(MQTTPort, json["MQTTPort"]);
          strcpy(MQTTUser, json["MQTTUser"]);
          strcpy(MQTTPassword, json["MQTTPassword"]);
          strcpy(MacAdresseSensorString, json["MacAdresseEmetteur"]);
          strcpy(longueurDuPuit, json["Longeur"]);
          strcpy(largeurDuPuit, json["Largeur"]);
          strcpy(hauteurDuPuit, json["Hauteur"]);
          formatDuPuit = json["format"].as<int>();

          return true;
        }
        else
        {
          Serial.println("failed to load json config");
        }
      }
    }
  }
  else
  {
    Serial.println("failed to mount File system");
  }
  // end read
  return false;
}
// callback
// notifying us of the need to save config
void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// Custom HTML WiFiManagerParameter don't support getValue directly
String getCustomParamValue(WiFiManager *myWiFiManager, String name)
{
  String value;

  int numArgs = myWiFiManager->server->args();

  if (myWiFiManager->server->hasArg(name))
  {
    value = myWiFiManager->server->arg(name);
  }
  return value;
}

void ConnexionMQTT()
{
  nombreEssaieConnexion = 0;
  Serial.println("connecting to MQTT Broker..");
  Serial.println("Port: " + String(mqttPort));
  Serial.println("User:" + String(MQTTUser));
  Serial.println("Password:" + String(MQTTPassword));

  while (mqttClient.connect("ESPReceveur", MQTTUser, MQTTPassword) == false)
  {

    if (connexionMQTT == false)
    {
      nombreEssaieConnexionMax = 500;
    }
    else
    {
      nombreEssaieConnexionMax = 20000;
    }

    Serial.println("Not Connected to MQTT Nombre d'essaie: " + String(nombreEssaieConnexion));
    if (nombreEssaieConnexion > nombreEssaieConnexionMax)
    {
      nombreEssaieConnexion = 0;
      // open config portal
      wm.startConfigPortal("Puits_Portail", "Puits2022");
      saveFile();
      delay(1000);
      ESP.restart();
    }
    nombreEssaieConnexion++;
  }
  nombreEssaieConnexion = 0;
  Serial.println("Connected to MQTT");
  mqttClient.subscribe("wifiManager");
}

// Réception des données ESP_NOW ET Envoies des données au serveur MQTT
void OnDataRecv(const uint8_t *mac, const uint8_t *donneeRecu, int len)
{

  lastMsg = 0;

  // Transfère des données recu vers nos variables locales
  memcpy(&donnees, donneeRecu, sizeof(donnees));

  // Vérification sur le port Serial
  Serial.println("Hauteur: " + String(donnees.hauteur));
  Serial.println("Profondeur: " + String(donnees.profondeur));
  Serial.println("PSI: " + String(donnees.pression));
  Serial.println("Initialized: " + String(donnees.initialized));
  Serial.println("Mac: " + String(MacAdresseSensorString));
  Serial.println("Largeur: " + String(largeurDuPuit));
  Serial.println("Longeur: " + String(longueurDuPuit));
  Serial.println("Hauteur du puit: " + String(hauteurDuPuit));
  Serial.println("Type: " + String(formatDuPuit));
  Serial.println("Choix du format: " + String(choixDuFormat[formatDuPuit]));

  if (donnees.hauteur < 0 || donnees.profondeur < 0 || donnees.pression < 0)
  {
    donnees.hauteur = 0;
    donnees.pression = 0;
    donnees.profondeur = 0;
  }

  // check if conencted to wifi
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Connected to WiFi");
  }
  else
  {
    Serial.println("Wifi not connected");
  }

  // check if connected to mqtt
  if (mqttClient.connected())
  {
    Serial.println("Connected to MQTT");
  }
  else
  {
    Serial.println("MQTT not connected");
  }

  // ENVOIS DES DONNÉES AU SERVEUR MQTT
  sprintf(donneeTempMQTT, "%f", donnees.hauteur);
  if (mqttClient.publish("sensor/niveauEau", donneeTempMQTT))
  {
    Serial.println("Donnée sur la hauteur envoyé à MQTT.");
  }
  sprintf(donneeTempMQTT, "%f", donnees.profondeur);
  if (mqttClient.publish("sensor/hauteur", donneeTempMQTT))
  {
    Serial.println("Donnée sur la profondeur envoyé à MQTT.");
  }
  sprintf(donneeTempMQTT, "%f", donnees.pression);
  if (mqttClient.publish("sensor/psi", donneeTempMQTT))
  {
    Serial.println("Donnée sur la pression(psi) envoyé à MQTT.");
  }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nTransfer des données d'initilisation vers senseur:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Bon Transfer ! ! !" : "Echec du transfer changement de channel WiFi ! ! !");
  if (status != ESP_NOW_SEND_SUCCESS)
  {
    Serial.println(String("Channel WiFi: ") + channel);
    Serial.println(String("Echec du transfer vers senseur # ") + nbErreur + "( Redémarre a 50)");
    nbErreur++;
    WiFi.disconnect();

    channel++;
    if (channel > 13)
    {
      channel = 1;
    }
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  }
  else
  {
    nbErreur = 0;
    initialConfig = true;
    WiFi.begin();
  }

  if (nbErreur == 50)
  {

    ESP.restart();
  }
}

void sendData()
{
  while (initialConfig == false)
  {
    currentMillis = millis();
    if (currentMillis - m_derniereIncrementation >= interval)
    {
      myData2.message = "INITIALIZED";
      myData2.type = formatDuPuit;
      myData2.largeur = largeurDuPuit;
      myData2.longeur = longueurDuPuit;
      myData2.hauteur = hauteurDuPuit;

      esp_err_t result = esp_now_send(AdresseMacSensorInt, (uint8_t *)&myData2, sizeof(myData2));
      if (!result == ESP_OK)
      {
        Serial.println("Error sending the data");
      }
      else
      {
        Serial.println("Data sent");
      }

      m_derniereIncrementation = currentMillis;
    }
  }
}

void saveFile()
{
  delay(3000);
  // save the custom parameters to FS
  if (shouldSaveConfig)
  {

    // Copy the string value
    strncpy(MQTTIpAddress, MQTTIpAddressConfig.getValue(), sizeof(MQTTIpAddress));
    strncpy(MQTTPort, MQTTPortConfig.getValue(), sizeof(MQTTPort));
    strncpy(MQTTUser, MQTTUserConfig.getValue(), sizeof(MQTTUser));
    strncpy(MQTTPassword, MQTTPasswordConfig.getValue(), sizeof(MQTTPassword));
    strncpy(MacAdresseSensorString, MacAdresseEmetteurConfig.getValue(), sizeof(MacAdresseSensorString));
    strncpy(largeurDuPuit, LargeurWell.getValue(), sizeof(largeurDuPuit));
    strncpy(longueurDuPuit, LongueurWell.getValue(), sizeof(longueurDuPuit));
    strncpy(hauteurDuPuit, HauteurWell.getValue(), sizeof(hauteurDuPuit));

    // The custom one
    formatDuPuit = atoi(custom_hidden.getValue());
    saveConfigFile();
  }
}

void InitialiserDonneesSensor()
{
  donnees.profondeur = 0;
  donnees.hauteur = 0;
  donnees.pression = 0;
}

void InitialiserSPIFF()
{
  bool spiffsSetup = loadConfigFile();
  if (!spiffsSetup)
  {
    Serial.println(F("Forcing config mode as there is no saved config"));
  }
}

void InitWiFiParameters()
{
  WiFi.persistent(true);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_AP_STA);

  // Configuration pour WifiManager
  wm.setConnectRetries(2);
  wm.setConnectTimeout(30);
  wm.setConfigPortalTimeout(300);

  // Initialisation des Callbacks
  wm.setSaveConfigCallback(saveConfigCallback);

#ifdef RESET_MODE
  wm.resetSettings(); // Sert à 'reset' les configurations de WifiManager
#endif
}

void InitCustomWiFiManagerParameter()
{
  // Select menu (custom HTML)
  //  The custom html options do not get handled the same way as other standard ones
  const char *numChoixPuit = R"(
    <h3>Format du puit</h3>
  <br/><label for='format'>Type</label>
  <select name="Format du puit" id="format" onchange="document.getElementById('key_custom').value = this.value">
    <option value="0">Circulaire</option>
    <option value="1">Rectangulaire</option>
  </select>
  <script>
    document.getElementById('format').value = "%d";
    document.querySelector("[for='key_custom']").hidden = true;
    document.getElementById('key_custom').hidden = true;
  </script>
  )";

  // The sprintf is so we can input the value of the current selected day
  // If you dont need to do that, then just pass the const char* straight in.
  sprintf(bufferStr, numChoixPuit, formatDuPuit);

  // Hidden field to get the data
  sprintf(valeurConvertis, "%d", formatDuPuit); // Need to convert to string to display a default value.

  // add all your parameters here
  wm.addParameter(&MQTTIpAddressConfig);
  wm.addParameter(&MQTTPortConfig);
  wm.addParameter(&MQTTUserConfig);
  wm.addParameter(&MQTTPasswordConfig);
  wm.addParameter(&MacAdresseEmetteurConfig);
  wm.addParameter(&custom_hidden); // Needs to be added before the javascript that hides it
  wm.addParameter(&custom_field);
  wm.addParameter(&LargeurWell);
  wm.addParameter(&LongueurWell);
  wm.addParameter(&HauteurWell);
}

void InitConnexion()
{
  bool resultatConnexion = wm.autoConnect("Puits_Portail", "Puits2022");

  // Vérification de l'état de la connexion Wifi
  if (!resultatConnexion)
  {
    Serial.println("Failed to connect");
    ESP.restart();
  }
  else
  {
    Serial.println("connected to WiFi ! ! !");

    saveFile();

    MacAddressSetup();

    MQTTFirstConnexion();

    InitESPNOW();
  }
}

void MacAddressSetup()
{
  // convert to uint8_t array
  uint8_t mac[6];
  sscanf(MacAdresseSensorString, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

  // set the mac address to AdresseMacReceveurESP2
  for (int i = 0; i < 6; i++)
  {
    AdresseMacSensorInt[i] = mac[i];
  }
}

void MQTTFirstConnexion()
{
  //**********************************************************************************************************
  // Connexion Initial au serveur MQTT
  mqttPort = atoi(MQTTPort);

  mqttClient.setServer(MQTTIpAddress, mqttPort);
  if (mqttClient.connect("ESPReceveur", MQTTUser, MQTTPassword))
  {
    Serial.println("MQTT Connected");
    connexionMQTT = true;
    mqttClient.subscribe("wifiManager");
  }
  else
  {
    Serial.println("MQTT Not Connected");
  }
  // if a message is received on topic "wifiManager", the callback function "callback" is called
  mqttClient.setCallback(callbackMQTT);
}

void InitESPNOW()
{
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("There was an error initializing ESP-NOW");
  }
  else
  {
    Serial.println("ESP_NOW Initialized");

    // Enregistrement de la réception de msg ESP_NOW
    memcpy(peerInfo.peer_addr, AdresseMacSensorInt, 6);
    esp_now_register_recv_cb(OnDataRecv);
    esp_now_register_send_cb(OnDataSent);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.println("Failed to add peer");
    }
  }
}
