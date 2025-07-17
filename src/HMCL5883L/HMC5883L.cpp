#include "HMC5883L.h"

HMC5883L::HMC5883L() {
    _x = _y = _z = 0;
    _offsetX = _offsetY = _offsetZ = 0;
    _scaleX = _scaleY = _scaleZ = 1.0;
    _declination = 0.0;
    _pitch = _roll = _yaw = 0.0;
    _accelX = _accelY = _accelZ = 0.0;
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
    
    // Cek chip ID untuk konfirmasi
    uint8_t chipID = getChipID();
    Serial.print("Chip ID: 0x");
    Serial.println(chipID, HEX);
    
    Serial.println("HMC5883L (QMC5883L) initialized successfully");
    return true;
}

void HMC5883L::read() {
    // Tunggu sampai data siap
    // while (!isReady()) {
    //     delay(1);
    // }
    
    Wire.beginTransmission(HMC5883L_ADDR);
    Wire.write(REG_X_LSB);
    Wire.endTransmission();
    
    Wire.requestFrom(HMC5883L_ADDR, (uint8_t)6);
    
    if (Wire.available() >= 6) {
        // QMC5883L data format: X_LSB, X_MSB, Y_LSB, Y_MSB, Z_LSB, Z_MSB
        _x = Wire.read() | (Wire.read() << 8);
        _y = Wire.read() | (Wire.read() << 8);
        _z = Wire.read() | (Wire.read() << 8);
        
        // Konversi ke signed 16-bit
        if (_x > 32767) _x -= 65536;
        if (_y > 32767) _y -= 65536;
        if (_z > 32767) _z -= 65536;
    }
    
    // Hitung sudut setelah membaca data
    calculateAngles();
}

void HMC5883L::calculateAngles() {
    // Apply calibration untuk magnetometer
    float calibratedX = (_x - _offsetX) * _scaleX;
    float calibratedY = (_y - _offsetY) * _scaleY;
    float calibratedZ = (_z - _offsetZ) * _scaleZ;
    
    // Jika ada data accelerometer, gunakan untuk pitch dan roll
    if (_accelX != 0 || _accelY != 0 || _accelZ != 0) {
        // Hitung pitch dan roll dari accelerometer
        _pitch = atan2(_accelY, sqrt(_accelX * _accelX + _accelZ * _accelZ)) * 180.0 / PI;
        _roll = atan2(-_accelX, _accelZ) * 180.0 / PI;
        
        // Hitung tilt-compensated heading
        _yaw = calculateTiltCompensatedHeading();
    } else {
        // Jika tidak ada accelerometer, gunakan estimasi sederhana
        _pitch = 0.0;
        _roll = 0.0;
        
        // Hitung heading biasa
        _yaw = atan2(calibratedY, calibratedX) * 180.0 / PI;
        
        // Tambahkan magnetic declination
        _yaw += _declination;
        
        // Normalize ke 0-360 degrees
        if (_yaw < 0) {
            _yaw += 360;
        } else if (_yaw >= 360) {
            _yaw -= 360;
        }
    }
}

float HMC5883L::calculateTiltCompensatedHeading() {
    // Konversi ke radian
    float pitchRad = _pitch * PI / 180.0;
    float rollRad = _roll * PI / 180.0;
    
    // Apply calibration
    float magX = (_x - _offsetX) * _scaleX;
    float magY = (_y - _offsetY) * _scaleY;
    float magZ = (_z - _offsetZ) * _scaleZ;
    
    // Tilt compensation
    float magXComp = magX * cos(pitchRad) + magZ * sin(pitchRad);
    float magYComp = magX * sin(rollRad) * sin(pitchRad) + 
                     magY * cos(rollRad) - 
                     magZ * sin(rollRad) * cos(pitchRad);
    
    // Hitung heading
    float heading = atan2(magYComp, magXComp) * 180.0 / PI;
    
    // Tambahkan magnetic declination
    heading += _declination;
    
    // Normalize ke 0-360 degrees
    if (heading < 0) {
        heading += 360;
    } else if (heading >= 360) {
        heading -= 360;
    }
    
    return heading;
}

// Getter functions
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
    return _yaw;
}

float HMC5883L::getPitch() {
    return _pitch;
}

float HMC5883L::getRoll() {
    return _roll;
}

float HMC5883L::getYaw() {
    return _yaw;
}

String HMC5883L::getDirection(float heading) {
    return degreesToDirection((int)heading);
}

void HMC5883L::setAccelData(float x, float y, float z) {
    _accelX = x;
    _accelY = y;
    _accelZ = z;
}

bool HMC5883L::isReady() {
    return (readRegister(REG_STATUS) & 0x01) != 0;
}

uint8_t HMC5883L::getChipID() {
    return readRegister(REG_CHIP_ID);
}

void HMC5883L::calibrate(int duration) {
    Serial.println("Starting calibration...");
    Serial.println("Rotate the compass slowly in all directions");
    
    int minX = 32767, maxX = -32768;
    int minY = 32767, maxY = -32768;
    int minZ = 32767, maxZ = -32768;
    
    unsigned long startTime = millis();
    
    while ((millis() - startTime) < duration) {
        read();
        
        if (_x < minX) minX = _x;
        if (_x > maxX) maxX = _x;
        if (_y < minY) minY = _y;
        if (_y > maxY) maxY = _y;
        if (_z < minZ) minZ = _z;
        if (_z > maxZ) maxZ = _z;
        
        delay(50);
    }
    
    // Calculate offsets and scales
    _offsetX = (maxX + minX) / 2.0;
    _offsetY = (maxY + minY) / 2.0;
    _offsetZ = (maxZ + minZ) / 2.0;
    
    float deltaX = maxX - minX;
    float deltaY = maxY - minY;
    float deltaZ = maxZ - minZ;
    float avgDelta = (deltaX + deltaY + deltaZ) / 3.0;
    
    if (deltaX > 0) _scaleX = avgDelta / deltaX;
    if (deltaY > 0) _scaleY = avgDelta / deltaY;
    if (deltaZ > 0) _scaleZ = avgDelta / deltaZ;
    
    Serial.println("Calibration complete!");
    Serial.printf("Offsets: X=%.2f, Y=%.2f, Z=%.2f\n", _offsetX, _offsetY, _offsetZ);
    Serial.printf("Scales: X=%.4f, Y=%.4f, Z=%.4f\n", _scaleX, _scaleY, _scaleZ);
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
    // Reset sensor terlebih dahulu
    reset();
    delay(100);
    
    // Configure QMC5883L
    // Mode: Continuous, ODR: 200Hz, Range: 8G, OSR: 512
    writeRegister(REG_CONFIG, 0x1D);
    
    // Set periode
    writeRegister(REG_PERIOD, 0x01);
}

String HMC5883L::degreesToDirection(int degrees) {
    const String directions[] = {
        "Utara",    // 0°
        "Timur",    // 90°
        "Selatan",  // 180°
        "Barat"     // 270°
    };
    
    // Normalisasi derajat ke rentang 0-359
    degrees = ((degrees % 360) + 360) % 360;
    
    // Setiap arah memiliki rentang 90 derajat
    // Utara: 315-44°, Timur: 45-134°, Selatan: 135-224°, Barat: 225-314°
    int index;
    if (degrees >= 315 || degrees < 45) {
        index = 0; // Utara
    } else if (degrees >= 45 && degrees < 135) {
        index = 1; // Timur
    } else if (degrees >= 135 && degrees < 225) {
        index = 2; // Selatan
    } else {
        index = 3; // Barat
    }
    
    return directions[index];
}

