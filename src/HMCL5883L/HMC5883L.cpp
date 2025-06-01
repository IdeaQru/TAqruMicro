#include "HMC5883L.h"

HMC5883LCompass::HMC5883LCompass() : heading(0.0) {}

bool HMC5883LCompass::begin() {
    compass.init();
    compass.setSmoothing(10, true); // Smoothing untuk pembacaan yang lebih baik
    return true;
}

void HMC5883LCompass::read() {
    compass.read();
    heading = compass.getAzimuth();
}

float HMC5883LCompass::getHeading() const {
    return heading;
}

String HMC5883LCompass::getCardinalDirection(float heading) const {
    if (heading >= 337.5 || heading < 22.5) return "North";
    else if (heading >= 22.5 && heading < 67.5) return "Northeast";
    else if (heading >= 67.5 && heading < 112.5) return "East";
    else if (heading >= 112.5 && heading < 157.5) return "Southeast";
    else if (heading >= 157.5 && heading < 202.5) return "South";
    else if (heading >= 202.5 && heading < 247.5) return "Southwest";
    else if (heading >= 247.5 && heading < 292.5) return "West";
    else if (heading >= 292.5 && heading < 337.5) return "Northwest";
    return "Unknown";
}
