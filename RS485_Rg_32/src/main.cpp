#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RX_PIN 16
#define TX_PIN 17
#define DE_RE 13
#define BUZZ 23
#define BTN_UP 34   // P34 (Pull-down da placa)
#define BTN_DOWN 32 // P32 (Pull-down da placa)

HardwareSerial RS485(2);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

int valorIHM = 0;
unsigned long ultimoEnvio = 0;
unsigned long ultimaTela = 0;
unsigned long pacotesPerdidos = 0;

String bufferEntrada = "";
int ultimoEstadoUp = LOW;
int ultimoEstadoDown = LOW;

void enviarPacote() {
  String payload = "VAL:" + String(valorIHM);
  byte crc = 0;
  for (int i = 0; i < payload.length(); i++) crc ^= payload[i];
  String checksum = String(crc, HEX);
  checksum.toUpperCase();
  
  String pacote = "<" + payload + ":" + checksum + ">";

  digitalWrite(DE_RE, HIGH);
  delay(1); // Tempo mínimo para o MAX485 acordar
  RS485.print(pacote);
  RS485.flush(); // Espera o dado sair do fio
  digitalWrite(DE_RE, LOW); // Volta a escutar
}

void atualizarTela() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("IHM - MODO TURBO");

  display.setTextSize(3);
  display.setCursor(45, 20);
  display.print(valorIHM);

  display.setTextSize(1);
  display.setCursor(0, 55);
  display.print("FALHAS ACK: ");
  display.print(pacotesPerdidos);
  display.display();
}

void setup() {
  pinMode(DE_RE, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(BTN_UP, INPUT);
  pinMode(BTN_DOWN, INPUT);
  digitalWrite(DE_RE, LOW);

  RS485.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  atualizarTela();
}

void loop() {
  // 1. Leitura dos Botões (Super Rápida)
  int lerUp = digitalRead(BTN_UP);
  int lerDown = digitalRead(BTN_DOWN);
  if (lerUp == HIGH && ultimoEstadoUp == LOW) { valorIHM++; delay(30); }
  if (lerDown == HIGH && ultimoEstadoDown == LOW) { valorIHM--; delay(30); }
  ultimoEstadoUp = lerUp;
  ultimoEstadoDown = lerDown;

  // 2. Metralhadora de Dados (A cada 50ms = 20 pacotes/seg)
  if (millis() - ultimoEnvio >= 50) {
    ultimoEnvio = millis();
    enviarPacote();
  }

  // 3. Leitura do C6 (Sem readStringUntil = Sem travamento)
  while (RS485.available() > 0) {
    char c = RS485.read();
    if (c == '<') { 
      bufferEntrada = "<"; 
    }
    else if (c == '>') {
      bufferEntrada += ">";
      if (bufferEntrada.indexOf("ACK") == -1) {
         // O que chegou não foi um ACK válido
         pacotesPerdidos++; 
      }
      bufferEntrada = "";
    } else {
      bufferEntrada += c;
    }
    
    // Trava de segurança contra lixo elétrico enchendo a memória
    if (bufferEntrada.length() > 20) bufferEntrada = "";
  }

  // 4. Atualização de Tela Desacoplada (Apenas 4 vezes por segundo)
  if (millis() - ultimaTela >= 250) {
    ultimaTela = millis();
    atualizarTela();
  }
}