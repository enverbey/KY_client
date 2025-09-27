// #ifndef GREE_UMATCH_MODBUS_HPP
// #define GREE_UMATCH_MODBUS_HPP

// #include <Arduino.h>
// // Not: Bu sınıf, 'ModbusMaster' kütüphanesini kullanacak şekilde tasarlanmıştır.
// #include <ModbusMaster.h>

// /**
//  * @brief Klimanın çalışma modlarını tanımlar.
//  * @note Değerler, Modbus dokümanındaki Word 17'ye göre belirlenmiştir.
//  */
// enum class OperatingMode : uint16_t {
//     [cite_start]COOL      = 1, // Soğutma [cite: 333]
//     [cite_start]HEAT      = 2, // Isıtma [cite: 333]
//     [cite_start]DRY       = 3, // Nem Alma [cite: 333]
//     [cite_start]FAN_ONLY  = 4, // Sadece Fan [cite: 333]
//     [cite_start]AUTO      = 5, // Otomatik Mod [cite: 333]
//     UNKNOWN   = 0
// };

// /**
//  * @brief Klimanın fan hızı seviyelerini tanımlar.
//  * @note Değerler, Modbus dokümanındaki Word 19'a göre belirlenmiştir.
//  */
// enum class FanSpeed : uint16_t {
//     [cite_start]AUTO   = 0, // Otomatik Hız [cite: 333]
//     [cite_start]SPEED_1 = 1, // Hız 1 [cite: 335]
//     [cite_start]SPEED_2 = 2, // Hız 2 [cite: 335]
//     [cite_start]SPEED_3 = 3, // Hız 3 [cite: 335]
//     [cite_start]SPEED_4 = 4, // Hız 4 [cite: 335]
//     [cite_start]SPEED_5 = 5, // Hız 5 [cite: 335]
//     [cite_start]TURBO  = 6, // Turbo Hız [cite: 335]
//     UNKNOWN = 99
// };

// /**
//  * @brief Klimanın uyku modlarını tanımlar.
//  * @note Değerler, Modbus dokümanındaki Word 25'e göre belirlenmiştir.
//  */
// enum class SleepMode : uint16_t {
//     [cite_start]OFF    = 0, // Uyku modu kapalı [cite: 339]
//     [cite_start]MODE_1 = 1, // Uyku Modu 1 [cite: 339]
//     [cite_start]MODE_2 = 2, // Uyku Modu 2 [cite: 339]
//     [cite_start]MODE_3 = 3, // Uyku Modu 3 [cite: 339]
//     UNKNOWN = 99
// };


// class GreeUmatchModbus {
// public:
//     /**
//      * @brief Kurucu Metot.
//      * @param node Kullanılacak ModbusMaster nesnesi.
//      * @param address Kontrol edilecek klimanın Modbus adresi (1-255).
//      */
//     GreeUmatchModbus(ModbusMaster &node, uint8_t address);

//     // --- TEMEL KONTROL FONKSİYONLARI ---

//     /**
//      * @brief Klimayı açar.
//      * @return İşlem başarılı ise true, değilse false döner.
//      */
//     bool powerOn();

//     /**
//      * @brief Klimayı kapatır.
//      * @return İşlem başarılı ise true, değilse false döner.
//      */
//     bool powerOff();

//     /**
//      * @brief Klimanın çalışma modunu ayarlar.
//      * @param mode Ayarlanacak çalışma modu (OperatingMode enum).
//      * @return İşlem başarılı ise true, değilse false döner.
//      */
//     bool setMode(OperatingMode mode);

//     /**
//      * @brief Klimanın hedef sıcaklığını ayarlar.
//      * @param temperature Ayarlanacak sıcaklık (16-30°C).
//      * @return İşlem başarılı ise true, değilse false döner.
//      */
//     bool setTemperature(uint8_t temperature);

//     /**
//      * @brief Klimanın fan hızını ayarlar.
//      * @param speed Ayarlanacak fan hızı (FanSpeed enum).
//      * @return İşlem başarılı ise true, değilse false döner.
//      */
//     bool setFanSpeed(FanSpeed speed);


//     // --- GELİŞMİŞ KONTROL FONKSİYONLARI ---
    
//     /**
//      * @brief Uyku modunu ayarlar.
//      * @param sleep_mode Ayarlanacak uyku modu (SleepMode enum).
//      * @return İşlem başarılı ise true, değilse false döner.
//      */
//     bool setSleepMode(SleepMode sleep_mode);

