#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <avr/pgmspace.h>

#define TFT_CS     10
#define TFT_RST    8  
#define TFT_DC     9
#define PIN_BLK    6 

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

const byte pinX = A0;   
const byte pinY = A1;   
const byte pinSW = 2;   
const byte pinExit = 3; 

// --- MINIMALIST & MAT ARAYÜZ (UI) RENKLERİ ---
#define RENK_BEYAZ      0xE73C 
#define RENK_SIYAH      0x10A2 
#define RENK_KOYU_GRI   0x2145 
#define RENK_ACIK_GRI   0xBDD7 
#define RENK_KART_KOYU  0x31C7 
#define RENK_KART_ACIK  0xEF5D 
#define RENK_VURGU      0x741A 
#define RENK_TEHLIKE    0xD349 
#define RENK_ALTIN      0xDDB2 
#define RENK_PASIF      0x52AA 

// --- PASTEL & MAT UYGULAMA RENKLERİ ---
#define C_YILAN      0x6D4B 
#define C_UZAY       0x5B55 
#define C_PONG       0xFBAE 
#define C_KACIS      0xE4B4 
#define C_ZIPLA      0xF694 
#define C_YAKALA     0x7558 
#define C_FLAPPY     0xE511 
#define C_YARIS      0x9CB5 
#define C_LABIRENT   0x5E54 
#define C_MAYIN      0xC24A 
#define C_KAYA       0xA410 
#define C_DEFANS     0x43B1 
#define C_TETRIS     0xB2B5 
#define C_TUGLA      0xB32C 
#define C_NISANCI    0x7C54 
#define C_AYARLAR    0x632C 

bool ayarEkranTers = false; 
bool ayarTemaAcik = false; 
bool ayarDilEn = false;    

uint16_t bgRenk = RENK_KOYU_GRI; 
uint16_t kartRenk = RENK_KART_KOYU;
uint16_t txtRenk = RENK_BEYAZ; 

int8_t seciliUygulama = 0;   
int8_t oncekiSayfa = -1;      
const byte toplamUygulama = 16; 

unsigned long sonExitBasZamani = 0;

const char isimlerTR[16][9] PROGMEM = {"YILAN", "UZAY", "PONG", "KACIS", "ZIPLA", "YAKALA", "FLAPPY", "YARIS", "LABIRENT","MAYIN", "KAYA", "DEFANS", "TETRIS", "TUGLA", "NISANCI", "AYARLAR"};
const char isimlerEN[16][9] PROGMEM = {"SNAKE", "SPACE", "PONG", "DODGE", "JUMP", "CATCH", "FLAPPY", "RACING", "MAZE", "MINES", "ROCK", "DEFEND", "TETRIS", "BRICK", "SNIPER", "SETTINGS"};
const uint16_t uygulamaRenkleri[16] PROGMEM = { C_YILAN, C_UZAY, C_PONG, C_KACIS, C_ZIPLA, C_YAKALA, C_FLAPPY, C_YARIS, C_LABIRENT, C_MAYIN, C_KAYA, C_DEFANS, C_TETRIS, C_TUGLA, C_NISANCI, C_AYARLAR };

const uint16_t tetrisSekiller[7][4] PROGMEM = {
  {0x0F00, 0x2222, 0x0F00, 0x2222}, {0x0660, 0x0660, 0x0660, 0x0660}, {0x0E20, 0x44C0, 0x8E00, 0x6440},
  {0x0E80, 0xC440, 0x2E00, 0x4460}, {0x0E40, 0x4C40, 0x4E00, 0x4640}, {0x06C0, 0x8C40, 0x06C0, 0x8C40}, {0x0C60, 0x4C80, 0x0C60, 0x4C80}
};
const uint16_t tetrisRenkler[7] PROGMEM = {0x07FF, 0xFFE0, 0xF800, 0x001F, 0xF81F, 0x07E0, 0xFD20};

void bootEkrani() {
  tft.fillScreen(RENK_SIYAH);

  // Ana Cihaz Logosu 
  tft.setTextColor(RENK_ALTIN); 
  tft.setTextSize(2);
  tft.setCursor(26, 35); 
  tft.print(F("Comsole 1"));

  // Android tarzı "Powered by" yazısı 
  tft.setTextColor(RENK_ACIK_GRI); 
  tft.setTextSize(1);
  tft.setCursor(50, 100); 
  tft.print(F("Powered by"));

  // İşletim Sistemi Adı 
  tft.setTextColor(RENK_VURGU); 
  tft.setCursor(65, 112); 
  tft.print(F("ComUI"));

  // Açılış ekranı 3 saniye kalsın
  delay(4500); 
}

