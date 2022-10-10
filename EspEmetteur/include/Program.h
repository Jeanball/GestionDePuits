#include <esp_now.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include "Sensor.h"
#include "EspNow.h"
class Program
{
private:
    Sensor *m_sensor; // Sensor object

    unsigned long m_currentMillis; // Current time in the loop
    long m_lastIncrementation;    // Last time the sensor was read
    const long m_interval = 500; // 500ms

    typedef struct struct_SendingMsg // Structure to send data to the ESP
    {
        float volume; // Volume of water in the well
        float hauteur; // Height of water in the well
        float psi; // Pressure of water in the well
        bool initialized; // If the sensor is initialized
    } struct_SendingMsg;
    
    struct_SendingMsg m_espNowSendMsg;   // Structure to send data to the ESP
    esp_now_peer_info_t m_peerInfo; // Structure to store the peer information

public:
    Program();   
    void loop();
};