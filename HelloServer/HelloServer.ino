#include <WiFi.h>
#include <esp_wifi.h>

const char* ssid = "Baanthurada4";
const char* password = "0890174916";
const char* apSsid = "ESP32-AP";
const char* apPassword = "1122334455667788";
void setup() {
  Serial.begin(115200);
  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Set up Wi-Fi AP
  WiFi.softAP(apSsid, apPassword);
  Serial.println("Wi-Fi AP started");
  Serial.print("SSID: ");
  Serial.println(apSsid);
  Serial.print("Password: ");
  Serial.println(apPassword);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Enable NAT and DNS forwarding
  esp_wifi_set_mode(WIFI_MODE_APSTA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filter);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
}

void promiscuousCallback(uint8_t *buf, uint16_t len) {
  int rssi = (int8_t) buf[0];
  Serial.println("RSSI: " + String(rssi));
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_NULL);
  WiFi.mode(WIFI_MODE_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(0); // capture all packets
  esp_wifi_set_promiscuous_rx_cb(promiscuousCallback);
}

void loop() {
  // your code here
}
void loop() {
  // your code here
}
