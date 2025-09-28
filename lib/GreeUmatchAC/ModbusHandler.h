#ifndef MODBUS_HANDLER_H
#define MODBUS_HANDLER_H

#include <Arduino.h>
#include <cstdint>

/**
 * @file ModbusHandler.h
 * @brief ESP32 için hedefe yönelik, basit bir Modbus RTU Master sınıfı.
 * * Bu sınıf, RS485-TTL dönüştürücü üzerinden tek bir slave cihaz (Gree Klima) ile
 * haberleşmek için gerekli alt seviye Modbus fonksiyonlarını yönetir.
 * CRC hesaplama, istek/yanıt çerçevelerinin yönetimi ve DE/RE pin kontrolü
 * gibi detayları soyutlar.
 */
class ModbusHandler {
public:
    /**
     * @brief Kurucu fonksiyon (Constructor).
     */
    ModbusHandler();

    /**
     * @brief Seri portu ve Modbus haberleşmesini başlatır. Bu fonksiyon setup() içinde çağrılmalıdır.
     * * @param serial Kullanılacak HardwareSerial portu (örn: Serial1, Serial2).
     * @param rxPin ESP32'nin UART RX pini.
     * @param txPin ESP32'nin UART TX pini.
     * @param deRePin RS485-TTL dönüştürücünün DE ve RE pinlerini kontrol eden ESP32 pini.
     * @param slaveAddress Kontrol edilecek klimanın Modbus adresi (1-255).
     */
    void begin(HardwareSerial& serial, int8_t rxPin, int8_t txPin, int8_t deRePin, uint8_t slaveAddress);

    /**
     * @brief Belirtilen adresteki bir Holding Register'a 16-bit'lik bir değer yazar (Fonksiyon Kodu 0x10).
     * * @param regAddress Yazılacak register'ın adresi (örn: REG_SET_TEMP).
     * @param value Yazılacak değer.
     * @return İşlem başarılı ise true, değilse false (timeout, CRC hatası vb.).
     */
    bool writeRegister(uint16_t regAddress, uint16_t value);

    /**
     * @brief Belirtilen adresteki bir Holding Register'ı okur (Fonksiyon Kodu 0x03).
     * * @param regAddress Okunacak register'ın adresi (örn: REG_AMBIENT_TEMP).
     * @param value Okunan değer bu değişkene referans yoluyla yazılır.
     * @return İşlem başarılı ise true, değilse false.
     */
    bool readRegister(uint16_t regAddress, uint16_t& value);

    /**
     * @brief Belirtilen adresteki bir Coil'e (Bit) bir değer yazar (Fonksiyon Kodu 0x0F).
     * * @param coilAddress Yazılacak coil'in adresi (örn: COIL_TURBO).
     * @param value Yazılacak durum (true için 1, false için 0).
     * @return İşlem başarılı ise true, değilse false.
     */
    bool writeCoil(uint16_t coilAddress, bool value);

    /**
     * @brief Belirtilen adresteki bir Coil'i (Bit) okur (Fonksiyon Kodu 0x01).
     * * @param coilAddress Okunacak coil'in adresi (örn: COIL_STATE_COMPRESSOR_ON).
     * @param value Okunan durum bu değişkene referans yoluyla yazılır.
     * @return İşlem başarılı ise true, değilse false.
     */
    bool readCoil(uint16_t coilAddress, bool& value);


private:
    // --- İç Değişkenler ---
    HardwareSerial* _serial;       // Kullanılan seri portun işaretçisi.
    uint8_t         _slaveAddress; // Klimanın sabit adresi.
    int8_t          _deRePin;      // RS485 yön kontrol pini (DE/RE).
    unsigned long   _timeout = 1000; // Yanıt için bekleme süresi (milisaniye).

    // --- Yardımcı (Private) Fonksiyonlar ---

    /**
     * @brief Gönderilecek paketin sonuna eklenecek 16-bit CRC kodunu hesaplar.
     * @param buffer CRC'si hesaplanacak veri dizisi.
     * @param length Veri dizisinin uzunluğu.
     * @return Hesaplanan 16-bit CRC değeri.
     */
    uint16_t calculateCRC(const uint8_t* buffer, uint8_t length);
    
    /**
     * @brief Modbus istek çerçevesini (istek paketini) oluşturup seri porttan gönderir.
     * Göndermeden önce DE/RE pinini HIGH, gönderdikten sonra LOW yapar.
     * @param requestBuffer Gönderilecek olan tam çerçeve.
     * @param length Çerçevenin uzunluğu.
     */
    void sendRequest(const uint8_t* requestBuffer, uint8_t length);

    /**
     * @brief Klimadan gelen yanıtı okur, doğrular ve buffer'a yazar.
     * Zaman aşımı ve CRC kontrolü yapar.
     * @param responseBuffer Yanıtın yazılacağı buffer.
     * @param expectedLength Beklenen yanıt uzunluğu.
     * @return Başarılı bir yanıt alınırsa true, değilse false.
     */
    bool receiveResponse(uint8_t* responseBuffer, uint8_t expectedLength);
};

#endif // MODBUS_HANDLER_H