//     /**
//      * @brief Turbo modunu aktif veya pasif hale getirir.
//      * @param is_active Aktif etmek için true, pasif için false.
//      * @return İşlem başarılı ise true, değilse false döner.
//      */
//     bool setTurboMode(bool is_active);

//     /**
//      * @brief Sessiz modu aktif veya pasif hale getirir.
//      * @param is_active Aktif etmek için true, pasif için false.
//      * @return İşlem başarılı ise true, değilse false döner.
//      */
//     bool setQuietMode(bool is_active);


//     // --- DURUM OKUMA FONKSİYONLARI ---

//     /**
//      * @brief Klimanın açık olup olmadığını kontrol eder.
//      * @return Klima açıksa true, kapalıysa false döner. Okuma hatasında false döner.
//      */
//     bool getPowerStatus();

//     /**
//      * @brief Klimanın mevcut çalışma modunu okur.
//      * @return Mevcut çalışma modu (OperatingMode enum). Hata durumunda UNKNOWN döner.
//      */
//     OperatingMode getCurrentMode();
    
//     /**
//      * @brief Ayarlanmış olan hedef sıcaklığı okur.
//      * @return Ayarlanan sıcaklık değeri. Hata durumunda -1 döner.
//      */
//     int8_t getSetTemperature();

//     /**
//      * @brief İç ortam sıcaklığını okur.
//      * @return Ortam sıcaklığı (°C). Hata durumunda -100 döner.
//      */
//     float getAmbientTemperature();
    
//     /**
//      * @brief Dış ortam sıcaklığını okur.
//      * @return Dış ortam sıcaklığı (°C). Hata durumunda -100 döner.
//      */
//     float getOutdoorTemperature();

//     // --- KİLİTLEME (SHIELDING) FONKSİYONLARI ---

//     /**
//      * @brief Yerel kumandadan sıcaklık ayarının değiştirilmesini engeller/izin verir.
//      * @param is_shielded Kilitlemek için true, kilidi açmak için false.
//      * @return İşlem başarılı ise true, değilse false döner.
//      */
//     bool shieldTemperatureSetting(bool is_shielded);

//     /**
//      * @brief Yerel kumandadan mod ayarının değiştirilmesini engeller/izin verir.
//      * @param is_shielded Kilitlemek için true, kilidi açmak için false.
//      * @return İşlem başarılı ise true, değilse false döner.
//      */
//     bool shieldModeSetting(bool is_shielded);

//     /**
//      * @brief Yerel kumandadan klimanın açılıp kapatılmasını engeller/izin verir.
//      * @param is_shielded Kilitlemek için true, kilidi açmak için false.
//      * @return İşlem başarılı ise true, değilse false döner.
//      */
//     bool shieldPowerButton(bool is_shielded);


// private:
//     ModbusMaster& _node; // ModbusMaster nesnesine referans
//     uint8_t _address;    // Klimanın Modbus adresi

//     // Modbus register ve coil (bit) adresleri için sabitler
//     // Bu, kodun daha okunaklı olmasını sağlar.
//     static const uint16_t REG_POWER        = 0x0002;
//     static const uint16_t REG_AMBIENT_TEMP = 0x0004;
//     static const uint16_t REG_OUTDOOR_TEMP = 0x0031; // 49 decimal = 0x31 hex
//     static const uint16_t REG_MODE         = 0x0011; // 17 decimal = 0x11 hex
//     static const uint16_t REG_FAN_SPEED    = 0x0013; // 19 decimal = 0x13 hex
//     static const uint16_t REG_SET_TEMP     = 0x0014; // 20 decimal = 0x14 hex
//     static const uint16_t REG_SLEEP_MODE   = 0x0019; // 25 decimal = 0x19 hex
    
//     static const uint16_t COIL_SHIELD_TEMP = 18;
//     static const uint16_t COIL_SHIELD_MODE = 19;
//     static const uint16_t COIL_SHIELD_POWER = 20;
//     static const uint16_t COIL_TURBO       = 27;
//     static const uint16_t COIL_QUIET       = 30;

//     // Tekrarlanan Modbus işlemlerini basitleştirmek için özel yardımcı fonksiyonlar
//     uint16_t readHoldingRegister(uint16_t reg);
//     bool writeHoldingRegister(uint16_t reg, uint16_t value);
//     bool readCoil(uint16_t coil);
//     bool writeCoil(uint16_t coil, bool state);
// };

// #endif // GREE_UMATCH_MODBUS_HPP