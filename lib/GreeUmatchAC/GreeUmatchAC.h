#ifndef GREE_UMATCH_AC_H
#define GREE_UMATCH_AC_H

#include "ModbusHandler.h"
#include "Registers.h"
#include <cstdint>

/**
 * @file GreeUmatchAC.h
 * @brief Gree U-Match klimalar için üst seviye kontrol sınıfı ("Kaptan Köşkü").
 * Bu sınıf, ModbusHandler'ı kullanarak klimanın tüm fonksiyonlarını
 * (açma, kapama, sıcaklık ayarı vb.) kolay ve anlaşılır metodlarla
 * yönetmeyi sağlar.
 */
class GreeUmatchAC {
public:
    GreeUmatchAC();

    /**
     * @brief Klimayla haberleşmeyi başlatır. ESP32'nin setup() fonksiyonu içinde çağrılmalıdır.
     * @param serial Kullanılacak HardwareSerial portu (örn: Serial1, Serial2).
     * @param rxPin ESP32'nin UART RX pini.
     * @param txPin ESP32'nin UART TX pini.
     * @param deRePin RS485-TTL dönüştürücünün DE/RE pinlerini kontrol eden ESP32 pini.
     * @param slaveAddress Kontrol edilecek klimanın Modbus adresi (1-255).
     */
    void begin(HardwareSerial& serial, int8_t rxPin, int8_t txPin, int8_t deRePin, uint8_t slaveAddress);

    // --- TEMEL KONTROLLER ---

    /** @brief Klimayı açar. */
    bool turnOn();

    /** @brief Klimayı kapatır. */
    bool turnOff();

    /** @brief Klimanın güç durumunu ayarlar. @param on true: Aç, false: Kapat. */
    bool setPower(bool on);

    /** @brief Klimanın çalışma modunu ayarlar.
     * @param mode Mod sabiti (örn: GreeUmatch::MODE_COOL, GreeUmatch::MODE_HEAT vb.). */
    bool setMode(uint16_t mode);

    /** @brief Klimanın fan hızını ayarlar.
     * @param speed Fan hızı sabiti (örn: GreeUmatch::FAN_AUTO, GreeUmatch::FAN_SPEED_3 vb.). */
    bool setFanSpeed(uint16_t speed);

    /** @brief Klimanın hedef sıcaklığını ayarlar.
     * @param temp Hedef sıcaklık (Genellikle 16-30 arası). */
    bool setTemperature(uint16_t temp);

    // --- GELİŞMİŞ KONTROLLER VE ÖZELLİKLER ---

    /** @brief Dikey kanatçıkların salınım modunu veya pozisyonunu ayarlar.
     * @param swingMode Dikey salınım sabiti (örn: GreeUmatch::SWING_V_FULL, GreeUmatch::SWING_V_POS_3 vb.). */
    bool setSwingVertical(uint16_t swingMode);

    /** @brief Yatay kanatçıkların salınım modunu veya pozisyonunu ayarlar.
     * @param swingMode Yatay salınım sabiti (örn: GreeUmatch::SWING_H_SAME_DIR, GreeUmatch::SWING_H_POS_3 vb.). */
    bool setSwingHorizontal(uint16_t swingMode);

    /** @brief Sessiz mod özelliğini açar veya kapatır. */
    bool setQuietMode(bool enabled);

    /** @brief Turbo (güçlü) mod özelliğini açar veya kapatır. */
    bool setTurboMode(bool enabled);

    /** @brief X-Fan (iç üniteyi kurutma) özelliğini açar veya kapatır. */
    bool setXFan(bool enabled);

    /** @brief Nem Alma (Drying) modunu aktif eder. Bu, setMode(GreeUmatch::MODE_DRY) ile aynı işi yapar. */
    bool enableDryMode();

    // --- ENERJİ YÖNETİMİ VE LİMİTLER ---

    /**
     * @brief Klimanın çalışabileceği sıcaklık aralığını sınırlar.
     * @param coolMin Soğutmada inilebilecek en düşük sıcaklık (16-30).
     * @param heatMax Isıtmada çıkılabilecek en yüksek sıcaklık (16-30).
     */
    bool setTempLimits(uint16_t coolMin, uint16_t heatMax);
    
    /**
     * @brief Ayarlanan sıcaklık limitlerinin aktif olup olmayacağını belirler.
     * @param enabled true ise limitler devreye girer, false ise devre dışı kalır.
     * @note PDF'e göre bu iki limit aynı anda açılıp kapatılmalıdır.
     */
    bool enableTempLimits(bool enabled);

    // --- DURUM OKUMA FONKSİYONLARI ---

    /** @brief Ortamın anlık sıcaklığını (°C) okur. @return Ortam sıcaklığı veya hata durumunda negatif bir değer. */
    float getRoomTemperature();

    /** @brief Dış ortamın anlık sıcaklığını (°C) okur. @return Dış ortam sıcaklığı veya hata durumunda negatif bir değer. */
    float getOutdoorTemperature();

    /** @brief Kompresörün çalışıp çalışmadığını kontrol eder. @return true: Çalışıyor, false: Duruyor/Hata. */
    bool isCompressorOn();

    // Mevcut ayarları okumak için (getter'lar)
    bool getPowerState(bool& isOn);
    bool getMode(uint16_t& mode);
    bool getFanSpeed(uint16_t& speed);
    bool getSetTemperature(uint16_t& temp);

private:
    ModbusHandler _modbus; // Modbus haberleşmesini yöneten alt sınıf
    uint8_t _slaveAddress;

    // Tek bitlik özellikleri (Coil) okumak/yazmak için özel yardımcı fonksiyonlar
    bool setBoolFeature(uint16_t coilAddress, bool enabled);
    bool getBoolFeature(uint16_t coilAddress, bool& value);
};

#endif // GREE_UMATCH_AC_H