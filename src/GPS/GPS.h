#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <TinyGPS++.h>

class GPS {
public:
    GPS(int rxPin, int txPin);
    void begin(long baudRate);
    void update();
    float getLatitude() const { return latitude; }
    float getLongitude() const { return longitude; }

private:
    TinyGPSPlus gps;
    HardwareSerial gpsSerial;
    float latitude;
    float longitude;
};

#endif
