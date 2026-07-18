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
Zbierasz próbki z mikrofonu za pomocą przetwornika ADC, a następnie wykorzystujesz algorytm FFT, aby rozbić falę dźwiękową na poszczególne 
pasma częstotliwości (basy, tony średnie, soprany). Wynik wyświetlasz jako dynamicznie skaczące słupki na swojej matrycy LED 8x8.
A: wyswietlanie wiersz po wierszu (budowanie do gory), zaqpalone w kazdym wierszu wszystkie kolumny, tak aby ostateczna wysokosc prostokata
odpowiadala wysokosci tonu*/
void loop() {
  int wskazanie_potencjometru=analogRead(PIN_POTENCJOMETR);
  Serial.printf (">Potencjometr: %d\n", wskazanie_potencjometru);
  byte bit_domyslny=0b00000000;
  int wysokosc_slupka=map(wskazanie_potencjometru, 0, 4095, 0, 8);
  if (wysokosc_slupka != 0) {
      for (int i=0; i<=7; i++){
    digitalWrite(piny_kolumn[i], LOW);
  }
        for (int i = 0; i < wysokosc_slupka; i++) {
            uint8_t bit_domyslny_do_wyslania = (bit_domyslny | 1)<<i;
            digitalWrite(PIN_STCP_REJESTR, LOW);
            shiftOut(PIN_DS_REJESTR, PIN_SHCP_REJESTR, LSBFIRST, bit_domyslny_do_wyslania);
            digitalWrite(PIN_STCP_REJESTR, HIGH);
            delay(2);
  }
  }
  digitalWrite(PIN_STCP_REJESTR, LOW);
  shiftOut(PIN_DS_REJESTR, PIN_SHCP_REJESTR, LSBFIRST, bit_domyslny);
  digitalWrite(PIN_STCP_REJESTR, HIGH);
  }



  //poprzednie_wskazanie_czasu=aktualne_wskazanie_czasu;
//}



