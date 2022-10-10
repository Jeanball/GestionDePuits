#include <Arduino.h>
#include "Wire.h"

class Sensor
{
private:
// VARIABLES
    unsigned long m_currentMillis; // current time
    long m_lastIncrementation;     // last time the sensor was read
    const long m_interval = 2000;  // 2 secondes
public:
    Sensor();
    void loop();
    // READING VARIABLES
    unsigned int m_readings[20] = {0}; // array to store the readings from the analog input
    unsigned char m_readCnt = 0;       // counter to keep track of where to store the next reading

    // SENSOR CALIBRATIONS VARIABLES
    const int m_pressureInput = 4;             // select the analog input pin for the pressure transducer
    const int m_pressureZero = 3900.0;         // analog reading of pressure transducer at 0psi
    const int m_pressureMax = 22800.0;         // analog reading of pressure transducer at 100psi
    const int m_pressuretransducermaxPSI = 18; // psi value of transducer being used
    const int m_baudRate = 115200;             // constant integer to set the baud rate for serial monitor
    const int m_sensorreadDelay = 250;         // constant integer to set the sensor read delay in milliseconds

    const int m_I2Caddress = 0x48; // I2C address of the pressure transducer

    // WELL VARIABLES
    int m_type;        // 0 = cylinder, 1 = rectangle
    float m_wellHeight; // in meters
    float m_wellWidth;  // in meters
    float m_wellLength; // in meters
    float m_volume;     // in liters

    // PRESSURE VARIABLES
    float m_pressureValue; // variable to store the value coming from the pressure transducer
    float m_waterHeight;   // variable to store the value of the water height in meters
};
