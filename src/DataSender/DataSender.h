#ifndef DATASENDER_H
#define DATASENDER_H

#include <Arduino.h>

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#else
  #error "Platform not supported. Use ESP32 or ESP8266."
#endif

#include <HTTPClient.h>

class DataSender {
public:
    DataSender(const char* ssid, const char* password, const char* serverUrl);
    void begin();  // Setup WiFi connection
    bool isConnected(); // Check if WiFi is connected
    void sendData(float roll, float pitch, float yaw, float heading, String cardinalDirection, String latitude, String longitude);

private:
    const char* _ssid;
    const char* _password;
    const char* _serverUrl;

    void connectWiFi();
};

#endif // DATASENDER_H
