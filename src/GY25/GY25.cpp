#include "Gy25.h"

Gy25::Gy25(uint8_t address) : i2cAddress(address), kalmanX(), kalmanY() {}

bool Gy25::begin(int sda_pin, int scl_pin) {
    Wire.begin(sda_pin, scl_pin);
    Wire.setClock(400000);
    
    if(!initMPU()) {
        return false;
    }
    
    calibrate();
    lastUpdate = millis();
    return true;
}

void Gy25::update() {
    SensorData data;
    if(readSensor(data)) {
        float deltaTime = (millis() - lastUpdate) / 1000.0f;
        lastUpdate = millis();
        
        applyCalibration(data);
        updateAngles(data, deltaTime);
    }else{
        Serial.println("Imu Gagal");
    }
}

bool Gy25::initMPU() {
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x6B);
    Wire.write(0x00);
    if(Wire.endTransmission()) return false;

    Wire.beginTransmission(i2cAddress);
    Wire.write(0x1C);
    Wire.write(0x08);
    if(Wire.endTransmission()) return false;

    Wire.beginTransmission(i2cAddress);
    Wire.write(0x1B);
    Wire.write(0x08);
    if(Wire.endTransmission()) return false;

    Wire.beginTransmission(i2cAddress);
    Wire.write(0x1A);
    Wire.write(0x03);
    return Wire.endTransmission() == 0;
}

bool Gy25::readSensor(SensorData &data) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x3B);
    if(Wire.endTransmission(false) != 0) return false;

    if(Wire.requestFrom(i2cAddress, 14) != 14) return false;

    data.accX = Wire.read()<<8 | Wire.read();
    data.accY = Wire.read()<<8 | Wire.read();
    data.accZ = Wire.read()<<8 | Wire.read();
    data.temp = Wire.read()<<8 | Wire.read();
    data.gyroX = Wire.read()<<8 | Wire.read();
    data.gyroY = Wire.read()<<8 | Wire.read();
    data.gyroZ = Wire.read()<<8 | Wire.read();

    return true;
}

void Gy25::calibrate() {
    float sumAccX = 0, sumAccY = 0, sumAccZ = 0;
    float sumGyroX = 0, sumGyroY = 0, sumGyroZ = 0;

    for(int i=0; i<CALIBRATION_SAMPLES; i++) {
        SensorData data;
        if(readSensor(data)) {
            sumAccX += data.accX;
            sumAccY += data.accY;
            sumAccZ += data.accZ;
            
            sumGyroX += data.gyroX;
            sumGyroY += data.gyroY;
            sumGyroZ += data.gyroZ;
            
            delay(2);
        }
    }

    offsets.accX = sumAccX / CALIBRATION_SAMPLES;
    offsets.accY = sumAccY / CALIBRATION_SAMPLES;
    offsets.accZ = (sumAccZ / CALIBRATION_SAMPLES) - 16384;
    
    offsets.gyroX = sumGyroX / CALIBRATION_SAMPLES;
    offsets.gyroY = sumGyroY / CALIBRATION_SAMPLES;
    offsets.gyroZ = sumGyroZ / CALIBRATION_SAMPLES;
}

void Gy25::applyCalibration(SensorData &data) {
    data.accX -= offsets.accX;
    data.accY -= offsets.accY;
    data.accZ -= offsets.accZ;
    
    data.gyroX -= offsets.gyroX;
    data.gyroY -= offsets.gyroY;
    data.gyroZ -= offsets.gyroZ;

    if(abs(data.gyroX) < GYRO_THRESHOLD) data.gyroX = 0;
    if(abs(data.gyroY) < GYRO_THRESHOLD) data.gyroY = 0;
    if(abs(data.gyroZ) < GYRO_THRESHOLD) data.gyroZ = 0;
}

void Gy25::updateAngles(SensorData &data, float deltaTime) {
    // Hitung sudut accelerometer
    float accRoll = atan2(data.accY, data.accZ) * 180.0 / M_PI;
    float accPitch = atan(-data.accX / sqrt(data.accY*data.accY + data.accZ*data.accZ)) * 180.0 / M_PI;

    // Hitung gyro rate
    float gyroX = data.gyroX / 65.5;
    float gyroY = data.gyroY / 65.5;
    float gyroZ = data.gyroZ / 65.5;

    // Update sudut gyro (integrasi)
    angles.gyroRoll += gyroX * deltaTime;
    angles.gyroPitch += gyroY * deltaTime;
    angles.gyroYaw += gyroZ * deltaTime;

    // Filter complementary
    angles.compRoll = COMPLEMENTARY_ALPHA * (angles.compRoll + gyroX * deltaTime) + (1 - COMPLEMENTARY_ALPHA) * accRoll;
    angles.compPitch = COMPLEMENTARY_ALPHA * (angles.compPitch + gyroY * deltaTime) + (1 - COMPLEMENTARY_ALPHA) * accPitch;

    // Filter Kalman
    angles.kalRoll = kalmanX.update(accRoll, gyroX, deltaTime);
    angles.kalPitch = kalmanY.update(accPitch, gyroY, deltaTime);

    // Hitung error antara accelerometer dan complementary filter
    float errorRoll = accRoll - angles.compRoll;
    float errorPitch = accPitch - angles.compPitch;

    // Output PID adaptif untuk koreksi sudut
    float pidOutputRoll = pidRoll.update(errorRoll, deltaTime);
    float pidOutputPitch = pidPitch.update(errorPitch, deltaTime);

    // Koreksi sudut dengan output PID
    angles.compRoll += pidOutputRoll;
    angles.compPitch += pidOutputPitch;
}


Gy25::Kalman::Kalman(float Q, float R) : Q(Q), R(R), x0(0), x1(0), P00(1), P01(0), P10(0), P11(1) {}

float Gy25::Kalman::update(float measurement, float rate, float dt) {
    float P = P00 + dt * (dt*P11 - P01 - P10 + Q);
    float S = R + P00;
    float K0 = P00 / S;
    float K1 = P10 / S;

    float y = measurement - x0;
    x0 += K0 * y;
    x1 += K1 * y;

    P00 -= K0 * P00;
    P01 -= K0 * P01;
    P10 -= K1 * P00;
    P11 -= K1 * P01;

    return x0;
}
