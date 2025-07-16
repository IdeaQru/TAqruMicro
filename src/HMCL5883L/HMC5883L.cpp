#include "HMC5883L.h"

HMC5883L::HMC5883L() {
    _x = _y = _z = 0;
    _offsetX = _offsetY = 0;
    _scaleX = _scaleY = 1.0;
    _declination = 0.0;
}

bool HMC5883L::begin(int sda, int scl) {
    Wire.begin(sda, scl);
    
    // Reset sensor
    reset();
    delay(100);
    
    // Configure sensor
    configure();
    delay(100);
    
    // Test communication
    Wire.beginTransmission(HMC5883L_ADDR);
    if (Wire.endTransmission() != 0) {
        return false;
    }
    
    Serial.println("HMC5883L initialized successfully");
    return true;
}

void HMC5883L::read() {
    Wire.beginTransmission(HMC5883L_ADDR);
    Wire.write(REG_X_LSB);
    Wire.endTransmission();
    
    Wire.requestFrom(HMC5883L_ADDR, (uint8_t)6);
    
    if (Wire.available() >= 6) {
        // Data format: X_LSB, X_MSB, Y_LSB, Y_MSB, Z_LSB, Z_MSB
        _x = Wire.read() | (Wire.read() << 8);
        _y = Wire.read() | (Wire.read() << 8);
        _z = Wire.read() | (Wire.read() << 8);
    }
}

int HMC5883L::getX() {
    return _x;
}

int HMC5883L::getY() {
    return _y;
}

int HMC5883L::getZ() {
    return _z;
}

float HMC5883L::getHeading() {
    // Apply calibration
    float calibratedX = (_x - _offsetX) * _scaleX;
    float calibratedY = (_y - _offsetY) * _scaleY;
    
    // Calculate heading
    float heading = atan2(calibratedY, calibratedX) * 180.0 / PI;
    
    // Add magnetic declination
    heading += _declination;
    
    // Normalize to 0-360 degrees
    if (heading < 0) {
        heading += 360;
    } else if (heading >= 360) {
        heading -= 360;
    }
    
    return (int)heading;
}

String HMC5883L::getDirection(float heading) {
    // int azimuth = getHeading();
    return degreesToDirection(heading);
}

void HMC5883L::calibrate(int duration) {
    Serial.println("Starting calibration...");
    Serial.println("Rotate the compass slowly in all directions");
    
    int minX = 32767, maxX = -32768;
    int minY = 32767, maxY = -32768;
    
    unsigned long startTime = millis();
    
    while ((millis() - startTime) < duration) {
        read();
        
        if (_x < minX) minX = _x;
        if (_x > maxX) maxX = _x;
        if (_y < minY) minY = _y;
        if (_y > maxY) maxY = _y;
        
        delay(50);
    }
    
    // Calculate offsets and scales
    _offsetX = (maxX + minX) / 2.0;
    _offsetY = (maxY + minY) / 2.0;
    
    float deltaX = maxX - minX;
    float deltaY = maxY - minY;
    float avgDelta = (deltaX + deltaY) / 2.0;
    
    _scaleX = avgDelta / deltaX;
    _scaleY = avgDelta / deltaY;
    
    Serial.println("Calibration complete!");
    Serial.printf("Offsets: X=%.2f, Y=%.2f\n", _offsetX, _offsetY);
    Serial.printf("Scales: X=%.4f, Y=%.4f\n", _scaleX, _scaleY);
}

void HMC5883L::setMagneticDeclination(float declination) {
    _declination = declination;
}

void HMC5883L::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(HMC5883L_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t HMC5883L::readRegister(uint8_t reg) {
    Wire.beginTransmission(HMC5883L_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    
    Wire.requestFrom(HMC5883L_ADDR, (uint8_t)1);
    if (Wire.available()) {
        return Wire.read();
    }
    return 0;
}

void HMC5883L::reset() {
    writeRegister(REG_RESET, 0x01);
}

void HMC5883L::configure() {
    // Mode: Continuous, ODR: 200Hz, Range: 8G, OSR: 512
    writeRegister(REG_CONFIG, 0x1D);
}

String HMC5883L::degreesToDirection(int degrees) {
    const String directions[] = {
        "Utara", "Utara Timur Laut", "Timur Laut", "Timur Timur Laut",
        "Timur", "Timur Tenggara", "Tenggara", "Tenggara Selatan",
        "Selatan", "Selatan Barat Daya", "Barat Daya", "Barat Barat Daya",
        "Barat", "Barat Barat Laut", "Barat Laut", "Utara Barat Laut"
    };
    
    int index = (degrees + 11) / 22; // 360/16 = 22.5 degrees per direction
    return directions[index % 16];
}
