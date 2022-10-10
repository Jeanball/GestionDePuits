#include "Program.h"

void OnDataRecv(const uint8_t *mac, const uint8_t *donneeRecu, int len);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
esp_now_peer_info_t peerInfo;
EspNow *espNow;

Program::Program()
{
    // Initialize the sensor
    Serial.begin(115200); // Initialize the serial port
    WiFi.disconnect(true); // Disconnect from the network
    WiFi.persistent(false); // Disable the persistent mode
    WiFi.mode(WIFI_OFF); // Turn off the wifi
    WiFi.mode(WIFI_STA); // Set the wifi mode to station

    espNow = new EspNow(); // Create the EspNow object

    m_peerInfo = {}; // Initialize the peer info structure
    espNow->m_espNowReceivingMsg.message = "NOT INITIALIZED"; // Set the message to not initialized

    esp_wifi_set_channel(espNow->m_channel, WIFI_SECOND_CHAN_NONE); // Set the channel of the ESP
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false)); // Disable the promiscuous mode
    // Initialisation ESP_NOW
    if (esp_now_init() != ESP_OK) 
    {
        Serial.println("Error initializing ESP-NOW"); 
    }

    memcpy(m_peerInfo.peer_addr, espNow->m_AddressMacReceveurESP, 6); // Copy the mac address of the receiving ESP
    esp_now_register_send_cb(OnDataSent); // Register the send callback
    esp_now_register_recv_cb(OnDataRecv); // Register the receive callback

    if (esp_now_add_peer(&m_peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add peer");
    }

    this->m_sensor = new Sensor(); // Create the sensor object
}

void Program::loop()
{
    this->m_sensor->loop();

    this->m_currentMillis = millis();
    if (this->m_currentMillis - this->m_lastIncrementation >= this->m_interval)
    {
        this->m_sensor->m_type = espNow->m_type; // Set the type of the well
        this->m_sensor->m_wellWidth = espNow->m_width; // Set the width of the well
        this->m_sensor->m_wellHeight = espNow->m_height; // Set the height of the well
        this->m_sensor->m_wellLength = espNow->m_length; // Set the length of the well
        Serial.println("");
        Serial.println("-------Sending data-------");
        Serial.print("TO MAC : ");
        for (int i = 0; i < 6; i++) // Print the mac address of the receiving ESP
        {
            Serial.print(espNow->m_AddressMacReceveurESP[i], HEX);
            if (i < 5)
            {
                Serial.print(":");
            }
        }
        Serial.println();
        Serial.println("Volume : " + String(m_sensor->m_volume));
        Serial.println("Height : " + String(m_sensor->m_waterHeight));
        Serial.println("PSI : " + String(m_sensor->m_pressureValue));
        Serial.println("------- Well Dimensions -------");
        Serial.println("Type : " + String(espNow->m_type));
        Serial.println("Height : " + String(espNow->m_height));
        Serial.println("Width : " + String(espNow->m_width));
        Serial.println("Length : " + String(espNow->m_length));


        this->m_espNowSendMsg.volume = this->m_sensor->m_volume; // Set the volume to send
        this->m_espNowSendMsg.hauteur = this->m_sensor->m_waterHeight; // Set the water height to send
        this->m_espNowSendMsg.psi = this->m_sensor->m_pressureValue; // Set the pressure to send

        if (this->m_sensor->m_pressureValue > 0 && this->m_sensor->m_volume <= 0) //Prevent sending data before the calculation for the volume is done
        {
            this->m_espNowSendMsg.initialized = false;            
            delay(3000);
            Serial.println("This Device Mac Address : "+ String(WiFi.macAddress()));
            Serial.println("");
            Serial.println("NOT SENDING DATA");
        }
        else
        {
            this->m_espNowSendMsg.initialized = true;
            esp_err_t result = esp_now_send(espNow->m_AddressMacReceveurESP, (uint8_t *)&m_espNowSendMsg, sizeof(m_espNowSendMsg)); // Send the data to the receiving ESP
            Serial.println("");
            Serial.println("SENDING DATA");

            if (result == ESP_OK)
            {
            }
            else
            {
                Serial.println("Error sending the data");
            }
        }

        this->m_lastIncrementation = m_currentMillis; // Set the last incrementation to the current millis
    }
}
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    if (status != ESP_NOW_SEND_SUCCESS)
    {
        Serial.println(String("Wifi channel: ") + espNow->m_channel);
        Serial.println(String("Fail to connect : ") + espNow->m_errorCount);
        espNow->m_errorCount++; 
        espNow->m_channel++;
        //Try to connect to the next channel
        if (espNow->m_channel > 13)
        {
            espNow->m_channel = 1;
        }
        esp_wifi_set_channel(espNow->m_channel, WIFI_SECOND_CHAN_NONE); // Set the channel of the ESP
    }
    else
    {
        espNow->m_errorCount = 0;
    }

    if (espNow->m_errorCount == 20)
    {
        ESP.restart(); // Restart the ESP if it can't connect to the receiving ESP
    }
}

void OnDataRecv(const uint8_t *mac, const uint8_t *donneeRecu, int len)
{

    memcpy(&espNow->m_espNowReceivingMsg, donneeRecu, sizeof(espNow->m_espNowReceivingMsg)); // Copy the data received to the receiving message structure
    memcpy(peerInfo.peer_addr, mac, 6); // Copy the mac address of the sender
    for (int i = 0; i < 6; i++)
    {
        espNow->m_AddressMacReceveurESP[i] = mac[i];
    }

    memcpy(peerInfo.peer_addr, espNow->m_AddressMacReceveurESP, 6); // Copy the mac address of the receiving ESP
    esp_now_add_peer(&peerInfo); // Add the peer to the ESP

    //Convert received data
    espNow->m_type = espNow->m_espNowReceivingMsg.type.toInt();
    espNow->m_height = espNow->m_espNowReceivingMsg.height.toFloat();
    espNow->m_width = espNow->m_espNowReceivingMsg.largeur.toFloat();
    espNow->m_length = espNow->m_espNowReceivingMsg.length.toFloat();

    if (espNow->m_espNowReceivingMsg.espRestarted == true)
    {
        ESP.restart();
    }
}