void kapatmaMenusu() {
  // Not yazısına yer açmak için kapatma menüsünü yukarı çektik
  tft.fillRoundRect(15, 10, 130, 60, 8, kartRenk);
  tft.drawRoundRect(15, 10, 130, 60, 8, RENK_TEHLIKE); 

  tft.setTextColor(txtRenk); tft.setTextSize(1);
  tft.setCursor(40, 20); 
  tft.print(ayarDilEn ? F("POWER OFF?") : F("KAPAT?"));

  // İstenilen Uyarı Notu - Rengi TEHLIKE (Kırmızı) ve Kalın (Bold) yapıldı
  tft.setTextColor(RENK_TEHLIKE);
  
  // KALINLAŞTIRMA HİLESİ: Aynı yazıyı yana 1 piksel kaydırarak tekrar çiziyoruz
  for(byte b=0; b<2; b++) { 
    if(ayarDilEn) {
      tft.setCursor(10+b, 78); tft.print(F("NOTE: Screen & systems"));
      tft.setCursor(10+b, 88); tft.print(F("turn off, but mainboard"));
      tft.setCursor(10+b, 98); tft.print(F("stays on in background."));
    } else {
      tft.setCursor(10+b, 74); tft.print(F("NOT: Bu sadece ekrani"));
      tft.setCursor(10+b, 84); tft.print(F("ve diger sistemleri"));
      tft.setCursor(10+b, 94); tft.print(F("kapatir, anakart arkada"));
      tft.setCursor(10+b, 104); tft.print(F("acik kalacaktir."));
    }
  }

  bool evetSecili = false, kararVerildi = false;
  delay(300); 

  while (!kararVerildi) {
    int jX = analogRead(pinX);
    if (jX < 300) { evetSecili = true; delay(150); }
    else if (jX > 800) { evetSecili = false; delay(150); }

    tft.fillRoundRect(25, 38, 45, 20, 4, evetSecili ? RENK_TEHLIKE : bgRenk);
    tft.setTextColor(evetSecili ? RENK_BEYAZ : txtRenk);
    tft.setCursor(35, 44); tft.print(ayarDilEn ? F("YES") : F("EVET"));

    tft.fillRoundRect(90, 38, 45, 20, 4, !evetSecili ? RENK_VURGU : bgRenk);
    tft.setTextColor(!evetSecili ? RENK_SIYAH : txtRenk);
    tft.setCursor(100, 44); tft.print(ayarDilEn ? F("NO") : F("HAYIR"));

    if (digitalRead(pinSW) == LOW) { kararVerildi = true; delay(300); }
  }

  if (evetSecili) {
    tft.fillScreen(RENK_SIYAH); 
    digitalWrite(PIN_BLK, LOW); 
    bool uyan = false;
    unsigned long sonUyanmaZamani = 0;
    while (!uyan) {
      if (digitalRead(pinExit) == LOW) {
        if (millis() - sonUyanmaZamani < 500 && sonUyanmaZamani != 0) uyan = true; 
        else sonUyanmaZamani = millis(); 
        delay(200); 
      }
    }
    digitalWrite(PIN_BLK, HIGH); 
    bootEkrani(); 
  }
  
  tft.fillScreen(bgRenk);
  oncekiSayfa = -1; 
}

void setup() {
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(pinExit, INPUT_PULLUP); 
  pinMode(PIN_BLK, OUTPUT);
  digitalWrite(PIN_BLK, HIGH); 
  
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(ayarEkranTers ? 3 : 1); 
  
  bootEkrani(); 
  tft.fillScreen(bgRenk);
}

void loop() {
  if (digitalRead(pinExit) == LOW) {
    if (millis() - sonExitBasZamani < 500 && sonExitBasZamani != 0) { 
      kapatmaMenusu();
      sonExitBasZamani = 0; 
    } else {
      sonExitBasZamani = millis(); 
    }
    delay(200); 
  }

  int xDegeri = analogRead(pinX), yDegeri = analogRead(pinY); 
  bool basildiSec = (digitalRead(pinSW) == LOW);

  int8_t yeniSecim = seciliUygulama;
  bool moved = true;
  if (xDegeri < 300) yeniSecim--;
  else if (xDegeri > 800) yeniSecim++;
  else if (yDegeri < 300) yeniSecim -= 3;
  else if (yDegeri > 800) yeniSecim += 3;
  else moved = false;
  
  if (moved) delay(150);

  if (yeniSecim < 0) yeniSecim = toplamUygulama - 1;
  if (yeniSecim >= toplamUygulama) yeniSecim = 0;

  int8_t aktifSayfa = yeniSecim / 6; 
  
  if (aktifSayfa != oncekiSayfa) {
    tft.fillRect(0, 15, 160, 113, bgRenk); 
    sayfaGostergesiniCiz(aktifSayfa); 
    oncekiSayfa = aktifSayfa;
    byte bitis = (aktifSayfa + 1) * 6;
    if(bitis > toplamUygulama) bitis = toplamUygulama;
    
    for (byte i = aktifSayfa * 6; i < bitis; i++) {
      kartCiz(i, i == yeniSecim);
      delay(35); 
    }
    seciliUygulama = yeniSecim;
    isimEtiketiCiz(seciliUygulama);
  } 
  else if (yeniSecim != seciliUygulama) {
    kartCiz(seciliUygulama, false); 
    seciliUygulama = yeniSecim;
    kartCiz(seciliUygulama, true);  
    isimEtiketiCiz(seciliUygulama); 
  }

  if (basildiSec) {
    for(byte b=0; b<2; b++) {
      kartCiz(seciliUygulama, false); delay(60);
      kartCiz(seciliUygulama, true); delay(60);
    }
    
    for(byte w=0; w<=80; w+=16) {
      tft.fillRect(0, 15, w, 113, RENK_SIYAH);      
      tft.fillRect(160-w, 15, w, 113, RENK_SIYAH);   
    }
    tft.fillScreen(RENK_SIYAH); 
    
    switch(seciliUygulama) {
      case 0: oynaYilan(); break;
      case 1: oynaUzay(); break;
      case 2: oynaPong(); break;
      case 3: oynaKacis(); break;
      case 4: oynaZipla(); break;
      case 5: oynaYakala(); break;
      case 6: oynaFlappy(); break;
      case 7: oynaYaris(); break;
      case 8: oynaLabirent(); break;
      case 9: oynaMayin(); break;
      case 10: oynaKaya(); break;
      case 11: oynaDefans(); break;
      case 12: oynaTetris(); break;
      case 13: oynaTugla(); break;
      case 14: oynaNisanci(); break;
      case 15: acAyarlar(); break;
    }
    
    tft.fillScreen(bgRenk);
    oncekiSayfa = -1; 
    delay(400);
  }
}

