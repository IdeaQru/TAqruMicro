#ifndef GY25_H
#define GY25_H

#include <Arduino.h>

class Gy25 {
public:
    Gy25();
    bool begin(int rx_pin, int tx_pin);
    void update();
    
    float getRoll() const { return roll; }
    float getPitch() const { return pitch; }
    float getYaw() const { return yaw; }

private:
    float roll, pitch, yaw;
    void parseSerialData(String data);

    // Konstanta
    static constexpr int BAUD_RATE = 115200;
};

#endif
