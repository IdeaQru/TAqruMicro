#include "GPS.h"

GPS::GPS(int rxPin, int txPin) : gpsSerial(Serial1), latitude(0.0), longitude(0.0) {}

void GPS::begin(long baudRate) {
    gpsSerial.begin(baudRate);
}

void GPS::update() {
    while (gpsSerial.available() > 0) {
        if (gps.encode(gpsSerial.read())) {
            if (gps.location.isValid()) {
                latitude = gps.location.lat();
                longitude = gps.location.lng();
            }
        }
    }
}
