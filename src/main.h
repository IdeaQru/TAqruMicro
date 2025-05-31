#include <Arduino.h>
#include "GY25/GY25.h"
#include "HMCL5883L/HMC5883L.h"
#include "GPS/GPS.h"
#include "DataSender/DataSender.h"

// Define pins for GPS SoftwareSerial
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define SDA_PIN 21
#define SCL_PIN 22
Gy25 imu;

HMC5883L compass;
GPS gps(GPS_RX_PIN, GPS_TX_PIN);

// Ganti dengan SSID dan password WiFi Anda
const char* ssid = "IdeaQru";
const char* password = "fiqru123";

// Ganti dengan URL endpoint backend Anda
const char* backendUrl = "http://172.20.10.12:3018/api/datasensor";

DataSender dataSender(ssid, password, backendUrl);

void setup_starting() {
    Serial.begin(115200);
    // dataSender.begin();
    if((!imu.begin(SDA_PIN, SCL_PIN)&&(!compass.begin(SDA_PIN, SCL_PIN)))) {
        Serial.println("Gagal inisialisasi sensor!");
        while(1);
    }
    Serial1.begin(115200,SERIAL_5N1, GPS_RX_PIN, GPS_TX_PIN);
    gps.begin(115200);
    Serial.println("Mulai kalibrasi - putar sensor di semua arah");
    compass.startCalibration();
    
    unsigned long start = millis();
    while(millis() - start < 15000) {  // Kalibrasi selama 30 detik
        if(compass.read()) {
            compass.addCalibrationData();
        }
        delay(100);
    }
    compass.endCalibration();
    Serial.println("Kalibrasi selesai!");
    delay(1000);
}