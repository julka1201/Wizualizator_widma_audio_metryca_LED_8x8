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
struct LiczbaZespolona {
    double rzeczywista; // to bedzie w calosci napisane
    double urojona=0; //a to roznie wiec lepiej zainicjowac chyba
};
const int liczba_probek=64;
float probki_zebrane[liczba_probek];
LiczbaZespolona tablica_wartosci_poprzestawianych_bitami[liczba_probek];
int fs=8000;
int indeks=0;
int rozdzielczosc=fs/liczba_probek;
float odstep_czasowy=1.0/fs;
float sinusy[liczba_probek/2]={0};
float cosinusy[liczba_probek/2]={0};
/*float odstep_czasowy = 1 / 8000;. W języku C++ dzielenie dwóch liczb całkowitych daje w wyniku liczbę całkowitą, więc procesor zignoruje 
ułamek i zapisze tam okrągłe 0.0. Żeby zmusić go do ułamków, musisz zapisać liczby z kropką dziesiętną (np. 1.0).*/
const int liczba_kolumn_matrycy=8;
float granice_dla_poszczegolnych_kolumn[liczba_kolumn_matrycy]; //granice czestotliwosci, wiec tez ile slupkow z widma brac pod uwage do kazdej kolumny
int pierwszy_uzyteczny_indeks_FFT=1;
void bit_reversal (int ilosc_probek, byte* tab_bitow_po_odwroceniu){
  int na_ilu_bitach=log2(ilosc_probek);
  byte tab_bitow_przed_odwroceniem[ilosc_probek];
  byte maska =(0b00000001<<na_ilu_bitach)-1;// da najwieksza liczbe jaka mozna zapisac na tylu bitach ile chemy docelowo np 00000111 jesli na 3
  for (int i=0; i<ilosc_probek; i++){
    tab_bitow_przed_odwroceniem[i]=maska&i;
  }

  //byte tab_bitow_po_odwroceniu[ilosc_probek];
  /*tab_bitow_przed_odwroceniem[ilosc_probek]={0};. Ponieważ ilosc_probek jest zmienną, kompilator widzi tu tablicę o zmiennym rozmiarze 
  i zabroni dodania na końcu ={0}.Rozwiązanie: Zadeklaruj tablice bez ={0}. Zauważ, że i tak w pętlach nadpisujesz każdy jeden element w tych 
  tablicach od nowa, więc początkowe zerowanie ich przy tworzeniu nie jest Ci w ogóle potrzebne.
  Jeśli planujesz na każdej jednej stronie nakleić nową, dużą naklejkę, to zmazywanie starego tekstu gumką to strata czasu. 
  I tak wszystko dokładnie zakryjesz!*/

      for (int i=0; i<ilosc_probek; i++){
        byte analizowany_bajt=tab_bitow_przed_odwroceniem[i];
        byte odwrocony_bajt=0;
        for (int j=0; j<na_ilu_bitach; j++){
          odwrocony_bajt=odwrocony_bajt<<1; // odwrocony_bajt<<=1;
          odwrocony_bajt|=analizowany_bajt&1;
          analizowany_bajt>>=1;
        }
        tab_bitow_po_odwroceniu[i]=odwrocony_bajt; 
  }
}
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
  float kat_bazowy =(2*PI)/liczba_probek;
  for (int potega=0; potega<32; potega++){
    float kat=potega*kat_bazowy;
    cosinusy[potega]=cos(kat);
    sinusy[potega]=sin(kat);
  }

  int ostatni_uzyteczny_indeks_FFT=32;
  int ostatnia_szerokosc=0;
  for ( int i=0; i<liczba_kolumn_matrycy; i++){
    granice_dla_poszczegolnych_kolumn[i]=(pierwszy_uzyteczny_indeks_FFT*pow(ostatni_uzyteczny_indeks_FFT/pierwszy_uzyteczny_indeks_FFT, (i+1.0)/liczba_kolumn_matrycy))-ostatnia_szerokosc;
    ostatnia_szerokosc+=granice_dla_poszczegolnych_kolumn[i];
  }
  
}

