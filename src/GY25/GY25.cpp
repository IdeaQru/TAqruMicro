#include "GY25.h"

Gy25::Gy25(uint8_t address) : mpu(nullptr), roll(0.0), pitch(0.0), yaw(0.0),
                              lastUpdate(0) {
    // Inisialisasi pointer MPU6050 akan dilakukan di begin()
}

bool Gy25::begin(int sda_pin, int scl_pin) {
    Wire.begin(sda_pin, scl_pin);
    delay(DELAY_INIT_MS);
    
    // Inisialisasi MPU6050 dengan TwoWire
    mpu = new MPU6050(Wire);
    if (!mpu) {
        return false;
    }
    
    byte status = mpu->begin();
    if (status != 0) {
        delete mpu;
        mpu = nullptr;
        return false;
    }
    
    mpu->setGyroConfig(GYRO_RANGE_CONFIG); // Gyro range
    mpu->setAccConfig(ACCEL_RANGE_CONFIG); // Accel range
    calibrate();
    return true;
}

void Gy25::calibrate() {
    if (mpu) {
        mpu->calcOffsets(true, true); // Calibrate gyro and accel offsets
    }
}

void Gy25::update() {
    if (!mpu) return;
    
    unsigned long currentTime = micros();
    float deltaTime = (currentTime - lastUpdate) / MICRO_TO_SECONDS; // Convert to seconds
    lastUpdate = currentTime;

    mpu->update();
    calculateAngles();
}

void Gy25::calculateAngles() {
    if (!mpu) return;
    
    // Get accelerometer data
    float accX = mpu->getAccX();
    float accY = mpu->getAccY();
    float accZ = mpu->getAccZ();

    // Get gyroscope data (angular velocity in deg/s)
    float gyroX = mpu->getGyroX();
    float gyroY = mpu->getGyroY();
    float gyroZ = mpu->getGyroZ();

    // Calculate roll and pitch from accelerometer
    float accRoll = atan2(accY, accZ) * (static_cast<float>(180.0) / PI_VALUE);
    float accPitch = atan2(-accX, sqrt(accY * accY + accZ * accZ)) * (static_cast<float>(180.0) / PI_VALUE);

    // Calculate time difference for integration
    static unsigned long prevTime = 0;
    float dt = (micros() - prevTime) / MICRO_TO_SECONDS; // Convert to seconds
    prevTime = micros();

    // Integrate gyroscope data for roll, pitch, and yaw
    roll += gyroX * dt;
    pitch += gyroY * dt;
    yaw += gyroZ * dt;

    // Apply complementary filter to combine accelerometer and gyroscope data
    roll = COMPLEMENTARY_ALPHA * roll + (static_cast<float>(1.0) - COMPLEMENTARY_ALPHA) * accRoll;
    pitch = COMPLEMENTARY_ALPHA * pitch + (static_cast<float>(1.0) - COMPLEMENTARY_ALPHA) * accPitch;
}
