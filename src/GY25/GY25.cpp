#include "GY25.h"

Gy25::Gy25(uint8_t address) {
    i2cAddress = address;
    addressAutoDetected = false;
    roll = pitch = yaw = 0.0;
    accelX = accelY = accelZ = 0;
    gyroX = gyroY = gyroZ = 0;
    temperature = 0;
    gyroXOffset = gyroYOffset = gyroZOffset = 0.0;
    accelXOffset = accelYOffset = accelZOffset = 0.0;
    gyroScale = 131.0; // Default for ±250°/s
    accelScale = 16384.0; // Default for ±2g
    lastUpdate = 0;
}

bool Gy25::begin(int sda_pin, int scl_pin) {
    Wire.begin(sda_pin, scl_pin);
    
    // Auto-detect address jika alamat default tidak berfungsi
    uint8_t detectedAddress = autoDetectAddress();
    if (detectedAddress != 0) {
        if (detectedAddress != i2cAddress) {
            Serial.printf("Auto-detected address: 0x%02X (was 0x%02X)\n", detectedAddress, i2cAddress);
            i2cAddress = detectedAddress;
            addressAutoDetected = true;
        }
    } else {
        Serial.println("No MPU6050 device found! Scanning I2C bus...");
        scanI2CDevices();
        return false;
    }
    
    // Check if device is present dengan address yang sudah terdeteksi
    uint8_t whoAmI = readRegister(WHO_AM_I);
    if (whoAmI != 0x68 && whoAmI != 0x72) {
        Serial.printf("Unexpected WHO_AM_I: 0x%02X (expected 0x68 or 0x72)\n", whoAmI);
        return false;
    }
    
    Serial.printf("MPU6050 found at address 0x%02X, WHO_AM_I: 0x%02X\n", i2cAddress, whoAmI);
    
    // Wake up the MPU6050
    writeRegister(PWR_MGMT_1, 0x00);
    delay(100);
    
    // Set sample rate to 1kHz
    writeRegister(SMPLRT_DIV, 0x07);
    
    // Set digital low pass filter
    writeRegister(CONFIG, 0x06);
    
    // Set gyro range to ±250°/s
    setGyroRange(0);
    
    // Set accelerometer range to ±2g
    setAccelRange(0);
    
    // Disable sleep mode
    writeRegister(PWR_MGMT_1, 0x01);
    
    delay(100);
    
    // Calibrate sensor
    calibrate();
    
    lastUpdate = micros();
    
    return true;
}

uint8_t Gy25::autoDetectAddress() {
    Serial.println("Auto-detecting MPU6050 address...");
    
    // Coba address yang diberikan dulu
    if (testAddress(i2cAddress)) {
        Serial.printf("Address 0x%02X confirmed\n", i2cAddress);
        return i2cAddress;
    }
    
    // Jika tidak berhasil, coba address-address yang umum
    uint8_t possibleAddresses[] = {MPU6050_ADDR_LOW, MPU6050_ADDR_HIGH, 0x6A, 0x6B};
    
    for (int i = 0; i < 4; i++) {
        if (possibleAddresses[i] != i2cAddress) { // Skip address yang sudah dicoba
            if (testAddress(possibleAddresses[i])) {
                Serial.printf("Found MPU6050 at address 0x%02X\n", possibleAddresses[i]);
                return possibleAddresses[i];
            }
        }
    }
    
    Serial.println("MPU6050 not found at common addresses");
    return 0; // Tidak ditemukan
}

bool Gy25::testAddress(uint8_t address) {
    // Test basic communication
    Wire.beginTransmission(address);
    if (Wire.endTransmission() != 0) {
        return false;
    }
    
    // Test WHO_AM_I register
    Wire.beginTransmission(address);
    Wire.write(WHO_AM_I);
    if (Wire.endTransmission() != 0) {
        return false;
    }
    
    Wire.requestFrom(address, (uint8_t)1);
    if (Wire.available()) {
        uint8_t whoAmI = Wire.read();
        // Check for valid WHO_AM_I values
        if (whoAmI == 0x68 || whoAmI == 0x72) {
            return true;
        }
    }
    
    return false;
}

void Gy25::scanI2CDevices() {
    Serial.println("Scanning I2C bus for devices...");
    
    int deviceCount = 0;
    for (byte address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) Serial.print("0");
            Serial.println(address, HEX);
            deviceCount++;
        }
    }
    
    if (deviceCount == 0) {
        Serial.println("No I2C devices found");
    } else {
        Serial.printf("Found %d I2C device(s)\n", deviceCount);
    }
}

// Fungsi-fungsi lainnya tetap sama seperti sebelumnya
void Gy25::setGyroRange(uint8_t range) {
    writeRegister(GYRO_CONFIG, range << 3);
    
    switch(range) {
        case 0: gyroScale = 131.0; break;   // ±250°/s
        case 1: gyroScale = 65.5; break;    // ±500°/s
        case 2: gyroScale = 32.8; break;    // ±1000°/s
        case 3: gyroScale = 16.4; break;    // ±2000°/s
        default: gyroScale = 131.0; break;
    }
}

