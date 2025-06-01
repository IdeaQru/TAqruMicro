#include <Arduino.h>
#include "GY25/GY25.h"
#include "HMCL5883L/HMC5883L.h"
#include "GPS/GPS.h"
#include "DataSender/DataSender.h"
#include "Display/LCDDisplay.h"

// Define pins for GPS SoftwareSerial and I2C
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define GY25_RX_PIN 4
#define GY25_TX_PIN 2
#define SDA_PIN 21
#define SCL_PIN 22
#define nol 0
#define dua 2
#define enam 6
// Initialize objects
Gy25 imu;
HMC5883LCompass compass;
GPS gps(GPS_RX_PIN, GPS_TX_PIN);
LCDDisplay lcd(SDA_PIN, SCL_PIN);

// WiFi credentials and backend URL
const char *ssid = "IdeaQru";
const char *password = "fiqru123";
const char *backendUrl = "http://172.20.10.12:3018/api/datasensor";

DataSender dataSender(ssid, password, backendUrl);

// Constants for timing and delays
static constexpr unsigned long CALIBRATION_DURATION_MS = 15000;
static constexpr unsigned long LCD_UPDATE_INTERVAL_MS = 2000;
static constexpr unsigned long LOOP_DELAY_MS = 100;
static constexpr long GPS_BAUD_RATE = 9600;
static constexpr long SERIAL_BAUD_RATE = 115200;

void setupMain()
{
    Serial.begin(SERIAL_BAUD_RATE);
    Serial1.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);


    if (!imu.begin(GY25_RX_PIN, GY25_TX_PIN)) {
        Serial.println("Gagal inisialisasi komunikasi Serial2!");
        while (1); // Berhenti jika gagal
    }

    Serial.println("Mulai membaca data dari GY25...");
    // Initialize components
    if (!imu.begin(SDA_PIN, SCL_PIN))
    {
        Serial.println("Gagal inisialisasi IMU!");
        while (true)
            ;
    }

    if (!compass.begin())
    {
        Serial.println("Gagal inisialisasi kompas!");
        while (true)
            ;
    }

    if (!lcd.begin())
    {
        Serial.println("Gagal inisialisasi LCD!");
        while (true)
            ;
    }

    gps.begin(GPS_BAUD_RATE);
    // dataSender.begin(); // Uncomment if WiFi data sending is needed

    // Calibrate compass
    Serial.println("Mulai kalibrasi kompas - putar sensor di semua arah");
    lcd.displayMessage("Calibrating...", "Rotate Sensor");

    unsigned long start = millis();
    while (millis() - start < CALIBRATION_DURATION_MS)
    {
        compass.read();
        delay(LOOP_DELAY_MS);
    }
    Serial.println("Kalibrasi kompas selesai!");
    lcd.displayMessage("Calibration", "Done!");
    delay(LOOP_DELAY_MS * 10); // Delay tambahan setelah kalibrasi
}

