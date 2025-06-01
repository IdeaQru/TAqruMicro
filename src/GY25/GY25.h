#ifndef GY25_H
#define GY25_H

#include <Wire.h>
#include <Arduino.h>
#include <MPU6050_light.h>

class Gy25 {
public:
    Gy25(uint8_t address = 0x68);
    bool begin(int sda_pin, int scl_pin);
    void update();
    void calibrate();
    
    float getRoll() const { return roll; }
    float getPitch() const { return pitch; }
    float getYaw() const { return yaw; }

private:
    MPU6050* mpu; // Menggunakan pointer untuk inisialisasi manual
    float roll, pitch, yaw;
    unsigned long lastUpdate;
    void calculateAngles();

    // Konstanta dan variabel untuk menghindari angka langsung di .cpp
    static constexpr uint8_t DEFAULT_I2C_ADDRESS = 0x68;
    static constexpr int DELAY_INIT_MS = 100;
    static constexpr int GYRO_RANGE_CONFIG = 0; // ±250 deg/s
    static constexpr int ACCEL_RANGE_CONFIG = 0; // ±2g
    static constexpr float COMPLEMENTARY_ALPHA = 0.98;
    static constexpr float PI_VALUE = 3.14159265359;
    static constexpr float MICRO_TO_SECONDS = 1000000.0;
};

#endif
