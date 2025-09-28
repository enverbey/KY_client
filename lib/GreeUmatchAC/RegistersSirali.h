#ifndef GREE_UMATCH_REGISTERS_H
#define GREE_UMATCH_REGISTERS_H

#include <cstdint>

namespace GreeUmatch
{
    // Register Addresses
    constexpr uint16_t REG_POWER                 = 0x0002; //[WORD 2]  (W/R) Klima Aç/Kapa
    constexpr uint16_t REG_AMBIENT_TEMP          = 0x0004; //[WORD 4]  (R)   Ortam Sıcaklığı
    constexpr uint16_t REG_IDU_ADDR              = 0x0005; //[WORD 5]  (R)   İç Ünite Adresi
    constexpr uint16_t REG_MODE                  = 0x0011; //[WORD 17] (W/R) Çalışma Modu Ayarı
    constexpr uint16_t REG_FAN_SPEED             = 0x0013; //[WORD 19] (W/R) Fan Hızı Ayarı
    constexpr uint16_t REG_SET_TEMP              = 0x0014; //[WORD 20] (W/R) Hedef Sıcaklık Ayarı
    constexpr uint16_t REG_SWING_VERTICAL        = 0x0016; //[WORD 22] (W/R) Dikey Salınım Ayarı
    constexpr uint16_t REG_SWING_HORIZONTAL      = 0x0017; //[WORD 23] (W/R) Yatay Salınım Ayarı
    constexpr uint16_t REG_FRESH_AIR_VALVE       = 0x0018; //[WORD 24] (W/R) Taze Hava Damperi Durumu
    constexpr uint16_t REG_SLEEP_MODE            = 0x0019; //[WORD 25] (W/R) Uyku Modu Ayarı
    constexpr uint16_t REG_CLEANING_FUNCTION     = 0x0022; //[WORD 34] (W/R) Temizlik Fonksiyonu
    constexpr uint16_t REG_ENERGY_SAVE_COOL_MIN  = 0x0023; //[WORD 35] (W/R) Soğutma Enerji Tasarrufu Alt Limiti
    constexpr uint16_t REG_ENERGY_SAVE_HEAT_MAX  = 0x0024; //[WORD 36] (W/R) Isıtma Enerji Tasarrufu Üst Limiti
    constexpr uint16_t REG_TEMP_SENSOR_SELECT    = 0x0027; //[WORD 39] (R)  Ortam Sıcaklık Sensörü Seçimi
    constexpr uint16_t REG_OUTDOOR_TEMP          = 0x0031; //[WORD 49] (R)  Dış Ortam Sıcaklığı
    constexpr uint16_t REG_DRED_FUNCTION         = 0x004D; //[WORD 77] (R)  DRED Fonksiyon Durumu
    constexpr uint16_t REG_IDU_RETURN_AIR_TEMP   = 0x0052; //[WORD 82] (R)  İç Ünite Dönüş Havası Sıcaklığı
    constexpr uint16_t REG_PCB_TEMP              = 0x0053; //[WORD 83] (R)  İç Ünite Lamba/PCB Kartı Sıcaklığı

