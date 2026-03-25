#include <Arduino.h>

//#define EXERCICIO_1 // DESCOMENTE ESTA LINHA PARA RODAR O EXERCÍCIO 1
//#define EXERCICIO_2_1 // DESCOMENTE ESTA LINHA PARA RODAR O EXERCÍCIO 2_1 
#define EXERCICIO_2_2 // DESCOMENTE ESTA LINHA PARA RODAR O EXERCÍCIO 2_2

// Exercicio 1 e 2
const int allPins[] = {15, 2, 4, 16, 17, 26, 27, 14};
const int numPins = sizeof(allPins) / sizeof(allPins[0]);
const int ledPin[] = {15, 2, 4, 16, 17};
const int numLeds = sizeof(ledPin) / sizeof(ledPin[0]);

// Outra forma possível
//const int ordemEfeito[] = {0, 1, 2, 3, 4, 3, 2, 1};

// Para o exercicio 2_1
int pinoAtual = 0;
int direcao = 1; // 1 para subir, -1 para descer

#define BlinkDelay 300

void setup() {

  for(int x = 0; x < numPins; x++){
    pinMode(allPins[x],OUTPUT);    
  }

  digitalWrite(allPins[5], HIGH);
  digitalWrite(allPins[6], LOW);
  digitalWrite(allPins[7], LOW);
}

void PiscaLed(int i){
      digitalWrite(ledPin[i], HIGH);
      delay(BlinkDelay);
      digitalWrite(ledPin[i], LOW);
}

void setarTodosLeds(int estado) {
    for(int x = 0; x < numLeds; x++) {
        digitalWrite(ledPin[x], estado);
    }
    delay(BlinkDelay);
}

void Exercicio1() {
    // Liga e desliga todos os leds ao mesmo tempo
    setarTodosLeds(HIGH);
    setarTodosLeds(LOW);
}

// Duas formas, a primeira
void Exercicio2_1() {
    // Liga e desliga os leds em sequencia
    for(int x=0;x<numLeds;x++){
      PiscaLed(x);
    }
    
    for(int x=(numLeds-2);x>0;x--){
      PiscaLed(x);
    }
}
// a segunda forma

void Exercicio2_2() {
    PiscaLed(pinoAtual);

    // Inverte a direção se chegar no último ou no primeiro
    if (pinoAtual == numLeds -1) direcao = -1;
    else if (pinoAtual == 0) direcao = 1;

    pinoAtual += direcao; // Move para o proximo pino
}

void loop() {
  #ifdef EXERCICIO_1
    Exercicio1();
  #elif defined(EXERCICIO_2_1)
    Exercicio2_1();
  #elif defined(EXERCICIO_2_2)
    Exercicio2_2();
  #endif  
}