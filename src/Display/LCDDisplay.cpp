#include "LCDDisplay.h"

LCDDisplay::LCDDisplay(int sda_pin, int scl_pin, uint8_t address)
    : lcd(address, 16, 2), sdaPin(sda_pin), sclPin(scl_pin), i2cAddress(address) {}

bool LCDDisplay::begin() {
    Wire.begin(sdaPin, sclPin);
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Initializing...");
    delay(1000);
    lcd.clear();
    return true;
}

void LCDDisplay::displayMessage(String line1, String line2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1.substring(0, 16)); // Ensure it fits on 16-char display
    lcd.setCursor(0, 1);
    lcd.print(line2.substring(0, 16));
}

void LCDDisplay::displayIMUData(float roll, float pitch, float yaw) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("R:" + String(roll, 1) + " P:" + String(pitch, 1));
    lcd.setCursor(0, 1);
    lcd.print("Yaw:" + String(yaw, 1));
}

void LCDDisplay::displayGPSData(float latitude, float longitude) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lat:" + String(latitude, 4));
    lcd.setCursor(0, 1);
    lcd.print("Lon:" + String(longitude, 4));
}
void LCDDisplay::displayCompassData(float heading, String cardinalDir) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Heading:" + String(heading, 1) );
    lcd.setCursor(0, 1);
    lcd.print("Cardinal:" + cardinalDir);
}
