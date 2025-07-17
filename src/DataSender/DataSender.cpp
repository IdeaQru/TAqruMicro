#include "DataSender.h"

DataSender::DataSender(const char *ssid, const char *password, const char *serverUrl)
    : _ssid(ssid), _password(password), _serverUrl(serverUrl) {}

void DataSender::begin()
{
  connectWiFi();
}

bool DataSender::isConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

void DataSender::connectWiFi()
{
  WiFi.begin(_ssid, _password);

  Serial.print("Connecting to WiFi");
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000)
  {
    Serial.print(".");
    delay(500);
  }
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println();
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println();
    Serial.println("Failed to connect to WiFi");
  }
}

void DataSender::sendData(float roll, float pitch, float yaw, float heading, String cardinalDirection, String latitude, String longitude)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected, trying to reconnect...");
    connectWiFi();
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Still not connected. Aborting send.");
      return;
    }
  }

  HTTPClient http;
  http.begin(_serverUrl);
  http.addHeader("Content-Type", "application/json");

  String payload = "{";
  payload += "\"roll\":" + String(roll, 2) + ",";
  payload += "\"pitch\":" + String(pitch, 2) + ",";
  payload += "\"yaw\":" + String(yaw, 2) + ",";
  payload += "\"heading\":" + String(heading, 2) + ",";
  payload += "\"cardinalDirection\":\"" + cardinalDirection + "\",";
  payload += "\"latitude\":\"" + latitude + "\",";
  payload += "\"longitude\":\"" + longitude + "\"";
  payload += "}";

  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0)
  {
    String response = http.getString();
    Serial.println("POST Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  }
  else
  {
    Serial.println("Error sending POST: " + String(httpResponseCode));
  }

  http.end();
}
