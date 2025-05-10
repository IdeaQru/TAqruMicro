#include "GPS.h"

GPS::GPS(uint8_t rxPin, uint8_t txPin) : gpsSerial(rxPin, txPin), latitude(""), longitude("") {}

void GPS::begin(unsigned long baud) {
    gpsSerial.begin(baud);
}

bool GPS::readData() {
    if (gpsSerial.available()) {
        String nmea = gpsSerial.readStringUntil('\n');
        if (nmea.startsWith("$GPRMC")) {
            parseNMEA(nmea);
            return true;
        }
    }
    return false;
}

void GPS::parseNMEA(String &nmea) {
    // Parsing latitude and longitude from $GPRMC sentence (simplified)
    // Format: $GPRMC,time,status,lat,N/S,lon,E/W,...
    int idx1 = nmea.indexOf(',');
    int idx2 = nmea.indexOf(',', idx1 + 1);
    int idx3 = nmea.indexOf(',', idx2 + 1);
    int idx4 = nmea.indexOf(',', idx3 + 1);
    int idx5 = nmea.indexOf(',', idx4 + 1);
    int idx6 = nmea.indexOf(',', idx5 + 1);

    String status = nmea.substring(idx2 + 1, idx3);
    if (status == "A") { // Data valid
        latitude = nmea.substring(idx3 + 1, idx4) + " " + nmea.substring(idx4 + 1, idx5);
        longitude = nmea.substring(idx5 + 1, idx6) + " " + nmea.substring(idx6 + 1, nmea.indexOf(',', idx6 + 1));
    } else {
        latitude = "";
        longitude = "";
    }
}

String GPS::getLatitude() const { return latitude; }
String GPS::getLongitude() const { return longitude; }