void sayfaGostergesiniCiz(byte sayfa) {
  tft.fillRect(50, 116, 60, 8, bgRenk); 
  for(byte i=0; i<3; i++) {
    if(i == sayfa) tft.fillRoundRect(62 + (i*12), 118, 8, 4, 2, txtRenk); 
    else tft.fillCircle(65 + (i*12), 120, 2, RENK_PASIF); 
  }
}

void isimEtiketiCiz(byte index) {
  tft.fillRect(0, 100, 160, 15, bgRenk); 
  char isim[9];
  strcpy_P(isim, ayarDilEn ? isimlerEN[index] : isimlerTR[index]);
  byte w = strlen(isim) * 6; 
  byte xPos = (160 - w) / 2;
  tft.fillRoundRect(xPos - 6, 99, w + 12, 11, 4, kartRenk);
  tft.setTextColor(txtRenk); tft.setTextSize(1); 
  tft.setCursor(xPos, 101); tft.print(isim);
}

void kartCiz(byte index, bool secili) {
  byte sut = index % 3, sat = (index % 6) / 3; 
  byte iX = 14 + (sut * 48), iY = 22 + (sat * 44);
  uint16_t cRenkUyg = pgm_read_word(&uygulamaRenkleri[index]);
  
  tft.fillRoundRect(iX, iY, 34, 34, 6, cRenkUyg);
  
  uint16_t cCerceve = secili ? RENK_VURGU : bgRenk;
  tft.drawRoundRect(iX - 2, iY - 2, 38, 38, 8, cCerceve);
  tft.drawRoundRect(iX - 3, iY - 3, 40, 40, 9, cCerceve);
  
  modernIkonCiz(index, iX, iY, cRenkUyg);
}

void modernIkonCiz(byte index, byte x, byte y, uint16_t bg) {
  byte cx = x + 17, cy = y + 17; uint16_t c = RENK_BEYAZ;
  switch(index) {
    case 0: tft.fillRect(x+8, y+10, 12, 6, c); tft.fillRect(x+14, y+16, 6, 6, c); tft.fillRect(x+14, y+22, 12, 6, c); break; 
    case 1: tft.fillTriangle(cx, y+6, x+10, y+20, x+24, y+20, c); tft.fillRect(x+14, y+20, 6, 6, bg); break; 
    case 2: tft.fillRect(x+6, y+6, 4, 10, c); tft.fillRect(x+24, y+18, 4, 10, c); tft.fillCircle(cx, cy, 3, c); break; 
    case 3: tft.fillRect(x+10, y+8, 6, 6, RENK_SIYAH); tft.fillRect(x+20, y+14, 6, 6, RENK_SIYAH); tft.fillTriangle(cx, y+26, x+10, y+18, x+24, y+18, c); break; 
    case 4: tft.fillRect(x+6, y+22, 10, 6, c); tft.fillRect(x+16, y+16, 10, 12, c); tft.fillCircle(x+11, y+10, 4, c); break; 
    case 5: tft.fillRect(x+10, y+18, 14, 10, c); tft.fillRect(x+12, y+18, 10, 6, bg); tft.fillCircle(cx, y+10, 3, RENK_SIYAH); break; 
    case 6: tft.fillCircle(x+14, cy, 6, c); tft.fillTriangle(x+18, cy-3, x+18, cy+3, x+26, cy, c); tft.fillCircle(x+14, cy-2, 2, bg); break; 
    case 7: tft.drawRect(x+10, y+6, 14, 22, c); tft.fillRect(x+14, y+18, 6, 8, RENK_SIYAH); break; 
    case 8: tft.drawRect(x+8, y+8, 18, 18, c); tft.drawLine(cx, y+8, cx, y+20, c); tft.drawLine(x+8, cy, x+20, cy, c); break; 
    case 9: tft.fillTriangle(cx, y+8, x+8, cy, cx, y+26, c); tft.fillTriangle(cx, y+8, x+26, cy, cx, y+26, c); tft.fillCircle(cx, cy, 3, bg); break; 
    case 10: tft.fillRoundRect(x+8, y+8, 18, 18, 4, RENK_SIYAH); tft.drawLine(x+12, y+8, x+22, y+26, c); break; 
    case 11: tft.fillTriangle(cx, y+26, x+8, y+14, x+26, y+14, c); tft.fillRect(x+8, y+8, 18, 6, c); break; 
    case 12: tft.fillRect(x+8, y+10, 18, 6, c); tft.fillRect(x+14, y+16, 6, 6, c); break; 
    case 13: tft.fillRect(x+8, y+8, 18, 6, RENK_SIYAH); tft.fillCircle(x+14, y+18, 3, c); tft.fillRect(x+12, y+24, 14, 4, c); break; 
    case 14: tft.drawCircle(cx, cy, 10, c); tft.drawLine(cx, y+4, cx, y+30, c); tft.drawLine(x+4, cy, x+30, cy, c); break; 
    case 15: tft.fillRect(x+8, y+12, 18, 3, c); tft.fillCircle(x+12, y+13, 4, bg); tft.fillRect(x+8, y+22, 18, 3, c); tft.fillCircle(x+22, y+23, 4, bg); break; 
  }
}

