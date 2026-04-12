#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Botões
#define BOT_CIMA    32 
#define BOT_BAIXO   34 
#define BOT_ESQ     39 
#define BOT_DIR     35 

// RGB e Transistores
#define PINO_TLED       26 
#define PINO_TLEDseven1 27 
#define PINO_TLEDseven2 14 
#define LED_R           5
#define LED_G           18
#define LED_B           19

enum EstadoSistema { MENU_1, MENU_2_PASSO_1, MENU_2_PASSO_2, MENU_APAGADO };
EstadoSistema estadoAtual = MENU_1;
EstadoSistema estadoSalvo = MENU_1;
bool precisaAtualizarTela = true;

unsigned long tempoAnteriorRGB = 0;
int indiceCor = 0;

void setCorRGB(int r, int g, int b) {
  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);
}

void atualizarRGB() {
  unsigned long tempoAtual = millis();
  int intervalo = (estadoAtual == MENU_2_PASSO_2) ? 400 : 700;

  if (estadoAtual == MENU_APAGADO) {
    setCorRGB(255, 255, 255);
    return;
  }

  if (tempoAtual - tempoAnteriorRGB >= intervalo) {
    tempoAnteriorRGB = tempoAtual;
    indiceCor = (indiceCor + 1) % 4;

    if (estadoAtual == MENU_1 || estadoAtual == MENU_2_PASSO_1) {
      if(indiceCor == 0) setCorRGB(255, 0, 0);   // Vermelho
      else if(indiceCor == 1) setCorRGB(0, 255, 0); // Verde
      else if(indiceCor == 2) setCorRGB(0, 0, 255); // Azul
      else setCorRGB(255, 255, 0);                 // Amarelo
    } 
    else if (estadoAtual == MENU_2_PASSO_2) {
      if(indiceCor == 0) setCorRGB(255, 128, 0); // Laranja
      else if(indiceCor == 1) setCorRGB(128, 0, 128); // Roxo
      else if(indiceCor == 2) setCorRGB(255, 0, 255); // Magenta
      else setCorRGB(0, 255, 255);                 // Ciano
    }
  }
}

void desenharTela() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (estadoAtual == MENU_1) {
    display.setCursor(0, 10); display.println("UTFPR Curitiba");
    display.setCursor(0, 30); display.println("Samuel - 27 anos");
  } 
  else if (estadoAtual == MENU_2_PASSO_1) {
    display.setCursor(0, 10); display.println("Eng. Mecatronica");
    display.setCursor(0, 30); display.println("Samuel - 27 anos");
  } 
  else if (estadoAtual == MENU_2_PASSO_2) {
    display.setCursor(0, 10); display.println("Eng. Mecatronica");
    display.setCursor(0, 30); display.println("Sistemas");
    display.setCursor(0, 45); display.println("Microcontrolados");
  } 
  else if (estadoAtual == MENU_APAGADO) {
    display.setCursor(25, 25); display.println("Menu Apagado");
  }
  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Espera o Serial estabilizar
  Serial.println("--- PAT32 OS BOOTING ---");

  pinMode(BOT_CIMA, INPUT);
  pinMode(BOT_BAIXO, INPUT);
  pinMode(BOT_ESQ, INPUT);
  pinMode(BOT_DIR, INPUT);

  pinMode(PINO_TLED, OUTPUT);
  pinMode(PINO_TLEDseven1, OUTPUT);
  pinMode(PINO_TLEDseven2, OUTPUT);
  
  digitalWrite(PINO_TLED, HIGH);   // Habilita RGB
  digitalWrite(PINO_TLEDseven1, LOW); // Mata os 7 seg
  digitalWrite(PINO_TLEDseven2, LOW); // Mata os 7 seg

  // Força os pinos SDA/SCL (21 e 22) antes de iniciar o display
  Serial.println("Iniciando I2C...");
  Wire.begin(21, 22);

  Serial.println("Iniciando OLED...");
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("ERRO CRITICO: SSD1306 nao encontrado!"));
    // Em vez de travar, vamos tentar seguir sem tela para ver se o LED funciona
  } else {
    Serial.println("OLED OK!");
    display.clearDisplay();
    display.display();
  }
}

void loop() {
  // Leitura com Serial para Debug
  if (digitalRead(BOT_BAIXO) == HIGH) {
    Serial.println("Botao Baixo Pressionado");
    if (estadoAtual == MENU_1) estadoAtual = MENU_2_PASSO_1;
    else if (estadoAtual == MENU_2_PASSO_1) estadoAtual = MENU_2_PASSO_2;
    precisaAtualizarTela = true;
    delay(300); 
  }

  if (digitalRead(BOT_CIMA) == HIGH) {
    Serial.println("Botao Cima Pressionado");
    if (estadoAtual != MENU_APAGADO) estadoAtual = MENU_1;
    precisaAtualizarTela = true;
    delay(300);
  }

  if (digitalRead(BOT_ESQ) == HIGH) {
    Serial.println("Botao Esquerda Pressionado");
    if (estadoAtual != MENU_APAGADO) {
      estadoSalvo = estadoAtual;
      estadoAtual = MENU_APAGADO;
      precisaAtualizarTela = true;
    }
    delay(300);
  }

  if (digitalRead(BOT_DIR) == HIGH) {
    Serial.println("Botao Direita Pressionado");
    if (estadoAtual == MENU_APAGADO) {
      estadoAtual = estadoSalvo;
      precisaAtualizarTela = true;
    }
    delay(300);
  }

  if (precisaAtualizarTela) {
    desenharTela();
    precisaAtualizarTela = false;
  }

  atualizarRGB();
}