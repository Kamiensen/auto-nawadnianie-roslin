int wartosc_czujnika = 0;
int pin_czujnika = A0;                 //pod ten pin podlaczyc wejscie analogowe czujnika wilgotnosci
int pin_pot = A1;                      //pod ten pin podlaczyc potencjometr
int pin_pomp = 10;                     //pod ten pin podlaczyc pompke
int wilgotnosc_procent = 100;
int wartosc_pot;
int wilgotnosc_ustawiona;              //poczatkowa wartosc nastawcza wilgotnosci
int histereza = 10;                    //roznica od ustawionej wilgotnosci do zalaczenia pompki
String zera;
bool pompowanie = LOW;
int stan_diody;                        //stan: LOW/HIGH (wlaczona/wylaczona)
int liczba_diod = 7;
byte piny_diod[] = {3, 4, 5, 6, 7, 8, 9};  //pod te piny podlaczyc diody. pierwszy z pinow to dioda z prawej
volatile bool state = LOW;                  //potrzebne do funkcji przerwania

void setup() {
  for(int i = 0; i < liczba_diod; i++) {
    pinMode(piny_diod[i], OUTPUT);
  }
  pinMode(pin_pomp, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  Serial.begin(9600);
  attachInterrupt(0, przerwanie, LOW);
}

void loop() {
  wartosc_czujnika = analogRead(pin_czujnika);
  wilgotnosc_procent = map(wartosc_czujnika, 350, 1023, 100, 0);
  wartosc_pot = analogRead(pin_pot);
  wilgotnosc_ustawiona = map(wartosc_pot, 0, 1023, 0, 100);

  if(state == HIGH) {
    liczba_binarnie(wilgotnosc_ustawiona);
    delay(10);
    state = LOW;
  }

  if(wilgotnosc_procent < wilgotnosc_ustawiona - histereza) {
      pompowanie = HIGH;
    }
    
  if(wilgotnosc_procent < wilgotnosc_ustawiona && pompowanie == HIGH){
     liczba_binarnie(wilgotnosc_procent);
     delay(10);
  }

  if(wilgotnosc_procent >= wilgotnosc_ustawiona) {
    pompowanie = LOW;
  }
  
  digitalWrite(pin_pomp, pompowanie);
  gaszenie_diod(); 
}

void liczba_binarnie(int liczba) {
  String liczba_binarnie = String(liczba, BIN);
  int dlugosc_liczby_binarnej = liczba_binarnie.length();
  zera = "";
  for(int i = 0; i < liczba_diod - dlugosc_liczby_binarnej; i++) {
    zera = zera + "0";
  }
  liczba_binarnie = zera + liczba_binarnie;
    for(int i = 0, x = 1; i < liczba_diod; i++, x+=2) { 
      if(liczba_binarnie[i] == '0') stan_diody = LOW;
      if(liczba_binarnie[i] == '1') stan_diody = HIGH;
      digitalWrite(piny_diod[i] + liczba_diod - x, stan_diody);
    }
}

void gaszenie_diod() {
  for(int i = 0; i < liczba_diod; i++) {
    digitalWrite(piny_diod[i], LOW);
  }
}

void przerwanie() {
  state = !state;
}

