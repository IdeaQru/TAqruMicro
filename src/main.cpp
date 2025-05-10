#include "main.h"
void setup()
{
    setup_starting();
}

void loop()
{
    imu.update();
    Serial.print("Roll: ");
    Serial.print(imu.getRoll());
    Serial.print("°, Pitch: ");
    Serial.print(imu.getPitch());
    Serial.print("°, Yaw: ");
    Serial.print(imu.getYaw());
    Serial.print("°");
    
    if (compass.read())
    {
        Serial.print(" || Heading: ");
        Serial.print(compass.getHeading());
        Serial.print("°, X: ");
        Serial.print(compass.getX());
        Serial.print(" µT, Y: ");
        Serial.print(compass.getY());
        Serial.print(" µT, Z: ");
        Serial.print(compass.getZ());
        Serial.println(" µT");
    }else{
        Serial.println("Gagal membaca Kompas!");
    }

    if (gps.readData())
    {
        Serial.println("Lat:" + gps.getLatitude() + " Long:" + gps.getLongitude());
    }

    delay(10);
}
