#include "HMC5883L.h"
#include <math.h>

HMC5883L::HMC5883L() : 
    rawX(0), rawY(0), rawZ(0),
    x(0), y(0), z(0),
    heading(0),
    currentGain(GAIN_1_3),
    calibrating(false),
    scaleFactor(0) {
    
    // Initialize calibration data
    cal.minX = cal.minY = cal.minZ = 32767;
    cal.maxX = cal.maxY = cal.maxZ = -32768;
}

bool HMC5883L::begin(uint8_t sda_pin, uint8_t scl_pin, Gain gain) {
    Wire.begin(sda_pin, scl_pin);
    Wire.setClock(400000);
    
    // Verify connection
    Wire.beginTransmission(0x0D); // Alamat I2C HMC5883L yang Anda gunakan
    if(Wire.endTransmission() != 0) return false;
    
    // Set initial configuration
    setGain(gain);
    
    // Set measurement mode
    Wire.beginTransmission(0x0D); // Alamat I2C HMC5883L yang Anda gunakan
    Wire.write(0x02); // Mode register
    Wire.write(0x00); // Continuous measurement mode
    return Wire.endTransmission() == 0;
}

void HMC5883L::setGain(Gain gain) {
    currentGain = gain;
    
    // Set gain and calculate scale factor
    Wire.beginTransmission(0x0D); // Alamat I2C HMC5883L yang Anda gunakan
    Wire.write(0x01); // Configuration register B
    Wire.write(gain); // Set the gain
    Wire.endTransmission();
    
    // Set scale factors (in mG/LSB)
    switch(gain) {
        case GAIN_0_88: scaleFactor = 0.73; break;
        case GAIN_1_3:  scaleFactor = 0.92; break;
        case GAIN_1_9:  scaleFactor = 1.22; break;
        case GAIN_2_5:  scaleFactor = 1.52; break;
        case GAIN_4_0:  scaleFactor = 2.27; break;
        case GAIN_4_7:  scaleFactor = 2.56; break;
        case GAIN_5_6:  scaleFactor = 3.03; break;
        case GAIN_8_1:  scaleFactor = 4.35; break;
    }
}
String HMC5883L::getCardinalDirection(float heading) {
    // Normalisasi heading ke rentang 0-360 derajat
    heading = fmod(heading, 360.0);
    if(heading < 0) heading += 360.0;
    
    // Tentukan sektor 45 derajat dengan offset 22.5 derajat
    int sector = static_cast<int>((heading + 22.5) / 45.0) % 8;
    
    // Mapping ke arah mata angin
    const String directions[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
    return directions[sector];
}
bool HMC5883L::read() {
    Wire.beginTransmission(0x0D); // Alamat I2C HMC5883L yang Anda gunakan
    Wire.write(0x03); // Start at data register (XMSB)
    if(Wire.endTransmission() != 0) return false;
    
    if(Wire.requestFrom(0x0D, 6) != 6) { // Alamat yang digunakan 0x0D
        return false;
    }
    
    // Read data (order: X, Z, Y)
    rawX = Wire.read() << 8 | Wire.read();
    rawZ = Wire.read() << 8 | Wire.read();
    rawY = Wire.read() << 8 | Wire.read();
    
    if(calibrating) return true;
    
    applyCalibration();
    
    // Convert to microtesla (1 Gauss = 100 microtesla)
    x = (rawX * scaleFactor) / 10.0;
    y = (rawY * scaleFactor) / 10.0;
    z = (rawZ * scaleFactor) / 10.0;
    
    // Calculate heading
    heading = atan2(y, x);
    heading = normalizeAngle(heading * 180.0 / M_PI);
    
    return true;
}

void HMC5883L::startCalibration() {
    calibrating = true;
    cal.minX = cal.minY = cal.minZ = 32767;
    cal.maxX = cal.maxY = cal.maxZ = -32768;
}

void HMC5883L::addCalibrationData() {
    if(!calibrating) return;
    
    cal.minX = min(cal.minX, rawX);
    cal.maxX = max(cal.maxX, rawX);
    cal.minY = min(cal.minY, rawY);
    cal.maxY = max(cal.maxY, rawY);
    cal.minZ = min(cal.minZ, rawZ);
    cal.maxZ = max(cal.maxZ, rawZ);
}

void HMC5883L::endCalibration() {
    calibrating = false;
}

void HMC5883L::applyCalibration() {
    // Hard iron offset
    int16_t offsetX = (cal.maxX + cal.minX) / 2;
    int16_t offsetY = (cal.maxY + cal.minY) / 2;
    int16_t offsetZ = (cal.maxZ + cal.minZ) / 2;
    
    // Soft iron scale
    float avg_deltaX = (cal.maxX - cal.minX) / 2.0;
    float avg_deltaY = (cal.maxY - cal.minY) / 2.0;
    float avg_delta = (avg_deltaX + avg_deltaY) / 2.0;
    
    float scaleX = avg_delta / avg_deltaX;
    float scaleY = avg_delta / avg_deltaY;
    
    // Apply calibration
    rawX = (rawX - offsetX) * scaleX;
    rawY = (rawY - offsetY) * scaleY;
    rawZ = rawZ - offsetZ;
}

float HMC5883L::getHeading() const {
    return heading;
}

float HMC5883L::getX() const {
    return x;
}

float HMC5883L::getY() const {
    return y;
}

float HMC5883L::getZ() const {
    return z;
}

float HMC5883L::normalizeAngle(float angle) {
    if(angle < 0) angle += 360.0;
    if(angle >= 360.0) angle -= 360.0;
    return angle;
}