void gosterSistemBilgisi() {
  tft.fillScreen(RENK_SIYAH); 
  tft.drawRoundRect(5, 5, 150, 118, 6, RENK_VURGU); 
  
  tft.setTextColor(RENK_ALTIN); tft.setTextSize(2);
  tft.setCursor(26, 15); tft.print(F("Comsole 1")); 
  tft.drawLine(20, 35, 140, 35, RENK_PASIF);

  tft.setTextSize(1);
  tft.setTextColor(RENK_BEYAZ);
  tft.setCursor(15, 45); 
  
  tft.print(F("OS: ComUI 1.0\nDevs:\n- C.Uzunsoy\n- O.Peksoz\n- M.Isik"));

  delay(500); 
  while(digitalRead(pinSW) == HIGH && digitalRead(pinExit) == HIGH) { delay(10); } 
  delay(300); 
}

void acAyarlar() {
  int8_t ayarSecili = 0, ayarOnceki = -1; const byte toplamAyar = 5; bool ayarlarda = true; delay(300); 
  while(ayarlarda) {
    if (digitalRead(pinExit) == LOW) return; 

    int jY = analogRead(pinY); bool basildi = (digitalRead(pinSW) == LOW);
    if (jY < 300) { ayarSecili--; delay(200); } else if (jY > 800) { ayarSecili++; delay(200); }
    if (ayarSecili < 0) ayarSecili = toplamAyar - 1; if (ayarSecili >= toplamAyar) ayarSecili = 0;

    if (ayarSecili != ayarOnceki) {
      tft.fillScreen(bgRenk); 
      tft.fillRoundRect(10, 5, 140, 20, 4, kartRenk);
      tft.setTextColor(RENK_VURGU); tft.setTextSize(2);
      byte baslikW = (ayarDilEn ? 8 : 7) * 12;
      tft.setCursor((160-baslikW)/2, 8); tft.print(ayarDilEn ? F("SETTINGS") : F("AYARLAR"));
      tft.setTextSize(1);
      
      for(byte i=0; i<toplamAyar; i++) {
        byte yPos = 34 + (i * 17); 
        if(i == ayarSecili) { tft.fillRoundRect(10, yPos-2, 140, 14, 4, RENK_VURGU); tft.setTextColor(RENK_SIYAH); } 
        else { tft.fillRoundRect(10, yPos-2, 140, 14, 4, kartRenk); tft.setTextColor(txtRenk); }
        
        tft.setCursor(15, yPos+1); 
        switch(i) {
          case 0: tft.print(ayarDilEn ? F("ROTATION") : F("EKRAN YONU")); break;
          case 1: tft.print(ayarDilEn ? F("THEME") : F("TEMA")); break;
          case 2: tft.print(ayarDilEn ? F("LANGUAGE") : F("DIL")); break;
          case 3: tft.print(ayarDilEn ? F("SYS INFO") : F("SISTEM BILGI")); break;
          case 4: tft.setCursor(65, yPos+1); tft.print(ayarDilEn ? F("EXIT") : F("CIKIS")); break;
        }
        
        tft.setCursor(105, yPos+1); 
        switch(i) {
          case 0: tft.print(ayarEkranTers ? (ayarDilEn ? F("FLIP") : F("TERS")) : (ayarDilEn ? F("NORMAL") : F("NORMAL"))); break;
          case 1: tft.print(ayarTemaAcik ? (ayarDilEn ? F("LIGHT") : F("ACIK")) : (ayarDilEn ? F("DARK") : F("KOYU"))); break;
          case 2: tft.print(ayarDilEn ? F("EN") : F("TR")); break;
          case 3: tft.print(F("  >")); break;
        }
      }
      ayarOnceki = ayarSecili;
    }
    
    if (basildi) {
      if (ayarSecili == 4) ayarlarda = false; 
      else if (ayarSecili == 3) {
        gosterSistemBilgisi(); 
        ayarOnceki = -1; 
      }
      else {
        switch(ayarSecili) {
          case 0: ayarEkranTers = !ayarEkranTers; tft.setRotation(ayarEkranTers ? 3 : 1); break;
          case 1: ayarTemaAcik = !ayarTemaAcik; 
                  bgRenk = ayarTemaAcik ? RENK_ACIK_GRI : RENK_KOYU_GRI; 
                  kartRenk = ayarTemaAcik ? RENK_KART_ACIK : RENK_KART_KOYU; 
                  txtRenk = ayarTemaAcik ? RENK_SIYAH : RENK_BEYAZ; break;
          case 2: ayarDilEn = !ayarDilEn; break;
        }
        ayarOnceki = -1; 
      } delay(250); 
    }
  }
}

void oyunBitti(int skor) {
  tft.fillScreen(RENK_SIYAH); 
  tft.drawRoundRect(20, 30, 120, 60, 8, RENK_VURGU); 
  tft.setTextColor(RENK_BEYAZ); tft.setTextSize(2);
  tft.setCursor(26, 40); tft.print(F("GAME OVER")); 
  tft.setTextSize(1);
  tft.setCursor(45, 70); tft.print(F("SCORE: ")); tft.print(skor);
  delay(3000); 
}