    constexpr uint16_t COIL_STATUS_ODU_FLAG         = 0x0008; //[BIT 8] (R) Dış Ünite Bayrak Biti
    constexpr uint16_t COIL_STATUS_MASTER_WC_FLAG   = 0x0009; //[BIT 9] (R) Ana Kablolu Kumanda Bayrak Biti
    constexpr uint16_t COIL_STATUS_SLAVE_WC_FLAG    = 0x000D; //[BIT 13] (R) Slave Kumanda Bayrak Biti
    constexpr uint16_t COIL_REMOTE_LOCK             = 0x0011; //[BIT 17] (W/R) Uzaktan Kumanda Kilitleme
    constexpr uint16_t COIL_SHIELD_TEMP             = 0x0012; //[BIT 18] (W/R) Sıcaklık Ayarını Koruma
    constexpr uint16_t COIL_SHIELD_MODE             = 0x0013; //[BIT 19] (W/R) Mod Ayarını Koruma
    constexpr uint16_t COIL_SHIELD_POWER            = 0x0014; //[BIT 20] (W/R) Açma/Kapama'yı Koruma
    constexpr uint16_t COIL_SHIELD_ENERGY_SAVE      = 0x0015; //[BIT 21] (W/R) Enerji Tasarrufu Ayarlarını Koruma
    constexpr uint16_t COIL_ABSENCE_MODE            = 0x0018; //[BIT 24] (W/R) Dışarıda/Devamsızlık Modu
    constexpr uint16_t COIL_HEALTHY_MODE            = 0x0019; //[BIT 25] (W/R) Sağlık Fonksiyonu
    constexpr uint16_t COIL_ENERGY_SAVE_COMFORT     = 0x001A; //[BIT 26] (W/R) Konforlu Enerji Tasarrufu
    constexpr uint16_t COIL_TURBO                   = 0x001B; //[BIT 27] (W/R) Süper/Turbo Modu
    constexpr uint16_t COIL_EHEATING_PROHIBIT       = 0x001C; //[BIT 28] (W/R) Elektrikli Ek Isıtıcı Yasağı
    constexpr uint16_t COIL_XFAN                    = 0x001D; //[BIT 29] (W/R) Kurutma / X-Fan Fonksiyonu
    constexpr uint16_t COIL_QUIET                   = 0x001E; //[BIT 30] (W/R) Sessiz Mod
    constexpr uint16_t COIL_LOW_TEMP_DRYING         = 0x001F; //[BIT 31] (W/R) Düşük Sıcaklıkta Nem Alma Modu
    constexpr uint16_t COIL_ENERGY_SAVE_COOL_ON     = 0x0020; //[BIT 32] (W/R) Soğutmada Enerji Tasarrufu Aktif
    constexpr uint16_t COIL_ENERGY_SAVE_HEAT_ON     = 0x0021; //[BIT 33] (W/R) Isıtmada Enerji Tasarrufu Aktif
    constexpr uint16_t COIL_BUTTON_LOCK             = 0x0022; //[BIT 34] (W/R) Düğme Kilidi (Çocuk Kilidi)
    constexpr uint16_t COIL_MEMORY_STATE            = 0x0023; //[BIT 35] (W/R) Aç/Kapa Durumunu Hafızaya Alma
    constexpr uint16_t COIL_CELSIUS_FAHRENHEIT      = 0x0024; //[BIT 36] (W/R) Celsius/Fahrenheit Seçimi
    constexpr uint16_t COIL_STATUS_TIMER_ON         = 0x0028; //[BIT 40] (R) Zamanlayıcı Durumu
    constexpr uint16_t COIL_STATUS_TIMER_ON_FLAG    = 0x0031; //[BIT 49] (R) Zamanlayıcı Açık Bayrağı
    constexpr uint16_t COIL_STATUS_TIMER_OFF_FLAG   = 0x0032; //[BIT 50] (R) Zamanlayıcı Kapalı Bayrağı
    constexpr uint16_t COIL_FAIL_WC_TEMP_SENSOR     = 0x0041; //[BIT 65] (R) Kablolu Kumanda Sensör Arızası
    constexpr uint16_t COIL_FAIL_WC_MEMORY_CHIP     = 0x0046; //[BIT 70] (R) Kablolu Kumanda Hafıza Çipi Arızası
    constexpr uint16_t COIL_CANCEL_TIMER            = 0x0048; //[BIT 72] (W/R) Zamanlayıcıyı İptal Et
    constexpr uint16_t COIL_STATE_EHEATING_ON       = 0x005B; //[BIT 91] (R) Elektrikli Ek Isıtıcı Çalışıyor
    constexpr uint16_t COIL_STATE_WATER_PUMP_ON     = 0x005C; //[BIT 92] (R) Su Pompası Çalışıyor
    constexpr uint16_t COIL_FAIL_IDU_EVAP_SENSOR    = 0x0061; //[BIT 97] (R) İç Ünite Evaporatör Sensör Arızası
    constexpr uint16_t COIL_FAIL_IDU_RET_AIR_SENSOR = 0x0062; //[BIT 98] (R) İç Ünite Dönüş Havası Sensör Arızası
    constexpr uint16_t COIL_FAIL_WATER_OVERFLOW     = 0x0065; //[BIT 101] (R) Su Taşırma Koruması/Arızası
    constexpr uint16_t COIL_FAIL_IDU_FAN            = 0x0069; //[BIT 105] (R) İç Ünite Fan Arızası
    constexpr uint16_t COIL_STATE_CLEANING_NEEDED   = 0x006C; //[BIT 108] (R) Ünitenin Temizlenmesi Gerekiyor
    constexpr uint16_t COIL_FAIL_COMM_MASTER_WC     = 0x0078; //[BIT 120] (R) Ana Kablolu Kumanda ile İletişim Arızası
    constexpr uint16_t COIL_FAIL_COMM_ODU           = 0x007A; //[BIT 122] (R) Dış Ünite ile İletişim Arızası
    constexpr uint16_t COIL_STATE_ODU_FAN_ON        = 0x0093; //[BIT 147] (R) Dış Ünite Fanı Çalışıyor
    constexpr uint16_t COIL_STATE_4WAY_VALVE_ON     = 0x0096; //[BIT 150] (R) 4 Yollu Vana Çalışıyor
    constexpr uint16_t COIL_STATE_COMPRESSOR_ON     = 0x0097; //[BIT 151] (R) Kompresör Çalışıyor
    constexpr uint16_t COIL_FAIL_DISCHARGE_TEMP     = 0x0098; //[BIT 152] (R) Kompresör Tahliye Sıcaklık Koruması
    constexpr uint16_t COIL_FAIL_FLUORINE_SHORTAGE  = 0x0099; //[BIT 153] (R) Soğutucu Akışkan Eksikliği Koruması
    constexpr uint16_t COIL_FAIL_HIGH_PRESSURE      = 0x009F; //[BIT 159] (R) Yüksek Basınç Koruması
    constexpr uint16_t COIL_FAIL_EVAP_FROZEN        = 0x00A0; //[BIT 160] (R) Evaporatör Donma Koruması
    constexpr uint16_t COIL_STATE_SYSTEM_DEFROST    = 0x00B3; //[BIT 179] (R) Sistem Buz Çözme (Defrost) Yapıyor

