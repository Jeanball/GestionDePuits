#include <Arduino.h>

class EspNow
{
private:
public:
    // VARIABLES
    String m_macStr; // String to store the mac address of the ESP
    int32_t m_channel; // Channel of the ESP
    uint8_t m_AddressMacReceveurESP[6]; // Mac address of the receiving ESP
    int m_errorCount; // Number of connections errors

    typedef struct m_struct_ReceivingMsg 
    {
        String message;  // Message to send to the ESP
        String type; // Type of well
        String height; // Height of the well
        String largeur; // Width of the well
        String length; // Length of the well
        bool espRestarted; // If the ESP has restarted
    } m_struct_ReceivingMsg; // Structure to store the data received from the ESP
    m_struct_ReceivingMsg m_espNowReceivingMsg;  // Object to store the data received from the ESP

    float m_height; // Height of the well
    float m_width; // Width of the well
    float m_length; // Length of the well
    int m_type; // Type of the well
    int m_espNotInitialized; // Number of times the ESP has not been initialized
    bool m_espToRestart; // If the ESP has to be restarted
    // constructor
    EspNow();
};