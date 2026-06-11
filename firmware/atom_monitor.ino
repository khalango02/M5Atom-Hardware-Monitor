#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const char* ssid = "SSID_WIFI";
const char* password = "SENHA_WIFI";

WebServer server(80);

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

float cpuUsage = 0;
float cpuTemp = 0;

float gpuUsage = 0;
float gpuTemp = 0;

float ramUsage = 0;

float cpuDisplay = 0;
float gpuDisplay = 0;
float ramDisplay = 0;

int currentScreen = 0;

unsigned long lastDraw = 0;

bool lastButtonState = false;

void atualizarAnimacao() {

  cpuDisplay += (cpuUsage - cpuDisplay) * 0.15;

  gpuDisplay += (gpuUsage - gpuDisplay) * 0.15;

  ramDisplay += (ramUsage - ramDisplay) * 0.15;
}

void desenharTelaUso() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0,0);
  display.printf("CPU %2.0f%%", cpuDisplay);

  drawBar(0,10,120,8,(int)cpuDisplay);

  display.setCursor(0,22);
  display.printf("GPU %2.0f%%", gpuDisplay);

  drawBar(0,32,120,8,(int)gpuDisplay);

  display.setCursor(0,44);
  display.printf("RAM %2.0f%%", ramDisplay);

  drawBar(0,54,120,8,(int)ramDisplay);

  display.display();
}

void desenharTelaTemperaturas() {

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(0,0);
  display.println("CPU TEMP");

  display.setTextSize(2);
  display.setCursor(0,12);
  display.printf("%.1fC", cpuTemp);

  display.setTextSize(1);
  display.setCursor(0,40);
  display.println("GPU TEMP");

  display.setTextSize(2);
  display.setCursor(0,50);
  display.printf("%.1fC", gpuTemp);

  display.display();
}

void desenharTelaSistema() {

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(0,0);
  display.println("IP");

  display.println(
    WiFi.localIP().toString()
  );

  display.println();

  display.printf(
    "WiFi %d dBm",
    WiFi.RSSI()
  );

  display.display();
}

void drawBar(int x, int y, int width, int height, int percent) {

  if (percent > 100) percent = 100;
  if (percent < 0) percent = 0;

  display.drawRect(x, y, width, height, SSD1306_WHITE);

  int fillWidth = ((width - 2) * percent) / 100;

  display.fillRect(
    x + 1,
    y + 1,
    fillWidth,
    height - 2,
    SSD1306_WHITE
  );
}

void atualizarTela() {

  switch(currentScreen) {

    case 0:
      desenharTelaUso();
      break;

    case 1:
      desenharTelaTemperaturas();
      break;

    case 2:
      desenharTelaSistema();
      break;
  }
}

void handleUpdate() {

  Serial.println("POST recebido");

  if(server.hasArg("plain")) {

    String body = server.arg("plain");

    Serial.print("Body: ");
    Serial.println(body);

    DynamicJsonDocument doc(256);

    DeserializationError error =
      deserializeJson(doc, body);

    if(error) {

      Serial.print("Erro JSON: ");
      Serial.println(error.c_str());

      server.send(400, "text/plain", "JSON invalido");
      return;
    }

    cpuUsage = doc["cpu_usage"];
    cpuTemp = doc["cpu_temp"];

    gpuUsage = doc["gpu_usage"];
    gpuTemp = doc["gpu_temp"];

    ramUsage = doc["ram_usage"];

    Serial.print("CPU: ");
    Serial.println(cpuUsage);

    Serial.print("TEMP: ");
    Serial.println(cpuTemp);

    server.send(200, "text/plain", "OK");
    return;
  }

  server.send(400, "text/plain", "Erro");
}

void setup() {

  pinMode(39, INPUT_PULLUP);

  Serial.begin(115200);

  Wire.begin(26,32);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  server.on("/update", HTTP_POST, handleUpdate);

  server.begin();

  cpuUsage = 99;
  cpuTemp = 55;

  atualizarTela();

  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println("Aguardando...");
  display.display();
}

void loop() {

  server.handleClient();

  bool buttonPressed =
      digitalRead(39) == LOW;

  if(buttonPressed &&
     !lastButtonState) {

      currentScreen++;

      if(currentScreen > 2)
          currentScreen = 0;
  }

  lastButtonState =
      buttonPressed;

  atualizarAnimacao();

  if(millis() - lastDraw > 50) {

      atualizarTela();

      lastDraw = millis();
  }
}
