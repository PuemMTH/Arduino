#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "esp_camera.h"
#include "BluetoothSerial.h"

// กำหนดค่าของ WiFi และรหัสผ่าน
const char *ssid = "Baanthurada4";
const char *password = "0890174916";

// สร้างออบเจกต์สำหรับ BluetoothSerial
BluetoothSerial SerialBT;

// กำหนดค่าของกล้อง OV2640
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

AsyncWebServer server(80);

void setup()
{
  // กำหนดค่าสำหรับบอร์ด ESP32-CAM
  pinMode(33, OUTPUT);
  digitalWrite(33, LOW);
  delay(50);
  digitalWrite(33, HIGH);

  // ตั้งค่ากล้อง OV2640
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 10;
  config.fb_count = 1;

  // กำหนดค่า Serial
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // ตั้งค่ากล้อง
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // ตั้งค่า Bluetooth
  SerialBT.begin("ESP32-CAM-BT");
  Serial.println("Bluetooth device is now active");

  // ตั้งค่า WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // กำหนดรับค่าสตรีมเว็บ
  server.on("/stream", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", "<html><body><img src=\"/capture\"></body></html>"); });
  // กำหนดรับค่าการจับภาพ
  server.on("/capture", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    camera_fb_t * fb = NULL;
    fb = esp_camera_fb_get();
    if(!fb) {
      Serial.println("Camera capture failed");
      request->send(500);
      return;
    }
    Serial.println("Camera capture success");
    request->send_P(200, "image/jpeg", (const uint8_t *)fb->buf, fb->len); });

  // เริ่มต้นเว็บเซิร์ฟเวอร์
  server.begin();
}

void loop()
{
  if (SerialBT.available())
  {
    Serial.write(SerialBT.read());
  }
}
