#include <Arduino.h>

//#define EXERCICIO_1 // Piscar todos
#define EXERCICIO_2 // Contador Binário 
//#define EXERCICIO_3 // Contador 7 Seg

// Exercicio 1 e 2
const int allPins[] = {15, 2, 4, 16, 17, 26, 27, 14, 5, 18, 19};
const int numPins = sizeof(allPins) / sizeof(allPins[0]);
const int ledPin[] = {15, 2, 4, 16, 17, 5, 18, 19};
const int numLeds = sizeof(ledPin) / sizeof(ledPin[0]);

#define BlinkDelay 300
#define DisplayDelay 500

const byte mapaNumeros[] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

void setup() {
  for(int x = 0; x < numPins; x++){
    pinMode(allPins[x],OUTPUT);    
  }
}

void AtivaLed(int i){
  if(i == 0){
    // Ativa RGB
    digitalWrite(allPins[5], HIGH);// RGB
    digitalWrite(allPins[6], LOW); // seven_1
    digitalWrite(allPins[7], LOW); // seven_2
  } else if(i == 1){
    // Ativa Display 1
    digitalWrite(allPins[5], LOW);// RGB
    digitalWrite(allPins[6], HIGH); // seven_1
    digitalWrite(allPins[7], LOW); // seven_2
  } else {
    // Ativa Display 2
    digitalWrite(allPins[5], LOW);// RGB
    digitalWrite(allPins[6], LOW); // seven_1
    digitalWrite(allPins[7], HIGH); // seven_2

  }
}

void setarTodosLeds(int estado, int Delay) {
    for(int x = 0; x < numLeds; x++) {
        digitalWrite(ledPin[x], estado);
    }
    delay(Delay);
}

void setar5Leds(int estado, int Delay) {
    for(int x = 0; x < 5; x++) {
        digitalWrite(ledPin[x], estado);
    }
    delay(Delay);
}

void PiscarTodos(int QualLed, int QuantasVezes, int Delay) {
  
  AtivaLed(QualLed);
  delay(10);

  for(int i = 0; i < QuantasVezes ; i++){
    setarTodosLeds(HIGH, Delay);
    setarTodosLeds(LOW, Delay);
  }
  
}

void Piscar5Leds(int QuantasVezes, int Delay) {
  
  AtivaLed(0);
  delay(10);

  for(int i = 0; i < QuantasVezes ; i++){
    setar5Leds(HIGH, Delay);
    setar5Leds(LOW, Delay);
  }
  
}

void SingleDisplayShow(int i, int who){

  for(int j = 0; j < 7; j++){
    int valorAtual = bitRead(mapaNumeros[i],j);
    digitalWrite(ledPin[j], valorAtual);
  }
  
  AtivaLed(who);
  delay(5);
  // // Parte apenas para o simulador
  //digitalWrite(allPins[6], LOW); // seven_1
  //digitalWrite(allPins[7], LOW); // seven_2

}

void DualDisplayShow(int i, int Delay){
  unsigned long time = millis();
  int FirstNumber = i/10;
  int SecondNumber = i%10;

  while((millis() - time) < Delay){
     SingleDisplayShow(FirstNumber, 1);
     SingleDisplayShow(SecondNumber, 2);
  }
}

void Exercicio1() {
     PiscarTodos(0, 1, BlinkDelay);
     PiscarTodos(1, 1, DisplayDelay);
     PiscarTodos(2, 1, DisplayDelay);
}

void Exercicio2() {
  // Ativa leds
  AtivaLed(0);

  for (int contador = 0; contador <= 15; contador++) {
    for (int i = 0; i < 4; i++) {
      // Lê o bit na posição 'i' do número 'contador'
      int EstadoBitAtual = bitRead(contador, i);
      // Envia para o LED. 
      // Usamos '3 - i' para garantir que o bit 0 fique no led[3] (Direita)
      digitalWrite(ledPin[4 - i], EstadoBitAtual);
    }
    delay(BlinkDelay); // 300ms de intervalo entre cada número
  }

  Piscar5Leds(3, DisplayDelay);
}

// Contar de 0 a 99
void Exercicio3() {
  int ContarAte = 99;
  for(int i = 0; i <= ContarAte; i++){
    DualDisplayShow(i, BlinkDelay);
  }

  Piscar5Leds(3, DisplayDelay);
}

void loop() {
  #ifdef EXERCICIO_1
    Exercicio1();
  #elif defined(EXERCICIO_2)
    Exercicio2();
  #elif defined(EXERCICIO_3)
    Exercicio3();
  #endif
}