#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <map>
#include <string>

class CommModule {
public:
    CommModule(const char* udpAddress, uint16_t udpPort);

    // Init WiFi + UDP
    void begin(const char* ssid, const char* password);

    // Send current key/value map via UDP (JSON)
    void send(const std::map<String, int>& kv);

    // Receive key/value map from UDP or Serial
    std::map<String, int> receive();

private:
    const char* udpAddress;
    uint16_t udpPort;
    WiFiUDP udp;
};