void oynaYilan() {
  byte yX[30], yY[30], uz = 3, yon = 1, eX = 80, eY = 60; int skor = 0; bool aktif = true;
  for(byte i=0; i<uz; i++) { yX[i]=40-i*4; yY[i]=60; } 
  while(aktif) {
    if (digitalRead(pinExit) == LOW) return; 

    int jX = analogRead(pinX), jY = analogRead(pinY);
    if (jY > 800 && yon != 0) yon = 2; else if (jY < 300 && yon != 2) yon = 0; 
    else if (jX < 300 && yon != 1) yon = 3; else if (jX > 800 && yon != 3) yon = 1; 
    
    tft.fillRect(yX[uz-1], yY[uz-1], 4, 4, RENK_SIYAH); 
    for(byte i=uz-1; i>0; i--) { yX[i] = yX[i-1]; yY[i] = yY[i-1]; }
    if(yon==0) yY[0]-=4; if(yon==1) yX[0]+=4; if(yon==2) yY[0]+=4; if(yon==3) yX[0]-=4;
    
    if(yX[0]>160 || yY[0]>128) aktif = false; 
    for(byte i=1; i<uz; i++) { if(yX[0] == yX[i] && yY[0] == yY[i]) aktif = false; }
    
    if(abs(yX[0]-eX)<4 && abs(yY[0]-eY)<4) { if(uz<30) uz++; skor+=10; tft.fillRect(eX, eY, 4, 4, RENK_SIYAH); eX=random(10,150)/4*4; eY=random(10,110)/4*4; }
    
    tft.fillRect(eX, eY, 4, 4, RENK_TEHLIKE); 
    tft.fillRect(yX[1], yY[1], 4, 4, C_YILAN); 
    tft.fillRect(yX[0], yY[0], 4, 4, RENK_VURGU); 
    delay(80); 
  } oyunBitti(skor);
}

void oynaUzay() {
  byte gX=70, dX=random(10,140), dY=0; int8_t mX=-1, mY=-1; int skor=0; bool aktif=true; 
  while(aktif) {
    if (digitalRead(pinExit) == LOW) return; 

    int jX=analogRead(pinX); bool ates=(digitalRead(pinSW)==LOW);
    tft.fillRect(gX, 106, 15, 16, RENK_SIYAH); 
    
    if (jX<300 && gX>=4) gX-=4; if (jX>800 && gX<=141) gX+=4;
    if (ates && mY<0) { mX=gX+6; mY=110; }
    
    if (mY>=0) { tft.fillRect(mX, mY, 3, 6, RENK_SIYAH); mY-=8; if (mY<0) mY=-1; else tft.fillRect(mX, mY, 3, 6, RENK_ALTIN); }
    tft.fillRect(dX, dY, 12, 12, RENK_SIYAH); dY+=2+(skor/30); if (dY>120) aktif=false; 
    
    if (mY>0 && mY<dY+12 && mX>dX && mX<dX+12) { skor+=10; tft.fillRect(dX, dY, 12, 12, RENK_SIYAH); tft.fillRect(mX, mY, 3, 6, RENK_SIYAH); dY=0; dX=random(10, 140); mY=-1; }
    
    tft.fillRect(gX, 115, 15, 6, RENK_VURGU); tft.fillTriangle(gX+2, 115, gX+13, 115, gX+7, 107, RENK_VURGU);
    tft.fillRect(dX, dY, 12, 12, RENK_TEHLIKE); 
    delay(30);
  } oyunBitti(skor);
}

void oynaPong() {
  byte tX=80, tY=64, p1=50, p2=50; int8_t hX=3, hY=3; int skor=0; bool aktif=true; 
  while(aktif) {
    if (digitalRead(pinExit) == LOW) return; 

    int jY=analogRead(pinY);
    tft.fillRect(5, p1, 4, 24, RENK_SIYAH); tft.fillRect(151, p2, 4, 24, RENK_SIYAH); 
    tft.fillRect(tX-1, tY-1, 7, 7, RENK_SIYAH); 
    
    if (jY<300 && p1>=5) p1-=5; if (jY>800 && p1<=99) p1+=5;
    if (tY>p2+12 && p2<104) p2+=3; if (tY<p2+12 && p2>0) p2-=3;
    tX+=hX; tY+=hY; if (tY>=200 || tY>=123) hY=-hY;
    if (tX<=9 && tY+5>=p1 && tY<=p1+24) { hX=abs(hX); tX=10; skor+=10; } 
    if (tX>=146 && tX<200 && tY+5>=p2 && tY<=p2+24) { hX=-abs(hX); tX=145; }
    if (tX>160) aktif=false;
    
    tft.fillRect(5, p1, 4, 24, RENK_VURGU); tft.fillRect(151, p2, 4, 24, RENK_TEHLIKE); 
    tft.fillCircle(tX+2, tY+2, 3, RENK_ALTIN); delay(30);
  } oyunBitti(skor);
}

void oynaKacis() {
  byte pX=70, eX=random(10, 140), eY=0; int skor=0; bool aktif=true; 
  while(aktif) {
    if (digitalRead(pinExit) == LOW) return; 

    int jX=analogRead(pinX); tft.fillRect(pX, 110, 15, 8, RENK_SIYAH);
    if (jX<300 && pX>=6) pX-=6; if (jX>800 && pX<=139) pX+=6;
    tft.fillRect(eX, eY, 12, 12, RENK_SIYAH); eY+=4+(skor/5); if(eY>120) { eY=0; eX=random(10, 140); skor++; }
    if (eY+12>=110 && eX+12>pX && eX<pX+15) aktif=false;
    tft.fillRect(pX, 110, 15, 8, RENK_VURGU); tft.fillRect(eX, eY, 12, 12, RENK_TEHLIKE); delay(30);
  } oyunBitti(skor);
}

