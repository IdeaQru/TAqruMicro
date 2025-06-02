#ifndef GPS_H
#define GPS_H

#include <Arduino.h>

class GPS {
public:
    GPS(int rxPin, int txPin);
    void begin(long baudRate);
    void update();
    float getLatitude() const { return latitude; }
    float getLongitude() const { return longitude; }
    bool isValid() const { return dataValid; }
    int getSatellites() const { return satellites; }
    float getSpeed() const { return speed; }
    float getAltitude() const { return altitude; }

private:
    HardwareSerial gpsSerial;
    float latitude;
    float longitude;
    float speed;
    float altitude;
    int satellites;
    bool dataValid;
    String nmeaBuffer;
    
    // NMEA parsing functions
    void parseNMEA(String sentence);
    void parseGPGGA(String sentence);
    void parseGPRMC(String sentence);
    void parseGPGSA(String sentence);
    
    // Utility functions
    float convertToDecimalDegrees(String coordinate, char direction);
    bool isValidChecksum(String sentence);
    String getValue(String data, char separator, int index);
    int hexCharToInt(char c);
};

#endif
