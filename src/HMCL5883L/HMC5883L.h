#ifndef HMC5883L_H
#define HMC5883L_H

#include <Wire.h>
#include <Arduino.h>

class HMC5883L {
public:
    enum Gain {
        GAIN_0_88 = 0x00,  // ±0.88 Ga
        GAIN_1_3  = 0x20,  // ±1.3 Ga (default)
        GAIN_1_9  = 0x40,
        GAIN_2_5  = 0x60,
        GAIN_4_0  = 0x80,
        GAIN_4_7  = 0xA0,
        GAIN_5_6  = 0xC0,
        GAIN_8_1  = 0xE0
    };

    HMC5883L();
    
    bool begin(uint8_t sda_pin, uint8_t scl_pin, Gain gain = GAIN_1_3);
    bool read();
    void setGain(Gain gain);
    void startCalibration();
    void addCalibrationData();
    void endCalibration();
    
    float getHeading() const;
    float getX() const;
    float getY() const;
    float getZ() const;
    String getCardinalDirection(float heading);

private:
    struct Calibration {
        int16_t minX, maxX;
        int16_t minY, maxY;
        int16_t minZ, maxZ;
    };
    
    void applyCalibration();
    float normalizeAngle(float angle);
    
    int16_t rawX, rawY, rawZ;
    float x, y, z;
    float heading;
    Gain currentGain;
    Calibration cal;
    bool calibrating;
    float scaleFactor;
};

#endif
