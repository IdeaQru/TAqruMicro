#ifndef HMC5883L_H
#define HMC5883L_H

#include <Wire.h>
#include <Arduino.h>
#include <QMC5883LCompass.h>

class HMC5883LCompass {
public:
    HMC5883LCompass();
    bool begin();
    void read();
    float getHeading() const; // Mengembalikan heading dalam derajat
    String getCardinalDirection(float heading) const; // Mengembalikan arah mata angin

private:
    ::QMC5883LCompass compass;
    float heading;
};

#endif