void oynaZipla() {
  int pY=900, hY=0, eX=150, eY=90, skor=0; bool aktif=true; 
  tft.drawLine(0, 105, 160, 105, RENK_PASIF); 
  while(aktif) {
    if (digitalRead(pinExit) == LOW) return; 

    bool btn=(digitalRead(pinSW)==LOW); int drawY = pY/10; tft.fillRect(30, drawY, 12, 15, RENK_SIYAH);
    if(btn && pY>=900) hY=-70; hY+=6; pY+=hY; if(pY>900) { pY=900; hY=0; } drawY = pY/10;
    
    tft.fillRect(eX, eY, 11, 16, RENK_SIYAH); 
    eX-=5+(skor/5); if(eX<0) { eX=150; skor++; }
    
    if(eX<42 && eX+10>30 && drawY+15>eY) aktif=false;
    tft.fillRect(30, drawY, 12, 15, RENK_VURGU); tft.fillTriangle(eX, eY+15, eX+5, eY, eX+10, eY+15, RENK_TEHLIKE); delay(30);
  } oyunBitti(skor);
}

void oynaYakala() {
  byte pX=70, eX=random(10, 140), eY=0; int skor=0; bool aktif=true; 
  while(aktif) {
    if (digitalRead(pinExit) == LOW) return; 

    int jX=analogRead(pinX); tft.fillRect(pX, 115, 30, 6, RENK_SIYAH);
    if (jX<300 && pX>=6) pX-=6; if (jX>800 && pX<=124) pX+=6;
    
    tft.fillRect(eX, eY, 7, 7, RENK_SIYAH); 
    eY+=4+(skor/25); 
    if (eY>=110) { if (eX+6>pX && eX<pX+30) { skor+=10; eY=0; eX=random(10, 140); } else if (eY>120) { aktif=false; } }
    tft.fillRect(pX, 115, 30, 6, RENK_VURGU); tft.fillCircle(eX+3, eY+3, 3, RENK_ALTIN); delay(30);
  } oyunBitti(skor);
}

void oynaFlappy() {
  int pY=600, hY=0, eX=160, bos=45, eY=random(20, 70), skor=0; bool aktif=true; 
  while(aktif) {
    if (digitalRead(pinExit) == LOW) return; 

    int drawY = pY/10; tft.fillRect(30, drawY, 10, 10, RENK_SIYAH);
    if(digitalRead(pinSW)==LOW && drawY>10) hY=-50; 
    hY+=5; pY+=hY; drawY = pY/10; if(drawY>120 || drawY<0) aktif=false;
    
    tft.fillRect(eX, 0, 15, eY, RENK_SIYAH); tft.fillRect(eX, eY+bos, 15, 128-(eY+bos), RENK_SIYAH);
    eX-=4; if(eX<-15) { eX=160; eY=random(20,70); skor++; }
    if(eX<40 && eX+15>30 && (drawY<eY || drawY+10>eY+bos)) aktif=false;
    
    tft.fillRect(30, drawY, 10, 10, RENK_ALTIN); tft.fillRect(eX, 0, 15, eY, C_FLAPPY); tft.fillRect(eX, eY+bos, 15, 128-(eY+bos), C_FLAPPY); delay(30);
  } oyunBitti(skor);
}

void oynaYaris() {
  byte serit=0, eY=0, eSer=random(0,2); int skor=0; bool aktif=true; 
  tft.drawLine(80,0,80,128,RENK_PASIF);
  while(aktif) {
    if (digitalRead(pinExit) == LOW) return; 

    int jX=analogRead(pinX); tft.fillRect(40+serit*80-12, 100, 24, 24, RENK_SIYAH);
    if(jX<300) serit=0; else if(jX>800) serit=1;
    tft.fillRect(40+eSer*80-12, eY, 24, 24, RENK_SIYAH); eY+=5+(skor/2); if(eY>128) { eY=0; eSer=random(0,2); skor++; }
    if(eY+24>100 && eY<124 && serit==eSer) aktif=false;
    tft.fillRect(40+serit*80-12, 100, 24, 24, RENK_VURGU); tft.fillRect(40+eSer*80-12, eY, 24, 24, RENK_TEHLIKE); delay(30);
  } oyunBitti(skor);
}

void oynaLabirent() {
  byte pX=10, pY=10, hX=140, hY=110, eX1=80, eY1=20; int8_t d1=4; int skor=0; bool aktif=true; 
  while(aktif) {
    if (digitalRead(pinExit) == LOW) return; 

    int jX=analogRead(pinX), jY=analogRead(pinY); tft.fillRect(pX, pY, 8, 8, RENK_SIYAH);
    if(jX<300 && pX>=4) pX-=4; if(jX>800 && pX<=148) pX+=4; if(jY<300 && pY>=4) pY-=4; if(jY>800 && pY<=116) pY+=4;
    tft.fillRect(eX1, eY1, 10, 40, RENK_SIYAH); eY1+=d1; if(eY1>200 || eY1>88) d1=-d1;
    if(pX+8>eX1 && pX<eX1+10 && pY+8>eY1 && pY<eY1+40) aktif=false;
    if(abs(pX-hX)<8 && abs(pY-hY)<8) { skor+=10; hX=random(10,140); hY=random(10,110); tft.fillScreen(RENK_SIYAH); }
    tft.fillRect(eX1, eY1, 10, 40, RENK_TEHLIKE); tft.fillCircle(hX+4, hY+4, 4, RENK_ALTIN); tft.fillRect(pX, pY, 8, 8, RENK_VURGU); delay(30);
  } oyunBitti(skor);
}

