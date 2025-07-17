#include "main.h"
void setup()
{
    setupMain();
}

void updateSensors()
{
    // imu.update();
    compass.read();
    gps.update();
}

// Fungsi untuk memeriksa validitas data sensor
bool checkSensorDataValidity(float roll, float pitch, float yaw, float heading, float latitude, float longitude)
{
    bool isValid = true;

    // Check IMU data validity
    // if (isnan(roll) || isnan(pitch) || isnan(yaw) ||
    //     roll < -180 || roll > 180 ||
    //     pitch < -90 || pitch > 90 ||
    //     yaw < -180 || yaw > 180)
    // {
    //     Serial.println("ERROR: IMU data invalid!");
    //     isValid = false;
    // }

    // Check compass data validity
    if (isnan(heading) || heading < 0 || heading > 360)
    {
        Serial.println("ERROR: Compass data invalid!");
        isValid = false;
    }

    // Check GPS data validity
    // if (isnan(latitude) || isnan(longitude) ||
    //     latitude == 0.0 || longitude == 0.0 ||
    //     latitude < -90 || latitude > 90 ||
    //     longitude < -180 || longitude > 180)
    // {
    //     Serial.println("ERROR: GPS data invalid!");
    //     isValid = false;
    // }

    return isValid;
}

// Fungsi untuk trigger buzzer dan relay saat data error

void displaySensorData(float roll, float pitch, float yaw, float heading, String Cardir, float latitude, float longitude)
{
    static unsigned long lastDisplay = nol;
    if (millis() - lastDisplay > LCD_UPDATE_INTERVAL_MS)
    {
        // Check if data is valid before displaying
        if (checkSensorDataValidity(roll, pitch, yaw, heading, latitude, longitude))
        {
            lcd.displayIMUData(roll, pitch, yaw);
            delay(LCD_UPDATE_INTERVAL_MS / dua);
            lcd.displayGPSData(latitude, longitude);
            delay(LCD_UPDATE_INTERVAL_MS / dua);
            lcd.displayCompassData(heading, Cardir);
        }
        else
        {
            lcd.displayMessage("SENSOR ERROR", "Check Connection");
        }
        lastDisplay = millis();
    }
}

void sendSensorData(float roll, float pitch, float yaw, float heading, String cardinalDir, float latitude, float longitude)
{
    if (dataSender.isConnected())
    {
        // Only send data if it's valid
        if (checkSensorDataValidity(roll, pitch, yaw, heading, latitude, longitude))
        {
            dataSender.sendData(roll, pitch, yaw, heading, cardinalDir, String(latitude, enam), String(longitude, enam));
        }
        else
        {
            Serial.println("Data not sent due to invalid sensor readings");
        }
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

    float roll = compass.getX() * 180.0 / PI / 100000.0;
    float pitch = compass.getY() * 180.0 / PI / 100000.0;
    float yaw = compass.getZ() * 180.0 / PI / 100000.0;
    float heading = compass.getHeading();
    // manipulasi pitch dari heading
    // float pitchfromheading = heading - 90;
    String cardinalDir = compass.getDirection(heading);
    float latitude = gps.getLatitude();
    float longitude = gps.getLongitude();

    // // Check sensor data validity and trigger alert if invalid
    if (!checkSensorDataValidity(roll, pitch, yaw, heading, latitude, longitude))
    {
        triggerErrorAlert();
    }

    displaySensorData(roll, pitch, yaw, heading, cardinalDir, latitude, longitude);
    sendSensorData(roll, pitch, yaw, heading, cardinalDir, latitude, longitude);
    debugSensorData(roll, pitch, yaw, heading, cardinalDir, latitude, longitude);

    delay(LOOP_DELAY_MS);
}
