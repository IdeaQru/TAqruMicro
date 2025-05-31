#ifndef GY25_H
#define GY25_H

#include <Wire.h>
#include <math.h>
#include <Arduino.h>
#include "PID.h"
class Gy25 {

public:
    Gy25(uint8_t address = 0x68);
    PIDAdaptive pidRoll = PIDAdaptive(1.0, 0.0, 0.0);   
    PIDAdaptive pidPitch = PIDAdaptive(1.0, 0.0, 0.0);
    PIDAdaptive pidYaw = PIDAdaptive(1.0, 0.0, 0.0);
    bool begin(int sda_pin, int scl_pin);
    void update();
    void calibrate();
    
    float getRoll() const { return angles.kalRoll; }
    float getPitch() const { return angles.kalPitch; }
    float getYaw() const { return angles.gyroYaw; }

private:
    struct SensorData {
        int16_t accX, accY, accZ;
        int16_t gyroX, gyroY, gyroZ;
        float temp;
    };
    
    struct {
        int16_t accX, accY, accZ;
        int16_t gyroX, gyroY, gyroZ;
    } offsets;
    
    struct {
        float accPitch, accRoll;
        float gyroPitch, gyroRoll, gyroYaw;
        float kalPitch, kalRoll;
        float compPitch, compRoll;
    } angles;
    
    class Kalman {
    public:
        Kalman(float Q = 0.001, float R = 0.03);
        float update(float measurement, float rate, float dt);
        
    private:
        float Q, R;
        float x0, x1;
        float P00, P01, P10, P11;
    };

    Kalman kalmanX, kalmanY;
    uint8_t i2cAddress;
    uint32_t lastUpdate;
   
    bool initMPU();
    bool readSensor(SensorData &data);
    void applyCalibration(SensorData &data);
    void updateAngles(SensorData &data, float deltaTime);
    
    static constexpr float COMPLEMENTARY_ALPHA = 0.96;
    static constexpr int CALIBRATION_SAMPLES = 2000;
    static constexpr int GYRO_THRESHOLD = 15;
};


#endif
