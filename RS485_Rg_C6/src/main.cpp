#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

// === MAPEAMENTO DE PINOS ===
#define SDA_PIN 23
#define SCL_PIN 22
#define BOTAO 13
#define LED_EXT_VERDE 21
#define LED_EXT_VERMELHO 20
#define RGB_NATIVO 8 // Pino interno padrão do LED RGB no C6 DevKit

// === OBJETOS DE HARDWARE ===
Adafruit_SSD1306 display(128, 64, &Wire, -1);
Adafruit_NeoPixel rgb(1, RGB_NATIVO, NEO_GRB + NEO_KHZ800);

// === VARIÁVEIS DE CONTROLE (MÁQUINA DE ESTADOS) ===
bool modoAtivo = false; // false = Standby, true = Operando

// Controle de Tempo do RGB (Sem usar delay)
unsigned long tempoAnteriorBlink = 0;
bool estadoBlink = false;

// Controle do Botão (Filtro Debounce Anti-Ruído)
int estadoBotao;
int ultimoEstadoBotao = HIGH;
unsigned long tempoUltimoDebounce = 0;
const unsigned long tempoDebounce = 50; 

// Função que só é chamada quando o botão é apertado
void atualizarIHM() {
  // 1. Atualiza LEDs Externos
  digitalWrite(LED_EXT_VERDE, modoAtivo ? HIGH : LOW);
  digitalWrite(LED_EXT_VERMELHO, modoAtivo ? LOW : HIGH);

  // 2. Atualiza a Tela OLED
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println("SISTEMA MESTRE");
  
  display.setCursor(0, 30);
  display.setTextSize(2); // Fonte grande para destacar
  
  if(modoAtivo) {
    display.println("OPERANDO");
  } else {
    display.println("STANDBY");
  }
  
  display.display();
}

void setup() {
  Serial.begin(115200);
  
  // Configura pinos
  pinMode(BOTAO, INPUT_PULLUP);
  pinMode(LED_EXT_VERDE, OUTPUT);
  pinMode(LED_EXT_VERMELHO, OUTPUT);

  // Inicia Tela
  Wire.begin(SDA_PIN, SCL_PIN);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Erro na tela OLED!");
    for(;;);
  }

  // Inicia LED RGB Embutido
  rgb.begin();
  rgb.setBrightness(30); // Limita o brilho (0-255) para não cegar ninguem
  rgb.show(); // Desliga os pixels no inicio

  // Desenha a tela inicial
  atualizarIHM(); 
}

void loop() {
  // ==========================================
  // TAREFA 1: LEITURA DO BOTÃO (Com Debounce)
  // ==========================================
  int leitura = digitalRead(BOTAO);
  if (leitura != ultimoEstadoBotao) {
    tempoUltimoDebounce = millis();
  }
  
  if ((millis() - tempoUltimoDebounce) > tempoDebounce) {
    if (leitura != estadoBotao) {
      estadoBotao = leitura;
      
      // Se o botão foi apertado (foi para LOW)
      if (estadoBotao == LOW) { 
        modoAtivo = !modoAtivo; // Inverte o modo atual
        atualizarIHM();         // Aciona a troca de tela e LEDs externos
      }
    }
  }
  ultimoEstadoBotao = leitura;

  // ==========================================
  // TAREFA 2: PISCAR O LED RGB A CADA 500ms
  // ==========================================
  if (millis() - tempoAnteriorBlink >= 500) {
    tempoAnteriorBlink = millis(); // Reseta o cronômetro
    estadoBlink = !estadoBlink;    // Inverte a chave do pisca

    if (estadoBlink) {
      // Hora de acender! Qual cor?
      if (modoAtivo) {
        rgb.setPixelColor(0, rgb.Color(0, 255, 0)); // Verde puro
      } else {
        rgb.setPixelColor(0, rgb.Color(255, 0, 0)); // Vermelho puro
      }
    } else {
      // Hora de apagar!
      rgb.setPixelColor(0, rgb.Color(0, 0, 0)); 
    }
    rgb.show(); // Manda o comando para o hardware do RGB
  }
}