#include "main.h"

void setup()
{
    setupMain();
}

void updateSensors()
{
    imu.update();
    compass.read();
    gps.update();
}

void displaySensorData(float roll, float pitch, float yaw, float heading, String Cardir, float latitude, float longitude)
{
    static unsigned long lastDisplay = nol;
    if (millis() - lastDisplay > LCD_UPDATE_INTERVAL_MS)
    {
        lcd.displayIMUData(roll, pitch, yaw);
        delay(LCD_UPDATE_INTERVAL_MS / dua);
        lcd.displayGPSData(latitude, longitude);
        delay(LCD_UPDATE_INTERVAL_MS / dua);
        lcd.displayCompassData(heading, Cardir);
        lastDisplay = millis();
    }
}

void sendSensorData(float roll, float pitch, float yaw, float heading, String cardinalDir, float latitude, float longitude)
{
    if (dataSender.isConnected())
    {
        dataSender.sendData(roll, pitch, yaw, heading, cardinalDir, String(latitude, enam), String(longitude, enam));
    }
}

void debugSensorData(float roll, float pitch, float yaw, float heading, String cardinalDir, float latitude, float longitude)
{
    Serial.printf("Roll: %.2f, Pitch: %.2f, Yaw: %.2f, Heading: %.2f, Cardinal: %s, Lat: %.6f, Lon: %.6f\n",
                  roll, pitch, yaw, heading, cardinalDir.c_str(), latitude, longitude);
}

void loop()
{
    updateSensors();

    // Get sensor values
    float roll = imu.getRoll();
    float pitch = imu.getPitch();
    float yaw = imu.getYaw();
    float heading = compass.getHeading();
    String cardinalDir = compass.getCardinalDirection(heading);
    float latitude = gps.getLatitude();
    float longitude = gps.getLongitude();

    displaySensorData(roll, pitch, yaw, heading, cardinalDir, latitude, longitude);
    sendSensorData(roll, pitch, yaw, heading, cardinalDir, latitude, longitude);
    debugSensorData(roll, pitch, yaw, heading, cardinalDir, latitude, longitude);

    delay(LOOP_DELAY_MS);
}
