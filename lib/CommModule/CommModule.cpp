#include "CommModule.h"

CommModule::CommModule(const char* addr, uint16_t port)
    : udpAddress(addr), udpPort(port) { }

void CommModule::begin(const char* ssid, const char* password) {
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    Serial.print("ESP32 IP: "); Serial.println(WiFi.localIP());
    udp.begin(udpPort);
    Serial.printf("Listening on UDP port %d...\n", udpPort);
}

void CommModule::send(const std::map<String, int>& kv) {
    DynamicJsonDocument doc(128);
    for (auto const& [key, value] : kv) {
        doc[key] = value;
    }

    char buffer[128];
    size_t len = serializeJson(doc, buffer);

    udp.beginPacket(udpAddress, udpPort);
    udp.write((uint8_t*)buffer, len);
    udp.endPacket();

    // Also print to Serial
    Serial.printf("Sent (%d bytes): %s\n", (int)len, buffer);
}

std::map<String, int> CommModule::receive() {
    std::map<String, int> kv;

    int packetSize = udp.parsePacket();
    if (packetSize) {
        char incoming[256];
        int rlen = udp.read(incoming, sizeof(incoming)-1);
        if (rlen > 0) incoming[rlen] = 0;

        DynamicJsonDocument doc(256);
        if (deserializeJson(doc, incoming) == DeserializationError::Ok) {
            for (JsonPair pair : doc.as<JsonObject>()) {
                int val = pair.value().as<int>();
                kv[pair.key().c_str()] = val;
                // Serial.printf("Received: %s = %d\n", pair.key().c_str(), val);
            }
        } else {
            Serial.printf("Invalid JSON: %s\n", incoming);
        }
    }

    // Optionally: add serial read here (like commands from USB)
    // while (Serial.available()) { ... }

    return kv;
}