    constexpr uint16_t POWER_ON  = 0x00AA; // Güç Açık
    constexpr uint16_t POWER_OFF = 0x0055; // Güç Kapalı

    constexpr uint16_t MODE_COOL = 1;      // Soğutma [cite: 494]
    constexpr uint16_t MODE_HEAT = 2;      // Isıtma [cite: 494]
    constexpr uint16_t MODE_DRY  = 3;      // Nem Alma (Kurutma) [cite: 494]
    constexpr uint16_t MODE_FAN  = 4;      // Sadece Fan [cite: 494]
    constexpr uint16_t MODE_AUTO = 5;      // Otomatik [cite: 494]
    
    constexpr uint16_t FAN_AUTO      = 0;  // Otomatik Hız [cite: 498]
    constexpr uint16_t FAN_SPEED_1   = 1;  // Hız Kademesi 1 [cite: 498]
    constexpr uint16_t FAN_SPEED_2   = 2;  // Hız Kademesi 2 [cite: 498]
    constexpr uint16_t FAN_SPEED_3   = 3;  // Hız Kademesi 3 [cite: 498]
    constexpr uint16_t FAN_SPEED_4   = 4;  // Hız Kademesi 4 [cite: 498]
    constexpr uint16_t FAN_SPEED_5   = 5;  // Hız Kademesi 5 [cite: 498]
    constexpr uint16_t FAN_TURBO_FAN = 6;  // Ultra/Turbo Hız [cite: 498]
    
    constexpr uint16_t SLEEP_OFF    = 0;   // Uyku Modu Kapalı [cite: 506]
    constexpr uint16_t SLEEP_MODE_1 = 1;   // Uyku Modu 1 [cite: 506]
    constexpr uint16_t SLEEP_MODE_2 = 2;   // Uyku Modu 2 [cite: 506]
    constexpr uint16_t SLEEP_MODE_3 = 3;   // Uyku Modu 3 [cite: 506]
    
    constexpr uint16_t SWING_V_OFF   = 0;  // Salınım Kapalı [cite: 500]
    constexpr uint16_t SWING_V_FULL  = 1;  // Tam Salınım [cite: 500]
    constexpr uint16_t SWING_V_POS_1 = 2;  // Pozisyon 1 (En Üst) [cite: 500]
    constexpr uint16_t SWING_V_POS_2 = 3;  // Pozisyon 2 [cite: 500]
    constexpr uint16_t SWING_V_POS_3 = 4;  // Pozisyon 3 [cite: 500]
    constexpr uint16_t SWING_V_POS_4 = 5;  // Pozisyon 4 [cite: 500]
    constexpr uint16_t SWING_V_POS_5 = 6;  // Pozisyon 5 (En Alt) [cite: 500]

    // Dikey Salınım - Bölgesel Modlar
    constexpr uint16_t SWING_V_3_5_SWEEP = 7;  // Bölgesel Salınım (Alt: Pozisyon 3-5 arası)
    constexpr uint16_t SWING_V_2_5_SWEEP = 8;  // Bölgesel Salınım (Orta-Alt: Pozisyon 2-5 arası)
    constexpr uint16_t SWING_V_2_4_SWEEP = 9;  // Bölgesel Salınım (Orta: Pozisyon 2-4 arası)
    constexpr uint16_t SWING_V_1_4_SWEEP = 10; // Bölgesel Salınım (Orta-Üst: Pozisyon 1-4 arası)
    constexpr uint16_t SWING_V_1_3_SWEEP = 11; // Bölgesel Salınım (Üst: Pozisyon 1-3 arası)

    // Yatay Salınım Modları
    constexpr uint16_t SWING_H_OFF           = 0;  // Salınım Kapalı
    constexpr uint16_t SWING_H_SAME_DIR      = 1;  // Aynı Yöne Tam Salınım
    constexpr uint16_t SWING_H_POS_1         = 2;  // Pozisyon 1 (En Sol)
    constexpr uint16_t SWING_H_POS_2         = 3;  // Pozisyon 2
    constexpr uint16_t SWING_H_POS_3         = 4;  // Pozisyon 3 (Orta)
    constexpr uint16_t SWING_H_POS_4         = 5;  // Pozisyon 4
    constexpr uint16_t SWING_H_POS_5         = 6;  // Pozisyon 5 (En Sağ)
    constexpr uint16_t SWING_H_1_5_SWEEP     = 12; // Bölgesel Salınım (Pozisyon 1-5 arası)
    constexpr uint16_t SWING_H_OPPOSITE_DIR  = 13; // Zıt Yöne Salınım (Ortadan yanlara)
}

#endif