void loop() {
unsigned long aktualne_wskazanie_czasu=micros();
if (aktualne_wskazanie_czasu-poprzednie_wskazanie_czasu>=125){
probki_zebrane[indeks]=analogRead(PIN_AO_CZ_DZWIEKU);
indeks++;
poprzednie_wskazanie_czasu=aktualne_wskazanie_czasu;
}
if (indeks==64){
byte tab_bitow_po_odwroceniu[liczba_probek];
bit_reversal(liczba_probek, tab_bitow_po_odwroceniu);

for (int i=0; i<liczba_probek; i++){
  int indeks_w_tablicy_docelowej=tab_bitow_po_odwroceniu[i];
  tablica_wartosci_poprzestawianych_bitami[indeks_w_tablicy_docelowej].rzeczywista=probki_zebrane[i];
  tablica_wartosci_poprzestawianych_bitami[indeks_w_tablicy_docelowej].urojona=0; //zeby po prostu wyzerowac po poprzednim przejsciu loop
}
//L= wielkosc grupy na zerowym etapie
for (int L=2; L<=liczba_probek; L*=2){
  int skok_w_danej_grupie=L/2; //skok to tez liczba motylkow na danym etapie, ale motylki numeruje sie od 0
  int ilosc_grup_L_elementowych=liczba_probek/L;
  //glowna petla for musi dotyczyc czegos zwiazanego z iloscia grup na danym etapie

  for (int g=0; g<ilosc_grup_L_elementowych; g++){
    int indeks_poczatku_dla_kolejnej_grupy=(g*L);
    int m=0;
    for (int i=indeks_poczatku_dla_kolejnej_grupy; (i+skok_w_danej_grupie)<(L*(g+1)); i++){
      int potega=m*(liczba_probek/L);
      /*
      mnożenie dwóch liczb zespolonych wygląda tak:$$(a + bi) \cdot (c - di) = (ac + bd) + (bc - ad)i$$
      Przekładając to na Twój algorytm:
      $a$ = probka_B.rzeczywista
      $b$ = probka_B.urojona
      $c$ = cosinusy[potega]
      $d$ = sinusy[potega]
      */
      float probka_A_rzeczywista=tablica_wartosci_poprzestawianych_bitami[i].rzeczywista;
      float probka_A_urojona=tablica_wartosci_poprzestawianych_bitami[i].urojona;
      float probka_B_rzeczywista=tablica_wartosci_poprzestawianych_bitami[i+skok_w_danej_grupie].rzeczywista;
      float probka_B_urojona=tablica_wartosci_poprzestawianych_bitami[i+skok_w_danej_grupie].urojona;
      float B_przeskalowana_cz_rzeczywista=(probka_B_rzeczywista*cosinusy[potega])+(probka_B_urojona*sinusy[potega]);
      float B_przeskalowana_cz_urojona=(probka_B_urojona*cosinusy[potega])-(probka_B_rzeczywista*sinusy[potega]);
      float nowe_A_rzeczywiste=probka_A_rzeczywista+B_przeskalowana_cz_rzeczywista;
      float nowe_A_urojone=probka_A_urojona+B_przeskalowana_cz_urojona;
      float nowe_B_rzeczywiste=probka_A_rzeczywista-B_przeskalowana_cz_rzeczywista;
      float nowe_B_urojone=probka_A_urojona-B_przeskalowana_cz_urojona;
      tablica_wartosci_poprzestawianych_bitami[i].rzeczywista=nowe_A_rzeczywiste;
      tablica_wartosci_poprzestawianych_bitami[i].urojona=nowe_A_urojone;
      tablica_wartosci_poprzestawianych_bitami[i+skok_w_danej_grupie].rzeczywista=nowe_B_rzeczywiste;
      tablica_wartosci_poprzestawianych_bitami[i+skok_w_danej_grupie].urojona=nowe_B_urojone;
      m++;
    }
    
  }

  //wewnetrzna petla for musi dotyczyc czegos z indeksacja wewnatrz tych pomniejszych grup i operacji na nich, motyki tez
}
float widmo_tab[liczba_probek/2];
for (int i=0; i<liczba_probek/2; i++){
  float re=tablica_wartosci_poprzestawianych_bitami[i].rzeczywista;
  float im=tablica_wartosci_poprzestawianych_bitami[i].urojona;
  float widmo=sqrt((re*re)+(im*im));
  widmo_tab[i]=widmo;
}

/*
Węższe pasma (mniej prążków FFT) przypisujesz do pierwszych kolumn LED (basy), a coraz szersze pasma (więcej prążków FFT) wrzucasz do wyższych 
kolumn (soprany). Dzięki temu cała matryca będzie żywo reagować na muzykę.
$$f_n = f_{min} \cdot \left( \frac{f_{max}}{f_{min}} \right)^{\frac{n}{N}}$$Gdzie:
$f_n$ to górna częstotliwość $n$-tego przedziału.
$f_{min}$ to najniższa częstotliwość, od jakiej zaczynamy pomiary (np. 100 Hz).
$f_{max}$ to najwyższa częstotliwość, na jakiej kończymy (np. 8000 Hz).
$N$ to całkowita liczba słupków (przedziałów), na które chcesz podzielić dźwięk (np. 8).
$n$ to numer konkretnego słupka, którego granicę właśnie wyliczasz (od 1 do $N$).

Kiedy już wiesz, które indeksy wchodzą w skład danej kolumny, pojawia się pytanie: jak połączyć kilka liczb z FFT w jedną wartość słupka? 
Możesz wyciągnąć średnią, ale dużo lepszy efekt wizualny daje wyciągnięcie wartości maksymalnej z danej grupy. Dzięki temu słupek 
od razu reaguje na ostry dźwięk i nie jest "spłaszczany" przez sąsiednie, cichsze częstotliwości.
*/
int j=pierwszy_uzyteczny_indeks_FFT;
float widmo_tab_uporzadkowane[liczba_kolumn_matrycy];
for (int i=0; i<liczba_kolumn_matrycy; i++){
  float najwieksza_wartosc_z_danej_grupy=0;
  for (int k=j; k<j+granice_dla_poszczegolnych_kolumn[i]; k++){
    if (widmo_tab[k]>najwieksza_wartosc_z_danej_grupy){
      najwieksza_wartosc_z_danej_grupy=widmo_tab[k];
      widmo_tab_uporzadkowane[i]=najwieksza_wartosc_z_danej_grupy;
    }
  }
  j+=granice_dla_poszczegolnych_kolumn[i];
}

byte bit_bazowy=0b00000001;
float widmo_tab_do_wyslania[liczba_kolumn_matrycy];
int maksymalna_glosnosc_mikrofonu = 2000;
for (int i=0; i<liczba_kolumn_matrycy; i++){
  widmo_tab_do_wyslania[i]=map(widmo_tab_uporzadkowane[i], 0, maksymalna_glosnosc_mikrofonu, 0, 8);
}

for (int i=0; i<liczba_kolumn_matrycy; i++){
  for (int j=0; j<=7; j++){
    digitalWrite(piny_kolumn[j], HIGH);
  }
  byte bit_do_wyslania=bit_bazowy<<i;
  digitalWrite(PIN_STCP_REJESTR, LOW);
  shiftOut(PIN_DS_REJESTR, PIN_SHCP_REJESTR, LSBFIRST, bit_do_wyslania);
  digitalWrite(PIN_STCP_REJESTR, HIGH);
  for (int k=0; k<liczba_kolumn_matrycy; k++){
      if (widmo_tab_do_wyslania[k]>i){
        digitalWrite(piny_kolumn[k], LOW);
      }
      else {
        digitalWrite(piny_kolumn[k], HIGH);
      }
  }
  delay (2);
}
poprzednie_wskazanie_czasu=micros();
indeks=0;
}

}




