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
const int piny_kolumn[8]={32,33,25,26,27,16,17,19};
int wysokosci_kolumn[8]={0,0,0,0,0,0,0,0};

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
/* 
Wskaźnik wysterowania z potencjometrem - dynamiczna zmiana obrazu na matrycy w zależności od wejścia analogowego z potencjometru
A: wyswietlanie wiersz po wierszu (budowanie do gory), zaqpalone w kazdym wierszu wszystkie kolumny, tak aby ostateczna wysokosc prostokata
odpowiadala wysokosci tonu
B"

*/
void loop() {
  unsigned long aktualne_wskazanie_czasu =millis();
  if (aktualne_wskazanie_czasu-poprzednie_wskazanie_czasu>100){
  int aktualne_wskazanie_potencjometru=analogRead(PIN_POTENCJOMETR);
  Serial.printf (">Potencjometr: %d\n", aktualne_wskazanie_potencjometru);
  int wysokosc_slupka=map(aktualne_wskazanie_potencjometru, 0, 4095, 0, 8);
  for (int i=7; i>0; i--){
    wysokosci_kolumn[i]=wysokosci_kolumn[i-1];
  }
  wysokosci_kolumn[0]=wysokosc_slupka;
  poprzednie_wskazanie_czasu=aktualne_wskazanie_czasu;
  }
  // wysokosci kolumn= {1,3,5,4,3,0,1,0}
  byte bit_domyslny=0b00000000;
  int najwiekszy_slupek=0;
  for (int i=0; i<=7; i++){
    if (wysokosci_kolumn[i]>najwiekszy_slupek){
      najwiekszy_slupek=wysokosci_kolumn[i];
    }
  }
  for (int k=0; k<najwiekszy_slupek; k++) //0,1,2,3,4
  //k=0 to pierwszy wiersz
  {
    for (int i=0; i<=7; i++){
    digitalWrite(piny_kolumn[i], HIGH);
    }
    byte bajt_do_wyslania=(bit_domyslny|1)<<k;
    digitalWrite(PIN_STCP_REJESTR, LOW);
    shiftOut(PIN_DS_REJESTR, PIN_SHCP_REJESTR, LSBFIRST, bajt_do_wyslania);
    digitalWrite(PIN_STCP_REJESTR, HIGH);
    for (int i=0; i<=7; i++){
      if (wysokosci_kolumn[i]>k){
        digitalWrite(piny_kolumn[i], LOW);
        }
      else{
        digitalWrite(piny_kolumn[i], HIGH);
        }
    }
    delay(2);
  }
digitalWrite(PIN_STCP_REJESTR, LOW);
shiftOut(PIN_DS_REJESTR, PIN_SHCP_REJESTR, LSBFIRST, bit_domyslny);
digitalWrite(PIN_STCP_REJESTR, HIGH);
}





