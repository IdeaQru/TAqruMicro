#include "GPS.h"

GPS::GPS(uint8_t rxPin, uint8_t txPin) : gpsSerial(rxPin, txPin) {}

void GPS::begin(unsigned long baud) {
    gpsSerial.begin(baud); // Memulai komunikasi serial dengan GPS
}

bool GPS::readData() {
    while (gpsSerial.available() > 0) {
        Serial.println("GPS available" + String(gpsSerial.read()));
        gps.encode(gpsSerial.read());  // Memproses data GPS yang diterima
    }

    // Jika data GPS valid dan ada update
    return gps.location.isUpdated();
}

String GPS::getLatitude() {
    // Mengambil latitude dalam format string dengan 6 digit desimal
    return String(gps.location.lat(), 6);
}

String GPS::getLongitude() {
    // Mengambil longitude dalam format string dengan 6 digit desimal
    return String(gps.location.lng(), 6);
}