void oynaMayin() {
  byte pX=80, pY=64, mX[5], mY[5]; int skor=0; bool aktif=true; for(byte i=0;i<5;i++){ mX[i]=random(0,150); mY[i]=random(0,120); } 
  while(aktif){
    if (digitalRead(pinExit) == LOW) return; 

    int jX=analogRead(pinX), jY=analogRead(pinY); tft.fillRect(pX, pY, 8, 8, RENK_SIYAH);
    if(jX<300 && pX>=4) pX-=4; if(jX>800 && pX<=148) pX+=4; if(jY<300 && pY>=4) pY-=4; if(jY>800 && pY<=116) pY+=4;
    for(byte i=0;i<5;i++){
       tft.fillRect(mX[i], mY[i], 9, 9, RENK_SIYAH); 
       mY[i]+=random(2,5); if(mY[i]>128){ mY[i]=0; mX[i]=random(0,150); skor++; }
       if(abs(pX-mX[i])<7 && abs(pY-mY[i])<7) aktif=false; tft.fillCircle(mX[i]+4, mY[i]+4, 4, RENK_TEHLIKE);
    } tft.fillRect(pX, pY, 8, 8, RENK_VURGU); delay(30);
  } oyunBitti(skor);
}

void oynaKaya() {
  byte pX=80, pY=110, kX=40, kY=0; int8_t hX=4, hY=4; int skor=0; bool aktif=true; 
  while(aktif){
    if (digitalRead(pinExit) == LOW) return; 

    int jX=analogRead(pinX); tft.fillRect(pX, pY, 16, 10, RENK_SIYAH);
    if(jX<300 && pX>=6) pX-=6; if(jX>800 && pX<=138) pX+=6;
    tft.fillRect(kX, kY, 24, 24, RENK_SIYAH); kX+=hX; kY+=hY;
    if(kX>136) hX=-hX; if(kY>128) { hY=-hY; if(kY<200){kY=0; skor+=5;} }
    if(pX+16>kX && pX<kX+24 && pY+10>kY && pY<kY+24) aktif=false;
    tft.fillRect(kX, kY, 24, 24, RENK_PASIF); tft.fillRect(pX, pY, 16, 10, RENK_VURGU); delay(30);
  } oyunBitti(skor);
}

void oynaDefans() {
  byte pX=80, pY=64, e1=0, y1=64, e2=160, y2=64; int8_t mX=-1, mY=-1, mYon=1; int skor=0; bool aktif=true; 
  while(aktif){
    if (digitalRead(pinExit) == LOW) return; 

    int jX=analogRead(pinX); bool ates=(digitalRead(pinSW)==LOW);
    if(jX<300) mYon=-1; else if(jX>800) mYon=1;
    tft.fillCircle(pX, pY, 8, C_DEFANS); tft.fillRect(pX+(mYon*6), pY-2, 6, 4, C_DEFANS); 
    
    if(ates && mX<0) { mX=pX; mY=pY; }
    if(mX>=0) { tft.fillRect(mX, mY-2, 6, 4, RENK_SIYAH); mX+=mYon*8; if(mX<0 || mX>160) mX=-1; else tft.fillRect(mX, mY-2, 6, 4, RENK_ALTIN); }
    
    tft.fillRect(e1, y1, 10, 10, RENK_SIYAH); tft.fillRect(e2, y2, 10, 10, RENK_SIYAH); e1+=2; e2-=2;
    if(e1>pX-10 || e2<pX+10) aktif=false; 
    
    if(mX>0 && mX<e1+10 && mX+6>e1 && mY>y1 && mY<y1+10) { skor+=10; tft.fillRect(e1,y1,10,10,RENK_SIYAH); e1=0; y1=random(10,110); mX=-1;}
    if(mX>0 && mX<e2+10 && mX+6>e2 && mY>y2 && mY<y2+10) { skor+=10; tft.fillRect(e2,y2,10,10,RENK_SIYAH); e2=160; y2=random(10,110); mX=-1;}
    tft.fillRect(e1, y1, 10, 10, RENK_TEHLIKE); tft.fillRect(e2, y2, 10, 10, RENK_TEHLIKE); delay(30);
  } oyunBitti(skor);
}

bool tetrisCarpisma(byte t[20][10], byte tip, byte rot, int8_t cx, int8_t cy) {
  uint16_t s = pgm_read_word(&tetrisSekiller[tip][rot]);
  for(byte i=0; i<16; i++) {
    if(s & (1 << (15 - i))) {
      int8_t nx = cx + (i % 4), ny = cy + (i / 4);
      if(nx < 0 || nx >= 10 || ny >= 20 || (ny >= 0 && t[ny][nx])) return true;
    }
  } return false;
}

