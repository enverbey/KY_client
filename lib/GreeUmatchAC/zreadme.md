[ ] zamanlama hatası alırsa aynı komutu tekrar gönder gibi bir şey eklemek gerekebilir
başarısız olacaksa bile en azından birkaç defa denesin

----

# GreeUmatchAC Kütüphanesi Kullanım Kılavuzu

Bu kütüphane, ESP32 geliştirme kartı ve bir RS485-TTL dönüştürücü kullanarak GREE U-Match 4. Nesil DC Inverter klimaları Modbus RTU protokolü üzerinden kontrol etmek için geliştirilmiştir.

### Gerekli Donanımlar ve Bağlantı

1.  **ESP32 Geliştirme Kartı**
2.  **RS485-TTL Dönüştürücü Modül**
3.  **Gree U-Match Klima (Modbus destekli)**

#### Bağlantı Şeması

Aşağıdaki şema, donanımların birbirine nasıl bağlanacağını gösterir. ESP32 üzerindeki pin numaraları örnektir ve değiştirilebilir.

**1. ESP32 -\> RS485-TTL Dönüştürücü**
| RS485 Pin | ESP32 Pin | Açıklama |
| :--- | :--- | :--- |
| `VCC` | `3V3` | Modülün Güç Beslemesi |
| `GND` | `GND` | Toprak Hattı |
| `RO` (Receiver Out) | `GPIO16 (RX2)` | ESP32'nin Veri Alma Pini |
| `DI` (Driver In) | `GPIO17 (TX2)` | ESP32'nin Veri Gönderme Pini |
| `DE` + `RE` | `GPIO4` | Yön Kontrol Pini (Gönderme/Alma) |

**2. RS485-TTL Dönüştürücü -\> Klima**
| RS485 Pin | Klima Modbus Terminali |
| :--- | :--- |
| `A` | `A` |
| `B` | `B` |

### Kütüphane Yapısı

  * `Registers.h`: Klimanın tüm Modbus adreslerini ve sabitlerini içerir.
  * `ModbusHandler.h` / `.cpp`: Alt seviye Modbus RTU haberleşmesini yönetir ("Makine Dairesi").
  * `GreeUmatchAC.h` / `.cpp`: Klimayı kontrol etmek için kullanılan üst seviye ve kullanıcı dostu arayüzü sunar ("Kaptan Köşkü").

### Örnek `main.cpp` Kullanımı

Bu örnek, PlatformIO veya Arduino IDE projenizin ana dosyası olabilir. Klimayı başlatır, soğutma moduna alır, sıcaklığı ayarlar ve her 10 saniyede bir ortam sıcaklığını okuyup seri monitöre yazar.

```cpp
#include <Arduino.h>
#include "GreeUmatchAC.h" // Kendi kütüphanemizi dahil ediyoruz

// --- AYARLAR ---
// ESP32'nin RS485 modülüne bağlandığı pinler
#define RX2_PIN 16
#define TX2_PIN 17
#define DE_RE_PIN 4 // Yön kontrol pini

// Kontrol edilecek klimanın Modbus adresi
#define AC_SLAVE_ADDRESS 1

// Klima kontrolü için bir nesne oluştur
GreeUmatchAC klima;

void setup() {
  // Hata ayıklama için ana seri portu başlat
  Serial.begin(115200);
  while (!Serial); // Seri portun açılmasını bekle
  Serial.println("\n--- Gree Klima Kontrol Sistemi Baslatiliyor ---");

  // Klimayı ve Modbus haberleşmesini başlat
  // Not: Serial2, ESP32'nin ikinci donanımsal seri portudur.
  klima.begin(Serial2, RX2_PIN, TX2_PIN, DE_RE_PIN, AC_SLAVE_ADDRESS);

  delay(1000);

  // --- BAŞLANGIÇ KOMUTLARI ---
  Serial.println("Klimaya ilk komutlar gonderiliyor...");

  // 1. Klimayı Aç
  if (klima.turnOn()) {
    Serial.println("Klima ACILDI.");
  } else {
    Serial.println("HATA: Klima acilamadi!");
  }
  
  delay(2000); // Komutlar arasında kısa bir bekleme

  // 2. Modu Soğutma olarak ayarla
  if (klima.setMode(GreeUmatch::MODE_COOL)) {
    Serial.println("Mod SOGUTMA olarak ayarlandi.");
  } else {
    Serial.println("HATA: Mod ayarlanamadi!");
  }

  delay(2000);

  // 3. Hedef sıcaklığı 24 derece olarak ayarla
  if (klima.setTemperature(24)) {
    Serial.println("Hedef sicaklik 24 C olarak ayarlandi.");
  } else {
    Serial.println("HATA: Sicaklik ayarlanamadi!");
  }
  
  Serial.println("\n--- Kurulum Tamamlandi ---");
}

void loop() {
  Serial.println("--------------------");
  
  // Her 10 saniyede bir ortam sıcaklığını oku ve yazdır
  float odaSicakligi = klima.getRoomTemperature();
  
  if (odaSicakligi > -900.0f) { // -999.0f hata kodu değilse
    Serial.print("Oda Sicakligi: ");
    Serial.print(odaSicakligi);
    Serial.println(" C");
  } else {
    Serial.println("HATA: Oda sicakligi okunamadi!");
  }

  // Kompresörün durumunu kontrol et
  bool kompresorCalisiyor = klima.isCompressorOn();
  Serial.print("Kompresor Durumu: ");
  Serial.println(kompresorCalisiyor ? "CALISIYOR" : "DURUYOR");
  
  delay(10000); // 10 saniye bekle
}
```