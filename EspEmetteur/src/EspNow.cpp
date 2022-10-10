#include "EspNow.h"

EspNow::EspNow()
{
    m_channel = 9;
    m_errorCount = 0;
    m_espNotInitialized = 0;
    m_espToRestart = false;
    m_height = 0;
    m_width = 0;
    m_length = 0;
    m_type = 0;    
    m_macStr = "00:00:00:00:00:00";
    m_espNowReceivingMsg.message = "";
    m_espNowReceivingMsg.type = "";
    m_espNowReceivingMsg.height = "";
    m_espNowReceivingMsg.largeur = "";
    m_espNowReceivingMsg.length = "";
    m_espNowReceivingMsg.espRestarted = false;

    //Random mac address for initialisation because null is taken as local broadcast address
    for (int i = 0; i < 6; i++){
        m_AddressMacReceveurESP[i] = 0x00 + i;
    }
}