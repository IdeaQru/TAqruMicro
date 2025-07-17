#include <Arduino.h>
#include "GY25/GY25.h"
#include "HMCL5883L/HMC5883L.h"
#include "GPS/GPS.h"
#include "DataSender/DataSender.h"
#include "Display/LCDDisplay.h"

// Define pins for GPS SoftwareSerial and I2C
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define SDA_PIN 21
#define SCL_PIN 22
#define nol 0
#define dua 2
#define enam 6

// Define pins for buzzer and relay
#define BUZZER_PIN 26
#define RELAY_PIN 27

// Initialize objects
Gy25 imu;
HMC5883L compass;
GPS gps(GPS_RX_PIN, GPS_TX_PIN);
LCDDisplay lcd(SDA_PIN, SCL_PIN);

// WiFi credentials and backend URL
const char *ssid = "Lab Robotika AI";
const char *password = "labrobotm101";
const char *backendUrl = "http://192.168.0.152:3718/api/datasensor";

DataSender dataSender(ssid, password, backendUrl);

// Constants for timing and delays
static constexpr unsigned long CALIBRATION_DURATION_MS = 15000;
static constexpr unsigned long LCD_UPDATE_INTERVAL_MS = 2000;
static constexpr unsigned long LOOP_DELAY_MS = 100;
static constexpr long GPS_BAUD_RATE = 9600;
static constexpr long SERIAL_BAUD_RATE = 115200;

// Constants for trigger timing in milliseconds
static constexpr unsigned long ONE_HOUR = 3600;
static constexpr unsigned long THIRTY_MINUTES = 1800;
static constexpr unsigned long FIFTEEN_MINUTES = 900;

// Variables to store the next trigger time for each interval
unsigned long next_trigger_1h = 0;
unsigned long next_trigger_30m = 0;
unsigned long next_trigger_15m = 0;

void triggerBuzzerAndRelay() {
    // Activate buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1000);
    digitalWrite(BUZZER_PIN, LOW);
    
    // Activate relay
    digitalWrite(RELAY_PIN, HIGH);
    delay(1000);
    digitalWrite(RELAY_PIN, LOW);
    
    Serial.println("Buzzer and Relay triggered!");
}
void triggerErrorAlert()
{
    static unsigned long lastTrigger = 0;
    unsigned long currentTime = millis();
    
    // Prevent rapid triggering (minimum 5 seconds between triggers)
    if (currentTime - lastTrigger > 5000)
    {
        // Activate buzzer with error pattern (3 short beeps)
        for (int i = 0; i < 3; i++)
        {
            digitalWrite(BUZZER_PIN, HIGH);
            delay(200);
            digitalWrite(BUZZER_PIN, LOW);
            delay(200);
        }
        
        // Activate relay for 2 seconds
        digitalWrite(RELAY_PIN, HIGH);
        delay(2000);
        digitalWrite(RELAY_PIN, LOW);
        
        Serial.println("ERROR ALERT: Buzzer and Relay triggered due to invalid sensor data!");
        lastTrigger = currentTime;
    }
}

void checkRandomTriggers() {
    unsigned long currentMillis = millis();
    
    // Check 1 hour trigger
    if (currentMillis >= next_trigger_1h) {
        if (random(2) == 1) {
            Serial.println("1 Hour trigger activated");
            triggerBuzzerAndRelay();
        }
        next_trigger_1h = currentMillis + ONE_HOUR;
    }
    
    // Check 30 minute trigger
    if (currentMillis >= next_trigger_30m) {
        if (random(2) == 1) {
            Serial.println("30 Minute trigger activated");
            triggerBuzzerAndRelay();
        }
        next_trigger_30m = currentMillis + THIRTY_MINUTES;
    }
    
    // Check 15 minute trigger
    if (currentMillis >= next_trigger_15m) {
        if (random(2) == 1) {
            Serial.println("15 Minute trigger activated");
            triggerBuzzerAndRelay();
        }
        next_trigger_15m = currentMillis + FIFTEEN_MINUTES;
    }
}

void setupMain()
{
    Serial.begin(SERIAL_BAUD_RATE);
    Serial1.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

    // Initialize buzzer and relay pins
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);
    
    // Initialize trigger timers
    unsigned long currentTime = millis();
    next_trigger_1h = currentTime + ONE_HOUR;
    next_trigger_30m = currentTime + THIRTY_MINUTES;
    next_trigger_15m = currentTime + FIFTEEN_MINUTES;
    
    // Initialize random seed
    randomSeed(analogRead(0));
    
    // Initialize components
    // if (!imu.begin(SDA_PIN, SCL_PIN))
    // {
    //     Serial.println("Gagal inisialisasi IMU!");
    //     while (true)
    //         ;
    // }

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

    // gps.begin(GPS_BAUD_RATE);
    dataSender.begin(); // Uncomment if WiFi data sending is needed

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

// Tambahkan fungsi ini ke dalam loop utama Anda

