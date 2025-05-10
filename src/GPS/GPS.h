#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class GPS {
public:
    GPS(uint8_t rxPin, uint8_t txPin);
    void begin(unsigned long baud = 9600);
    bool readData();
    String getLatitude() const;
    String getLongitude() const;

private:
    SoftwareSerial gpsSerial;
    String latitude;
    String longitude;
    void parseNMEA(String &nmea);
};

#endif
