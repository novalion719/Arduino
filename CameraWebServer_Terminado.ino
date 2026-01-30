#include "esp_camera.h"
#include <WiFi.h>

// =============================================================
// 1. SELECCIÓN DE MODELO (AI THINKER)
// =============================================================
#define CAMERA_MODEL_AI_THINKER // <--- Modelo seleccionado correctamente
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
//#define CAMERA_MODEL_M5STACK_ESP32CAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL

// =============================================================
// 2. TUS CREDENCIALES WIFI
// =============================================================
const char* ssid = "Nova";
const char* password = "novalion712";

// =============================================================
// 3. CONFIGURACIÓN DE IP FIJA (NO CAMBIARÁ)
// =============================================================
// Elige una IP que termine en un número alto (.25, .50, .200)
IPAddress local_IP(192, 168, 0, 25);     // <--- TU IP FIJA: 192.168.0.25
IPAddress gateway(192, 168, 0, 1);       // <--- TU ROUTER
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); 
IPAddress secondaryDNS(8, 8, 4, 4);

// =============================================================
// NO TOCAR - ARCHIVOS DEL SISTEMA
// =============================================================
#include "camera_pins.h"

// Esta función está en la otra pestaña (app_httpd.cpp)
void startCameraServer();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Inicializar cámara
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); 
    s->set_brightness(s, 1); 
    s->set_saturation(s, -2); 
  }
  s->set_framesize(s, FRAMESIZE_QVGA);

  // =============================================================
  // APLICAR IP ESTÁTICA ANTES DE CONECTAR
  // =============================================================
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Fallo al configurar IP Estática");
  }

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  Serial.print("Conectando al WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");

  // INICIAR INTERFAZ WEB COMPLETA
  startCameraServer();

  Serial.print("Cámara lista! Usa esta dirección FIJA: http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(10000);
}