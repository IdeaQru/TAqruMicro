#ifndef GPS_H
#define GPS_H

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

class GPS {
public:
    // Konstruktor untuk menginisialisasi pin RX dan TX
    GPS(uint8_t rxPin, uint8_t txPin);

    // Memulai komunikasi GPS dengan baud rate
    void begin(unsigned long baud);

    // Membaca data GPS
    bool readData();

    // Mendapatkan latitude dan longitude
    String getLatitude();
    String getLongitude();

private:
    TinyGPSPlus gps;          // Objek dari TinyGPS++ untuk parsing data GPS
    SoftwareSerial gpsSerial; // Objek SoftwareSerial untuk komunikasi GPS
};

#endif // GPS_H
