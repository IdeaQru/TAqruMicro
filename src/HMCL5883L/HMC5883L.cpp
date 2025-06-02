// HMC5883L.cpp
#include "HMC5883L.h"

HMC5883LCompass::HMC5883LCompass() {
    rawX = rawY = rawZ = 0;
    offsetX = offsetY = offsetZ = 0;
    scaleX = scaleY = scaleZ = 1.0;
    heading = 0.0;
    cardinalString = "Unknown";
    declination = 0.0; // Set your local magnetic declination here
}

bool HMC5883LCompass::begin() {
    Wire.begin();
    
    // Check if device is present
    uint8_t idA = readRegister(ID_REG_A);
    uint8_t idB = readRegister(ID_REG_B);
    uint8_t idC = readRegister(ID_REG_C);
    
    // HMC5883L should return 'H', '4', '3'
    if (idA != 0x48 || idB != 0x34 || idC != 0x33) {
        return false;
    }
    
    // Configure the sensor
    // Configuration Register A: 8-average, 15 Hz default, normal measurement
    writeRegister(CONFIG_REG_A, 0x70);
    
    // Configuration Register B: Gain = 5 (default)
    writeRegister(CONFIG_REG_B, 0xA0);
    
    // Mode Register: Continuous measurement mode
    writeRegister(MODE_REG, 0x00);
    
    delay(100); // Wait for sensor to stabilize
    
    return true;
}

void HMC5883LCompass::read() {
    if (isDataReady()) {
        readRawData();
        applyCalibration();
        
        // Calculate heading using calibrated X and Y values
        float calibratedX = (rawX - offsetX) * scaleX;
        float calibratedY = (rawY - offsetY) * scaleY;
        
        heading = calculateHeading(calibratedX, calibratedY);
        updateCardinalString(); // Update cardinal string setiap kali read
    }
}

float HMC5883LCompass::getHeading() const {
    return heading;
}

String HMC5883LCompass::getCardinalDirection(float heading) const {
    // 16-point compass rose dengan nama lengkap dalam bahasa Indonesia
    if (heading >= 348.75 || heading < 11.25) return "Utara";
    else if (heading >= 11.25 && heading < 33.75) return "Utara";
    else if (heading >= 33.75 && heading < 56.25) return "Timur Laut";
    else if (heading >= 56.25 && heading < 78.75) return "Timur Laut";
    else if (heading >= 78.75 && heading < 101.25) return "Timur";
    else if (heading >= 101.25 && heading < 123.75) return "Timur";
    else if (heading >= 123.75 && heading < 146.25) return "Tenggara";
    else if (heading >= 146.25 && heading < 168.75) return "Tenggara-Selatan";
    else if (heading >= 168.75 && heading < 191.25) return "Selatan";
    else if (heading >= 191.25 && heading < 213.75) return "Selatan";
    else if (heading >= 213.75 && heading < 236.25) return "Barat Daya";
    else if (heading >= 236.25 && heading < 258.75) return "Barat Daya";
    else if (heading >= 258.75 && heading < 281.25) return "Barat";
    else if (heading >= 281.25 && heading < 303.75) return "Barat";
    else if (heading >= 303.75 && heading < 326.25) return "Barat Laut";
    else if (heading >= 326.25 && heading < 348.75) return "Utara Laut";
    else return "Tidak Diketahui";
}


String HMC5883LCompass::getCardinalString() const {
    return cardinalString;
}

void HMC5883LCompass::updateCardinalString() {
    cardinalString = getCardinalDirection(heading);
}

void HMC5883LCompass::calibrate(int samples) {
    Serial.println("Starting compass calibration...");
    Serial.println("Rotate the compass in all directions for 30 seconds");
    
    float minX = 32767, maxX = -32768;
    float minY = 32767, maxY = -32768;
    float minZ = 32767, maxZ = -32768;
    
    unsigned long startTime = millis();
    int count = 0;
    
    while (count < samples && (millis() - startTime) < 30000) {
        if (isDataReady()) {
            readRawData();
            
            if (rawX < minX) minX = rawX;
            if (rawX > maxX) maxX = rawX;
            if (rawY < minY) minY = rawY;
            if (rawY > maxY) maxY = rawY;
            if (rawZ < minZ) minZ = rawZ;
            if (rawZ > maxZ) maxZ = rawZ;
            
            count++;
            delay(50);
        }
    }
    
    // Calculate offsets and scales
    offsetX = (maxX + minX) / 2.0;
    offsetY = (maxY + minY) / 2.0;
    offsetZ = (maxZ + minZ) / 2.0;
    
    scaleX = 2.0 / (maxX - minX);
    scaleY = 2.0 / (maxY - minY);
    scaleZ = 2.0 / (maxZ - minZ);
    
    Serial.println("Calibration complete!");
    Serial.printf("Offsets: X=%.2f, Y=%.2f, Z=%.2f\n", offsetX, offsetY, offsetZ);
    Serial.printf("Scales: X=%.4f, Y=%.4f, Z=%.4f\n", scaleX, scaleY, scaleZ);
}

bool HMC5883LCompass::isDataReady() {
    uint8_t status = readRegister(STATUS_REG);
    return (status & 0x01); // Check RDY bit
}

void HMC5883LCompass::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(HMC5883L_ADDRESS);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t HMC5883LCompass::readRegister(uint8_t reg) {
    Wire.beginTransmission(HMC5883L_ADDRESS);
    Wire.write(reg);
    Wire.endTransmission();
    
    Wire.requestFrom(HMC5883L_ADDRESS, (uint8_t)1);
    if (Wire.available()) {
        return Wire.read();
    }
    return 0;
}

void HMC5883LCompass::readRawData() {
    Wire.beginTransmission(HMC5883L_ADDRESS);
    Wire.write(DATA_OUTPUT_X_MSB);
    Wire.endTransmission();
    
    Wire.requestFrom(HMC5883L_ADDRESS, (uint8_t)6);
    
    if (Wire.available() >= 6) {
        // HMC5883L data order: X, Z, Y
        rawX = (Wire.read() << 8) | Wire.read();
        rawZ = (Wire.read() << 8) | Wire.read();
        rawY = (Wire.read() << 8) | Wire.read();
    }
}

float HMC5883LCompass::calculateHeading(float x, float y) {
    float heading = atan2(y, x);
    
    // Convert to degrees
    heading = heading * 180.0 / PI;
    
    // Add magnetic declination
    heading += declination;
    
    // Normalize to 0-360 degrees
    if (heading < 0) {
        heading += 360;
    } else if (heading >= 360) {
        heading -= 360;
    }
    
    return heading;
}

void HMC5883LCompass::applyCalibration() {
    // Apply hard iron correction (offset)
    // The calibration is already applied in the read() function
    // This function can be extended for soft iron correction if needed
    
    // For advanced soft iron correction, you would apply a 3x3 transformation matrix here
    // For now, we're using simple hard iron correction with scale factors
}
