#include <Arduino.h>
#include <ArduinoJson.h>
#include <string>
#include <SPIFFS.h>
#include <WiFi.h>


#include <WebSocketsServer.h>


inline constexpr const char* WIFI_SSID = "ESP32-Team3-Access-Point";
inline constexpr const char* WIFI_PASSWORD = "123456789";

// Core components
WiFiServer server{80};
WebSocketsServer webSocket{81};

void setup() {
    Serial.begin(115200);

    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Access Point IP: ");
    Serial.println(WiFi.softAPIP());

    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    server.begin();
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
}

void loop() {
  webSocket.loop();

    WiFiClient client = server.available();
    if (client) {
        Serial.println("New Client.");
        handleClientRequest(client);
        client.stop();
        Serial.println("Client disconnected.");
    }
}
void handleClientRequest(WiFiClient& client) {
    String request = client.readStringUntil('\r');
    client.read();

    if (request.indexOf("GET / ") >= 0) {
        File file = SPIFFS.open("/index.html", "r");
        if (!file) {
            Serial.println("Failed to open /index.html");
            client.println("HTTP/1.1 500 Internal Server Error");
            client.println("Content-type:text/plain");
            client.println("Connection: close");
            client.println();
            client.println("Failed to load index.html");
            return;
        }

        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();

        client.write(file);

        while (file.available()) {
            client.write(file.read());
        }

        file.close();
        client.println();
    }
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    if (type == WStype_TEXT) {
        String json = (char*)payload;

        JsonDocument doc;
        deserializeJson(doc, json);
        Serial.printf("Recieve Input: %s\n", doc["info"]);
    }
}