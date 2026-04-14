#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RX_PIN 7
#define TX_PIN 6
#define DE_RE 8

HardwareSerial RS485(1);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

int ultimoValor = 0;
unsigned long pacotesOK = 0;
unsigned long pacotesERRO = 0;
String bufferEntrada = "";
unsigned long ultimaTela = 0;

void atualizarTela() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("C6 - RS485 Monitor");

  display.setCursor(0,15);
  display.print("OK: "); display.print(pacotesOK);
  display.print(" ERR: "); display.println(pacotesERRO);

  display.setCursor(0,40);
  display.print("VALOR IHM:");
  
  display.setTextSize(2);
  display.setCursor(45, 50);
  display.print(ultimoValor);

  display.display();
}

void setup() {
  pinMode(DE_RE, OUTPUT);
  digitalWrite(DE_RE, LOW); // Nasce escutando

  RS485.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  Wire.begin(23, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  atualizarTela();
}

void loop() {
  // 1. Processamento Bruto do Barramento
  while (RS485.available() > 0) {
    char c = RS485.read();
    if (c == '<') { 
      bufferEntrada = ""; // Início de pacote
    }
    else if (c == '>') {
      // Fim de pacote - Hora da auditoria
      int sep = bufferEntrada.lastIndexOf(':');
      if (sep != -1) {
        String payload = bufferEntrada.substring(0, sep);
        String crcRx = bufferEntrada.substring(sep + 1);

        byte crcCalc = 0;
        for (int i=0; i<payload.length(); i++) crcCalc ^= payload[i];
        String crcCalcStr = String(crcCalc, HEX);
        crcCalcStr.toUpperCase();

        if (crcRx == crcCalcStr) {
          // PACOTE LIMPO E APROVADO!
          pacotesOK++;
          ultimoValor = payload.substring(4).toInt();

          // Envia o Recibo (ACK)
          digitalWrite(DE_RE, HIGH);
          delay(1);
          RS485.print("<ACK>");
          RS485.flush();
          digitalWrite(DE_RE, LOW);
        } else {
          // EMI: Checksum falhou
          pacotesERRO++;
        }
      } else {
        // EMI: Sinal corrompido ou cortado
        pacotesERRO++;
      }
      bufferEntrada = "";
    } else {
      bufferEntrada += c;
    }

    // Trava de segurança contra inundação de lixo
    if (bufferEntrada.length() > 25) { 
      pacotesERRO++; 
      bufferEntrada = ""; 
    }
  }

  // 2. Atualização de Tela Desacoplada (Apenas 4 vezes por segundo)
  if (millis() - ultimaTela >= 250) {
    ultimaTela = millis();
    atualizarTela();
  }
}