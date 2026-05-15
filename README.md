# Comsole 1 - Retro Mini El Konsolu (ComUI)

Comsole 1, Arduino altyapısı kullanılarak geliştirilmiş, içinde 15 farklı mini oyun ve dinamik bir arayüz (ComUI) barındıran taşınabilir bir retro el konsolu projesidir. ST7735 TFT ekran ve analog joystick modülü ile tamamen entegre çalışır.

## 🚀 Özellikler

* **Dinamik İşletim Sistemi Arayüzü (ComUI 1.0):** Kart tasarımlı minimalist ve mat renk paletine sahip modern menü geçişleri.
* **15 Farklı Mini Oyun:** 
  * Yılan, Uzay Savaşları, Pong, Kaçış, Zıplama, Nesne Yakalama, Flappy Bird, Yarış, Labirent, Mayın Tarlası, Kaya Kaçışı, Defans, Tetris, Tuğla Kırma, Nişancı.
* **Gelişmiş Ayarlar Menüsü:**
  * **Ekran Yönü:** Ekranı ters çevirme (Flip) seçeneği.
  * **Tema Desteği:** Açık (Light) ve Koyu (Dark) tema modları.
  * **Dil Desteği:** Tamamen Türkçe ve İngilizce çift dil seçeneği.
  * **Sistem Bilgisi:** Geliştirici ekibi gösteren animasyonlu sistem ekranı.
* **Güvenli Güç Yönetimi:** Arka planda güvenli kapatma uyarı ekranı sistemi.

---

## 🛠️ Donanım Gereksinimleri

Projeyi hayata geçirmek için aşağıdaki donanım bileşenleri kullanılmıştır:

| Bileşen | Pin Bağlantısı (Arduino) | Açıklama |
| :--- | :--- | :--- |
| **Arduino Kartı** | - | Uno, Nano veya Mega |
| **ST7735 TFT Ekran** | CS: 10, RST: 8, DC: 9, BLK: 6 | 128x128 veya 128x160 Renkli Ekran |
| **Analog Joystick (X Ekseni)** | A0 | Menü ve Oyun Kontrolleri |
| **Analog Joystick (Y Ekseni)** | A1 | Menü ve Oyun Kontrolleri |
| **Joystick Buton (SW)** | Pin 2 | Seçim / Ateş Etme / Zıplama |
| **Exit / Kapatma Butonu** | Pin 3 | Oyundan Çıkış ve Güç Menüsü |

---

## 📦 Gerekli Kütüphaneler

Kodun sorunsuz derlenebilmesi için Arduino IDE'nize aşağıdaki kütüphanelerin kurulması gerekir:

* `Adafruit_GFX.h` (Adafruit GFX Library)
* `Adafruit_ST7735.h` (Adafruit ST7735 and ST7789 Library)
* `SPI.h` (Yerleşik kütüphane)
* `avr/pgmspace.h` (Yerleşik kütüphane - PROGMEM kullanımı için)

---

## 💻 Kurulum ve Çalıştırma

1. Bu depoyu (repository) bilgisayarınıza indirin veya klonlayın.
2. Ana `.ino` dosyasını Arduino IDE ile açın.
3. Yukarıda belirtilen eksik kütüphaneleri **Kütüphane Yöneticisi** üzerinden kurun.
4. Donanım bağlantılarını pin şemasına göre yapın.
5. Arduino kartınızı ve doğru portu seçerek kodu yükleyin!

---

## 👥 Geliştiriciler (Devs)
* **C. Uzunsoy**
* **O. Peksoz**
* **M. Isik**

---

## 📝 Lisans
Bu proje açık kaynaklı bir hobi projesidir. İstediğiniz gibi geliştirebilir, değiştirebilir ve kendi el konsolunuzu üretebilirsiniz!