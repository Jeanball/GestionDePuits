#include "Sensor.h"
Sensor::Sensor()
{
    this->m_type = 0;       // 0 = cylinder, 1 = rectangle
    this->m_wellHeight = 0; // in meters
    this->m_wellWidth = 0;  // in meters
    this->m_wellLength = 0; // in meters
    this->m_volume = 0;     // in liters

    // PRESSURE VARIABLES
    m_pressureValue = 0; // variable to store the value coming from the pressure transducer
    m_waterHeight = 0;   // variable to store the value of the water height in meters
    Wire.begin();        // join i2c bus (address optional for master)
}
void Sensor::loop()
{
    this->m_currentMillis = millis();
    if (this->m_currentMillis - this->m_lastIncrementation >= this->m_interval)
    {
        // Step 1: Point to Config register - set to continuous conversion
        Wire.beginTransmission(m_I2Caddress);

        // Point to Config Register
        Wire.write(0b00000001);

        // Write the MSB + LSB of Config Register
        // MSB: Bits 15:8
        // Bit  15		0=No effect, 1=Begin Single Conversion (in power down mode)
        // Bits 14:12 	How to configure A0 to A3 (comparator or single ended)
        // Bits	11:9 	Programmable Gain 000=6.144v 001=4.096v 010=2.048v .... 111=0.256v
        // Bits	8 		0=Continuous conversion mode, 1=Power down single shot
        Wire.write(0b01000010);

        // LSB:	Bits 7:0
        // Bits 7:5	Data Rate (Samples per second) 000=8, 001=16, 010=32, 011=64,
        //			100=128, 101=250, 110=475, 111=860
        // Bit 	4 	Comparator Mode 0=Traditional, 1=Window
        // Bit	3 	Comparator Polarity 0=low, 1=high
        // Bit	2 	Latching 0=No, 1=Yes
        // Bits	1:0	Comparator # before Alert pin goes high
        //			00=1, 01=2, 10=4, 11=Disable this feature
        Wire.write(0b00000010);

        // Send the above bytes as an I2C WRITE to the module
        Wire.endTransmission();

        // ====================================

        // Step 2: Set the pointer to the conversion register
        Wire.beginTransmission(m_I2Caddress);

        // Point to Conversion register (read only , where we get our results from)
        Wire.write(0b00000000);

        // Send the above byte(s) as a WRITE
        Wire.endTransmission();

        // =======================================

        // Step 3: Request the 2 converted bytes (MSB plus LSB)
        Wire.requestFrom(m_I2Caddress, 2);

        // Read two bytes and convert to full 16-bit int
        uint16_t convertedValue;

        // Read the the first byte (MSB) and shift it 8 places to the left then read
        // the second byte (LSB) into the last byte of this integer
        convertedValue = (Wire.read() << 8 | Wire.read());

        // Debug the value
        // Serial.println(convertedValue >> 6 << 6);
        // Serial.println(map(convertedValue, 0, 32767, 0, 5000));
        // Serial.println(convertedValue);
        m_readings[m_readCnt] = convertedValue;
        m_readCnt = m_readCnt == 19 ? 0 : m_readCnt + 1;

        // Get the average
        unsigned long totalReadings = 0;
        for (unsigned char cnt = 0; cnt < 20; cnt++)
        {
            totalReadings += m_readings[cnt];
        }

        m_pressureValue = convertedValue;                                                                                       // reads value from input pin and assigns to variable
        m_pressureValue = ((m_pressureValue - m_pressureZero) * m_pressuretransducermaxPSI) / (m_pressureMax - m_pressureZero); // conversion equation to convert analog reading to psi
        delay(m_sensorreadDelay);                                                                                               // delay in milliseconds between read values

        // convert psi to centimeters
        m_waterHeight = (m_pressureValue * 703.08893732448) / 1000;

        // Volume Calculation of a vertical cylinder in liters tank volume = π × r2 × l

        if (m_type == 0)
        {
            m_volume = (PI * (m_wellWidth / 2) * (m_wellWidth / 2) * m_waterHeight) * 1000; // convert to liters
        }

        if (m_type == 1)
        {
            m_volume = (m_wellLength * m_wellWidth * m_waterHeight) * 1000; // convert to liters
        }

        this->m_lastIncrementation = m_currentMillis; // Update the last time the sensor was read
    }
}