void oynaTetris() {
  byte tahta[20][10] = {0}; int8_t px = 3, py = -2; byte rot = 0, tip = random(0,7); int skor = 0; 
  unsigned long sonDusme = millis(); int hiz = 400; bool aktif = true; 
  tft.drawRect(49, 3, 62, 122, RENK_PASIF); 
  
  while(aktif) {
    if (digitalRead(pinExit) == LOW) return; 

    int jX = analogRead(pinX), jY = analogRead(pinY); bool btn = (digitalRead(pinSW) == LOW);
    int8_t oPx = px, oPy = py; byte oRot = rot;
    
    if(btn) { rot = (rot + 1) % 4; if(tetrisCarpisma(tahta, tip, rot, px, py)) rot = oRot; delay(200); }
    if(jX < 300) { px--; if(tetrisCarpisma(tahta, tip, rot, px, py)) px++; delay(100); }
    if(jX > 800) { px++; if(tetrisCarpisma(tahta, tip, rot, px, py)) px--; delay(100); }
    if(jY > 800) { py++; if(tetrisCarpisma(tahta, tip, rot, px, py)) py--; delay(50); }

    if(millis() - sonDusme > hiz) {
      py++; sonDusme = millis();
      if(tetrisCarpisma(tahta, tip, rot, px, py)) {
        py--; uint16_t s = pgm_read_word(&tetrisSekiller[tip][rot]);
        for(byte i=0; i<16; i++) if(s & (1 << (15 - i))) { int8_t ny = py + (i / 4); if(ny < 0) aktif = false; else tahta[ny][px + (i % 4)] = tip + 1; }
        
        for(int8_t r = 19; r >= 0; r--) {
          bool dolu = true; for(byte c = 0; c < 10; c++) if(!tahta[r][c]) dolu = false;
          if(dolu) { skor += 10; for(int8_t k = r; k > 0; k--) for(byte c = 0; c < 10; c++) tahta[k][c] = tahta[k-1][c]; r++; hiz = max(100, hiz - 10); }
        }
        px = 3; py = -2; tip = random(0,7); rot = 0;
      }
    }
    for(byte r=0; r<20; r++) for(byte c=0; c<10; c++) {
      byte x = 50 + c*6, y = 4 + r*6;
      bool parcaMi = false; uint16_t s = pgm_read_word(&tetrisSekiller[tip][rot]);
      for(byte i=0; i<16; i++) if(s & (1 << (15 - i))) if(px + (i%4) == c && py + (i/4) == r) parcaMi = true;
      if(parcaMi) tft.fillRect(x, y, 5, 5, pgm_read_word(&tetrisRenkler[tip]));
      else if(tahta[r][c]) tft.fillRect(x, y, 5, 5, pgm_read_word(&tetrisRenkler[tahta[r][c]-1]));
      else tft.fillRect(x, y, 5, 5, RENK_SIYAH);
    }
  } oyunBitti(skor);
}

void oynaTugla() {
  byte tugla[5][10]; byte kalan = 50; for(byte i=0; i<5; i++) for(byte j=0; j<10; j++) tugla[i][j] = 1;
  byte pX = 60, pY = 118, tX = 80, tY = 60; int8_t hX = 2, hY = -2; int skor = 0; bool aktif = true; 
  
  while(aktif) {
    if (digitalRead(pinExit) == LOW) return; 

    int jX = analogRead(pinX); tft.fillRect(pX, pY, 30, 4, RENK_SIYAH); 
    tft.fillRect(tX-3, tY-3, 7, 7, RENK_SIYAH); 
    
    if(jX < 300 && pX >= 5) pX -= 5; if(jX > 800 && pX <= 125) pX += 5;
    
    tX += hX; tY += hY;
    if(tX <= 3 || tX >= 157) hX = -hX; if(tY <= 3) hY = -hY;
    if(tY >= pY-3 && tX >= pX && tX <= pX+30) { hY = -abs(hY); tY = pY-3; } 
    if(tY > 128) aktif = false;

    if(tY < 40) {
      byte c = tX / 16, r = tY / 8;
      if(r < 5 && c < 10 && tugla[r][c]) { tugla[r][c] = 0; hY = -hY; skor += 5; kalan--; tft.fillRect(c*16, r*8, 15, 6, RENK_SIYAH); }
    }
    if(kalan == 0) aktif = false; 

    for(byte i=0; i<5; i++) for(byte j=0; j<10; j++) if(tugla[i][j]) tft.fillRect(j*16, i*8, 15, 6, pgm_read_word(&tetrisRenkler[i%7]));
    tft.fillRect(pX, pY, 30, 4, RENK_VURGU); tft.fillCircle(tX, tY, 3, RENK_BEYAZ); delay(20);
  } oyunBitti(skor);
}

void oynaNisanci() {
  byte cX = 80, cY = 64; byte hX = random(20, 140), hY = random(20, 100); int8_t sure = 100; int skor = 0; bool aktif = true; 
  while(aktif) {
    if (digitalRead(pinExit) == LOW) return; 

    int jX = analogRead(pinX), jY = analogRead(pinY); bool atis = (digitalRead(pinSW) == LOW);
    tft.drawCircle(cX, cY, 6, RENK_SIYAH); tft.drawPixel(cX, cY, RENK_SIYAH);
    
    if(jX < 300 && cX > 6) cX -= 3; if(jX > 800 && cX < 154) cX += 3;
    if(jY < 300 && cY > 6) cY -= 3; if(jY > 800 && cY < 122) cY += 3;

    tft.fillCircle(hX, hY, 8, RENK_TEHLIKE); tft.fillCircle(hX, hY, 4, RENK_BEYAZ); 
    
    if(atis) {
      if(abs(cX - hX) < 8 && abs(cY - hY) < 8) { skor += 10; tft.fillCircle(hX, hY, 8, RENK_SIYAH); hX = random(20, 140); hY = random(20, 100); sure = max(20, sure - 5); }
      tft.fillScreen(RENK_VURGU); delay(50); tft.fillScreen(RENK_SIYAH); cX = 80; cY = 64; 
    }
    sure--; if(sure <= 0) aktif = false;
    
    tft.drawCircle(cX, cY, 6, RENK_VURGU); tft.drawPixel(cX, cY, RENK_VURGU); delay(30);
  } oyunBitti(skor);
}