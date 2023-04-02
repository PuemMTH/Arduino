#include <WiFi.h>
#include <WebSocketsServer.h>
#include "esp_camera.h"

const char* ssid = "Baanthurada4";
const char* password = "0890174916";
const IPAddress server_ip(192, 168, 1, 218);
const uint16_t server_port = 80;
WebSocketsServer webSocket = WebSocketsServer(81);
camera_fb_t* fb = NULL;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");

  // Print IP address of device
  Serial.println(WiFi.localIP());

  
  // Initialize camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  Serial.println("Camera initialized");

  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  Serial.println("WebSocket server started");
}

void loop() {
  // Wait for WebSocket connection
  webSocket.loop();
  delay(1000);
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.printf("WebSocket client #%d connected\n", num);
      break;
    case WStype_DISCONNECTED:
      Serial.printf("WebSocket client #%d disconnected\n", num);
      break;
    case WStype_TEXT:
      if (payload[0] == 't') {
        // Take photo
        if (fb) {
          esp_camera_fb_return(fb);
        }
        fb = esp_camera_fb_get();
        if (fb) {
          Serial.printf("Took photo with size %d\n", fb->len);
          // Send photo to WebSocket clients
          webSocket.broadcastBIN(fb->buf, fb->len);
        } else {
          Serial.println("Camera capture failed");
        }
      }
      break;
    default:
      break;
  }
}
