#ifndef BME280_HPP
#define BME280_HPP

#include <Arduino.h>
#include <Wire.h>

class BME280 {
public:
    // ---- Kurucu ----
    BME280(uint8_t i2cAddress = 0x76);   // Kart üzerindeki SDO pinine göre 0x76 veya 0x77

    // ---- Başlatma ----
    bool begin(TwoWire &wirePort = Wire);

    // ---- Sıcaklık okuma ----
    float getTemp();   // °C cinsinden sıcaklık döndürür

private:
    uint8_t _address;
    TwoWire *_wire;

    // Kalibrasyon sabitleri
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    // Ham sıcaklık verisini okur
    int32_t readRawTemp();

    // Düzeltme/kompanzasyon için yardımcı
    int32_t t_fine;
    float compensateTemp(int32_t adc_T);
};

#endif
