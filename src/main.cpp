#include <Arduino.h>
#define PIN_AO_CZ_DZWIEKU 34
#define PIN_POTENCJOMETR 35
#define PIN_DS_REJESTR 14
#define PIN_STCP_REJESTR 18
#define PIN_SHCP_REJESTR 5
#define PIN_PIN13_MATRYCA 32
#define PIN_PIN3_MATRYCA 33
#define PIN_PIN4_MATRYCA 25
#define PIN_PIN10_MATRYCA 26
#define PIN_PIN6_MATRYCA 27
#define PIN_PIN11_MATRYCA 16
#define PIN_PIN15_MATRYCA 17
#define PIN_PIN16_MATRYCA 19
unsigned long poprzednie_wskazanie_czasu=0;
const byte serce[8] = {
  0b00000000,
  0b00101000,
  0b01010100,
  0b01010100,
  0b01000100,
  0b00101000,
  0b00010000,
  0b00000000
};
const int piny_kolumn[8]={32,33,25,26,27,16,17,19};

void setup() {
  pinMode(PIN_AO_CZ_DZWIEKU, INPUT);
  pinMode(PIN_POTENCJOMETR, INPUT);
  pinMode(PIN_DS_REJESTR, OUTPUT);
  pinMode(PIN_STCP_REJESTR, OUTPUT);
  pinMode(PIN_SHCP_REJESTR, OUTPUT);
  pinMode(PIN_PIN13_MATRYCA, OUTPUT);
  pinMode(PIN_PIN3_MATRYCA, OUTPUT);
  pinMode(PIN_PIN4_MATRYCA, OUTPUT);
  pinMode(PIN_PIN10_MATRYCA, OUTPUT);
  pinMode(PIN_PIN6_MATRYCA, OUTPUT);
  pinMode(PIN_PIN11_MATRYCA, OUTPUT);
  pinMode(PIN_PIN15_MATRYCA, OUTPUT);
  pinMode(PIN_PIN16_MATRYCA, OUTPUT);
  for (int i=0; i<=7; i++){
    digitalWrite(piny_kolumn[i], HIGH);
  }
  Serial.begin(115200);
}

void loop() {
  //unsigned long aktualne_wskazanie_czasu =millis();
  //if (aktualne_wskazanie_czasu-poprzednie_wskazanie_czasu>1){
  //int wskazanie_potencjometru=analogRead(PIN_POTENCJOMETR);
  //int wskazanie_czujnika_dzwieku=analogRead(PIN_AO_CZ_DZWIEKU);
  // Format wymagany przez rozszerzenie Teleplot
  //Serial.printf(">Potencjometr:%d\n", wskazanie_potencjometru);
  //Serial.printf(">Dzwiek:%d\n", wskazanie_czujnika_dzwieku);
  /*bede przechodzic w petli for po wierszach przesuwajac ta jedynke czyli od 0b10000000 do 0b00000001 i wewnatrz tej petli patrzec na 
  ktorym miejscu jest jedynka i ten element tablicy serce analizowac, domyslnie wszytskie piny kolumn sa ustawione na high, ale przechodzimy 
  w kolejnej petli wewnetrznej for przechodzimy przez kolejne elementy danego elementu tablicy serce i jak napotkamy 1 to ustawiamy low na tym 
  numerze elementu z tablicy pinow kolumn na ktorym jest 1*/
  byte bit_dla_rejestru_domyslny =0b10000000;
  for (int i=0; i<=7; i++){
    byte bit_dla_rejestru=bit_dla_rejestru_domyslny >> i;
    for (int k=7; k>=0; k--){
      if (bitRead(serce[i], k)==1){
        digitalWrite(piny_kolumn[7-k], LOW);
      }
      else{
        digitalWrite(piny_kolumn[7-k], HIGH);
      }
    }
  digitalWrite(PIN_STCP_REJESTR, LOW);
  shiftOut(PIN_DS_REJESTR, PIN_SHCP_REJESTR, LSBFIRST, bit_dla_rejestru);
  digitalWrite(PIN_STCP_REJESTR, HIGH);
  delay(10);
}
  //poprzednie_wskazanie_czasu=aktualne_wskazanie_czasu;
}



