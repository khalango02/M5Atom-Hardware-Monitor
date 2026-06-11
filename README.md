# M5Atom Lite PC Hardware Monitor

Monitor de hardware para PC utilizando **M5Atom Lite + Display OLED SSD1306 128x64**, exibindo em tempo real:

* Uso da CPU
* Temperatura da CPU
* Uso da GPU
* Temperatura da GPU
* Uso da RAM
* Informações de rede

Os dados são coletados no Windows através do Libre Hardware Monitor e enviados para o M5Atom via HTTP.

---

# Demonstração

## Tela 1 - Utilização

```text
CPU 25%
█████░░░░░░░░░░

GPU 71%
██████████████░

RAM 46%
█████████░░░░░░
```

## Tela 2 - Temperaturas

```text
CPU TEMP

47.1C

GPU TEMP

33.2C
```

## Tela 3 - Sistema

```text
IP

192.168.1.12

WiFi -57 dBm
```

Troca de telas através do botão integrado do M5Atom Lite.

---

# Arquitetura

```text
┌─────────────────────┐
│ Libre Hardware      │
│ Monitor             │
└─────────┬───────────┘
          │ HTTP JSON
          ▼
┌─────────────────────┐
│ Python Monitor      │
└─────────┬───────────┘
          │ POST
          ▼
┌─────────────────────┐
│ M5Atom Lite         │
│ ESP32               │
└─────────┬───────────┘
          │ I2C
          ▼
┌─────────────────────┐
│ OLED SSD1306        │
│ 128x64              │
└─────────────────────┘
```

---

# Hardware

## Componentes

* M5Atom Lite
* Display OLED SSD1306 128x64 I2C
* Cabo USB-C
* PC Windows

---

# Ligações

## OLED SSD1306

| OLED | M5Atom Lite |
| ---- | ----------- |
| VCC  | 5V          |
| GND  | GND         |
| SDA  | GPIO26      |
| SCL  | GPIO32      |

### Diagrama

```text
OLED SSD1306            M5Atom Lite

VCC -----------------> 5V

GND -----------------> GND

SDA -----------------> GPIO26

SCL -----------------> GPIO32
```

---

# Ambiente Arduino

## Instalar suporte ESP32

Arduino IDE:

```text
Boards Manager
→ ESP32 by Espressif Systems
```

Selecionar:

```text
M5Atom Lite
```

ou

```text
M5Stack Atom
```

dependendo da versão instalada.

---

# Bibliotecas

Instalar:

```text
Adafruit SSD1306
Adafruit GFX
ArduinoJson
WiFi
WebServer
```

---

# Configuração WiFi

Alterar no firmware:

```cpp
const char* ssid = "SEU_WIFI";
const char* password = "SUA_SENHA";
```

---

# Servidor HTTP

O M5Atom cria um endpoint:

```http
POST /update
```

Exemplo:

```json
{
  "cpu_usage": 24,
  "cpu_temp": 58,
  "gpu_usage": 71,
  "gpu_temp": 63,
  "ram_usage": 42
}
```

---

# Firmware ESP32

## Variáveis recebidas

```cpp
float cpuUsage;
float cpuTemp;

float gpuUsage;
float gpuTemp;

float ramUsage;
```

---

## Animação suave

Para evitar que as barras "saltem" entre valores:

```cpp
cpuDisplay += (cpuUsage - cpuDisplay) * 0.15;

gpuDisplay += (gpuUsage - gpuDisplay) * 0.15;

ramDisplay += (ramUsage - ramDisplay) * 0.15;
```

---

# Troca de telas

O botão integrado do M5Atom utiliza:

```cpp
GPIO39
```

A cada clique:

```cpp
Tela 1 -> Tela 2 -> Tela 3 -> Tela 1
```

---

# Libre Hardware Monitor

Baixar:

https://github.com/LibreHardwareMonitor/LibreHardwareMonitor

Executar o software.

Habilitar o servidor web.

Endpoint:

```text
http://localhost:8085/data.json
```

---

# Script Python

## Dependências

```bash
pip install requests
```

---

## Coleta de sensores

Sensores utilizados:

### CPU

```text
CPU Total
Core (Tctl/Tdie)
```

### GPU

```text
GPU Core (%)
GPU Core (°C)
```

### RAM

```text
Memory (%)
```

---

## Envio para o ESP32

```python
requests.post(
    f"http://192.168.1.12/update",
    json=payload,
    timeout=2
)
```

---

# Execução

## 1. Iniciar Libre Hardware Monitor

Verificar:

```text
http://localhost:8085/data.json
```

---

## 2. Executar monitor Python

```bash
python monitor.py
```

Saída:

```python
{
 'cpu_usage': 5.1,
 'cpu_temp': 48.3,
 'gpu_usage': 0.0,
 'gpu_temp': 33.2,
 'ram_usage': 46.0
}
```

---

## 3. Energizar o M5Atom

Conectar via USB.

Aguardar conexão WiFi.

---

## 4. Visualizar métricas

Pressionar o botão do Atom para alternar entre:

* Uso
* Temperaturas
* Sistema

---

# Melhorias Futuras

* LED RGB indicando temperatura da CPU
* Uptime do computador
* Temperatura de SSD
* Uso de rede
* Velocidade de download/upload
* Troca automática de telas
* Dashboard Web
* MQTT
* Home Assistant
* Suporte a múltiplos PCs
* OTA (Over The Air Update)

---

# Resultado Final

Projeto de monitoramento de hardware em tempo real utilizando:

* ESP32 (M5Atom Lite)
* OLED SSD1306
* Libre Hardware Monitor
* Python
* HTTP REST

Permitindo visualizar CPU, GPU, RAM e temperaturas diretamente em um display externo conectado ao computador.
