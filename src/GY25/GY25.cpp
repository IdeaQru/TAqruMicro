#include "GY25.h"

Gy25::Gy25() {
    roll = 0.0;
    pitch = 0.0;
    yaw = 0.0;
}

bool Gy25::begin(int rx_pin, int tx_pin) {
    // Inisialisasi Serial2 untuk komunikasi dengan GY-25
    Serial2.begin(BAUD_RATE, SERIAL_8N1, rx_pin, tx_pin);
    return true; // Asumsi inisialisasi selalu berhasil
}

void Gy25::update() {
    // Baca data dari Serial2 jika tersedia
    if (Serial2.available()) {
        String data = Serial2.readStringUntil('\n'); // Baca hingga newline
        parseSerialData(data); // Parse data ke sudut
    }
}

void Gy25::parseSerialData(String data) {
    // Asumsi format data: "Roll:xxx Pitch:xxx Yaw:xxx"
    // Contoh: "Roll:12.34 Pitch:56.78 Yaw:90.12"
    int rollIndex = data.indexOf("Roll:");
    int pitchIndex = data.indexOf("Pitch:");
    int yawIndex = data.indexOf("Yaw:");

    if (rollIndex != -1 && pitchIndex != -1 && yawIndex != -1) {
        // Ekstrak nilai Roll
        String rollStr = data.substring(rollIndex + 5, pitchIndex);
        rollStr.trim();
        roll = rollStr.toFloat();

        // Ekstrak nilai Pitch
        String pitchStr = data.substring(pitchIndex + 6, yawIndex);
        pitchStr.trim();
        pitch = pitchStr.toFloat();

        // Ekstrak nilai Yaw
        String yawStr = data.substring(yawIndex + 4);
        yawStr.trim();
        yaw = yawStr.toFloat();
    }
}