void Gy25::setAccelRange(uint8_t range) {
    writeRegister(ACCEL_CONFIG, range << 3);
    
    switch(range) {
        case 0: accelScale = 16384.0; break; // ±2g
        case 1: accelScale = 8192.0; break;  // ±4g
        case 2: accelScale = 4096.0; break;  // ±8g
        case 3: accelScale = 2048.0; break;  // ±16g
        default: accelScale = 16384.0; break;
    }
}

void Gy25::calibrate() {
    Serial.println("Calibrating GY25... Keep sensor still!");
    
    float gyroXSum = 0, gyroYSum = 0, gyroZSum = 0;
    float accelXSum = 0, accelYSum = 0, accelZSum = 0;
    
    for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
        readRawData();
        
        gyroXSum += gyroX;
        gyroYSum += gyroY;
        gyroZSum += gyroZ;
        
        accelXSum += accelX;
        accelYSum += accelY;
        accelZSum += accelZ;
        
        delay(3);
    }
    
    gyroXOffset = gyroXSum / CALIBRATION_SAMPLES;
    gyroYOffset = gyroYSum / CALIBRATION_SAMPLES;
    gyroZOffset = gyroZSum / CALIBRATION_SAMPLES;
    
    accelXOffset = accelXSum / CALIBRATION_SAMPLES;
    accelYOffset = accelYSum / CALIBRATION_SAMPLES;
    accelZOffset = (accelZSum / CALIBRATION_SAMPLES) - accelScale; // Gravity compensation
    
    Serial.println("Calibration complete!");
    Serial.printf("Gyro offsets: X=%.2f, Y=%.2f, Z=%.2f\n", gyroXOffset, gyroYOffset, gyroZOffset);
    Serial.printf("Accel offsets: X=%.2f, Y=%.2f, Z=%.2f\n", accelXOffset, accelYOffset, accelZOffset);
}

void Gy25::update() {
    if (isDataReady()) {
        readRawData();
        calculateAngles();
    }
}

bool Gy25::isDataReady() {
    uint8_t status = readRegister(INT_STATUS);
    return (status & 0x01); // Check data ready bit
}

void Gy25::readRawData() {
    Wire.beginTransmission(i2cAddress);
    Wire.write(ACCEL_XOUT_H);
    Wire.endTransmission(false);
    Wire.requestFrom(i2cAddress, (uint8_t)14, true);
    
    accelX = (Wire.read() << 8) | Wire.read();
    accelY = (Wire.read() << 8) | Wire.read();
    accelZ = (Wire.read() << 8) | Wire.read();
    temperature = (Wire.read() << 8) | Wire.read();
    gyroX = (Wire.read() << 8) | Wire.read();
    gyroY = (Wire.read() << 8) | Wire.read();
    gyroZ = (Wire.read() << 8) | Wire.read();
}

void Gy25::calculateAngles() {
    unsigned long currentTime = micros();
    float dt = (currentTime - lastUpdate) / 1000000.0;
    lastUpdate = currentTime;
    
    if (dt > 0.1) dt = 0.01; // Prevent large dt values
    
    complementaryFilter(dt);
}

float Gy25::getAccelAngleX() {
    float accelXCal = (accelX - accelXOffset) / accelScale;
    float accelYCal = (accelY - accelYOffset) / accelScale;
    float accelZCal = (accelZ - accelZOffset) / accelScale;
    
    return atan2(accelYCal, sqrt(accelXCal * accelXCal + accelZCal * accelZCal)) * RAD_TO_DEG;
}

float Gy25::getAccelAngleY() {
    float accelXCal = (accelX - accelXOffset) / accelScale;
    float accelYCal = (accelY - accelYOffset) / accelScale;
    float accelZCal = (accelZ - accelZOffset) / accelScale;
    
    return atan2(-accelXCal, sqrt(accelYCal * accelYCal + accelZCal * accelZCal)) * RAD_TO_DEG;
}

void Gy25::updateGyroAngles(float dt) {
    float gyroXRate = (gyroX - gyroXOffset) / gyroScale;
    float gyroYRate = (gyroY - gyroYOffset) / gyroScale;
    float gyroZRate = (gyroZ - gyroZOffset) / gyroScale;
    
    roll += gyroXRate * dt;
    pitch += gyroYRate * dt;
    yaw += gyroZRate * dt;
}

void Gy25::complementaryFilter(float dt) {
    // Get accelerometer angles
    float accelAngleX = getAccelAngleX();
    float accelAngleY = getAccelAngleY();
    
    // Update gyro angles
    updateGyroAngles(dt);
    
    // Apply complementary filter
    roll = COMPLEMENTARY_ALPHA * roll + (1.0 - COMPLEMENTARY_ALPHA) * accelAngleX;
    pitch = COMPLEMENTARY_ALPHA * pitch + (1.0 - COMPLEMENTARY_ALPHA) * accelAngleY;
    
    // Yaw is only from gyroscope (no magnetometer correction in this implementation)
    // For better yaw accuracy, you would need magnetometer data
}

void Gy25::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t Gy25::readRegister(uint8_t reg) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(i2cAddress, (uint8_t)1, true);
    
    if (Wire.available()) {
        return Wire.read();
    }
    return 0;
}
