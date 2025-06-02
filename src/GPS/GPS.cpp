// GPS.cpp
#include "GPS.h"

GPS::GPS(int rxPin, int txPin) : gpsSerial(1) {
    latitude = 0.0;
    longitude = 0.0;
    speed = 0.0;
    altitude = 0.0;
    satellites = 0;
    dataValid = false;
    nmeaBuffer = "";
}

void GPS::begin(long baudRate) {
    gpsSerial.begin(baudRate, SERIAL_8N1, 16, 17); // RX=16, TX=17 untuk ESP32
}

void GPS::update() {
    while (gpsSerial.available()) {
        char c = gpsSerial.read();
        
        if (c == '\n' || c == '\r') {
            if (nmeaBuffer.length() > 0) {
                parseNMEA(nmeaBuffer);
                nmeaBuffer = "";
            }
        } else {
            nmeaBuffer += c;
        }
    }
}

void GPS::parseNMEA(String sentence) {
    if (sentence.startsWith("$GPGGA") || sentence.startsWith("$GNGGA")) {
        parseGPGGA(sentence);
    } else if (sentence.startsWith("$GPRMC") || sentence.startsWith("$GNRMC")) {
        parseGPRMC(sentence);
    } else if (sentence.startsWith("$GPGSA") || sentence.startsWith("$GNGSA")) {
        parseGPGSA(sentence);
    }
}

void GPS::parseGPGGA(String sentence) {
    if (!isValidChecksum(sentence)) return;
    
    // $GPGGA,time,lat,N/S,lon,E/W,quality,satellites,hdop,altitude,M,geoid,M,dgps_time,dgps_id*checksum
    String time = getValue(sentence, ',', 1);
    String lat = getValue(sentence, ',', 2);
    String latDir = getValue(sentence, ',', 3);
    String lon = getValue(sentence, ',', 4);
    String lonDir = getValue(sentence, ',', 5);
    String quality = getValue(sentence, ',', 6);
    String sats = getValue(sentence, ',', 7);
    String alt = getValue(sentence, ',', 9);
    
    if (quality.toInt() > 0 && lat.length() > 0 && lon.length() > 0) {
        latitude = convertToDecimalDegrees(lat, latDir.charAt(0));
        longitude = convertToDecimalDegrees(lon, lonDir.charAt(0));
        satellites = sats.toInt();
        altitude = alt.toFloat();
        dataValid = true;
    } else {
        dataValid = false;
    }
}

void GPS::parseGPRMC(String sentence) {
    if (!isValidChecksum(sentence)) return;
    
    // $GPRMC,time,status,lat,N/S,lon,E/W,speed,course,date,magnetic_variation,checksum
    String status = getValue(sentence, ',', 2);
    String lat = getValue(sentence, ',', 3);
    String latDir = getValue(sentence, ',', 4);
    String lon = getValue(sentence, ',', 5);
    String lonDir = getValue(sentence, ',', 6);
    String speedKnots = getValue(sentence, ',', 7);
    
    if (status == "A" && lat.length() > 0 && lon.length() > 0) {
        latitude = convertToDecimalDegrees(lat, latDir.charAt(0));
        longitude = convertToDecimalDegrees(lon, lonDir.charAt(0));
        speed = speedKnots.toFloat() * 1.852; // Convert knots to km/h
        dataValid = true;
    }
}

void GPS::parseGPGSA(String sentence) {
    if (!isValidChecksum(sentence)) return;
    
    // $GPGSA,mode1,mode2,sat1,sat2,...,sat12,pdop,hdop,vdop*checksum
    String mode2 = getValue(sentence, ',', 2);
    
    if (mode2.toInt() >= 2) {
        // Count satellites in use
        int satCount = 0;
        for (int i = 3; i <= 14; i++) {
            String sat = getValue(sentence, ',', i);
            if (sat.length() > 0 && sat.toInt() > 0) {
                satCount++;
            }
        }
        satellites = satCount;
    }
}

float GPS::convertToDecimalDegrees(String coordinate, char direction) {
    if (coordinate.length() < 4) return 0.0;
    
    // Format: DDMM.MMMM or DDDMM.MMMM
    int dotIndex = coordinate.indexOf('.');
    if (dotIndex < 0) return 0.0;
    
    String degreesStr, minutesStr;
    
    if (coordinate.length() >= 10) { // Longitude format DDDMM.MMMM
        degreesStr = coordinate.substring(0, 3);
        minutesStr = coordinate.substring(3);
    } else { // Latitude format DDMM.MMMM
        degreesStr = coordinate.substring(0, 2);
        minutesStr = coordinate.substring(2);
    }
    
    float degrees = degreesStr.toFloat();
    float minutes = minutesStr.toFloat();
    
    float decimal = degrees + (minutes / 60.0);
    
    if (direction == 'S' || direction == 'W') {
        decimal = -decimal;
    }
    
    return decimal;
}

bool GPS::isValidChecksum(String sentence) {
    int asteriskIndex = sentence.lastIndexOf('*');
    if (asteriskIndex < 0 || asteriskIndex + 3 != sentence.length()) {
        return false;
    }
    
    String checksumStr = sentence.substring(asteriskIndex + 1);
    int receivedChecksum = hexCharToInt(checksumStr.charAt(0)) * 16 + hexCharToInt(checksumStr.charAt(1));
    
    int calculatedChecksum = 0;
    for (int i = 1; i < asteriskIndex; i++) { // Skip the '$' character
        calculatedChecksum ^= sentence.charAt(i);
    }
    
    return receivedChecksum == calculatedChecksum;
}

String GPS::getValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;
    
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

int GPS::hexCharToInt(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}
