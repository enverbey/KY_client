#include "BME280.hpp"

// BME280 register adresleri
#define BME280_REG_ID         0xD0
#define BME280_REG_RESET      0xE0
#define BME280_REG_CTRL_MEAS  0xF4
#define BME280_REG_TEMP_MSB   0xFA
#define BME280_REG_TEMP_LSB   0xFB
#define BME280_REG_TEMP_XLSB  0xFC

#define BME280_REG_CALIB00    0x88

BME280::BME280(uint8_t i2cAddress)
: _address(i2cAddress), _wire(nullptr), t_fine(0),
  dig_T1(0), dig_T2(0), dig_T3(0) {}

bool BME280::begin(TwoWire &wirePort) {
    _wire = &wirePort;

    Serial.println("[BME280] I2C başlatılıyor...");
    // ESP32’de SDA=21, SCL=22 olacak şekilde başlat
    _wire->begin(21, 22);

    // ---- Chip ID oku ----
    Serial.println("[BME280] Chip ID okunuyor...");
    _wire->beginTransmission(_address);
    _wire->write(0x60);
    uint8_t txErr = _wire->endTransmission();
    if (txErr != 0) {
        Serial.print("[BME280] I2C hata kodu: ");
        Serial.println(txErr);
        return false;
    }

    _wire->requestFrom(_address, (uint8_t)1);
    if (!_wire->available()) {
        Serial.println("[BME280] ChipID okunamadı, cihaz cevap vermiyor.");
        return false;
    }
    uint8_t chipID = _wire->read();
    Serial.print("[BME280] Okunan ChipID = 0x");
    Serial.println(chipID, HEX);

    if (chipID != 0x60) {
        Serial.println("[BME280] Yanlış ChipID! (BME280 için 0x60 olmalı)");
        return false;
    }

    // ---- Kalibrasyon verilerini oku ----
    Serial.println("[BME280] Kalibrasyon değerleri okunuyor...");
    _wire->beginTransmission(_address);
    _wire->write(BME280_REG_CALIB00);
    _wire->endTransmission();
    _wire->requestFrom(_address, (uint8_t)6);

    if (_wire->available() < 6) {
        Serial.println("[BME280] Kalibrasyon verileri eksik geldi.");
        return false;
    }

    dig_T1 = (uint16_t)(_wire->read() | (_wire->read() << 8));
    dig_T2 = (int16_t)(_wire->read() | (_wire->read() << 8));
    dig_T3 = (int16_t)(_wire->read() | (_wire->read() << 8));
    Serial.println("[BME280] Kalibrasyon değerleri okundu.");

    // ---- Ölçüm modunu ayarla ----
    Serial.println("[BME280] Ölçüm moduna geçiliyor...");
    _wire->beginTransmission(_address);
    _wire->write(BME280_REG_CTRL_MEAS);
    _wire->write(0x20); // osrs_t=1, mode=forced/normal
    txErr = _wire->endTransmission();
    if (txErr != 0) {
        Serial.print("[BME280] CTRL_MEAS yazılamadı, hata kodu: ");
        Serial.println(txErr);
        return false;
    }

    Serial.println("[BME280] Başarıyla başlatıldı.");
    return true;
}

int32_t BME280::readRawTemp() {
    _wire->beginTransmission(_address);
    _wire->write(BME280_REG_TEMP_MSB);
    _wire->endTransmission();

    _wire->requestFrom(_address, (uint8_t)3);
    if (_wire->available() < 3) return 0;

    int32_t msb = _wire->read();
    int32_t lsb = _wire->read();
    int32_t xlsb = _wire->read();

    int32_t adc_T = ((msb << 12) | (lsb << 4) | (xlsb >> 4));
    return adc_T;
}

float BME280::compensateTemp(int32_t adc_T) {
    // Datasheet’teki formül
    float var1 = (((float)adc_T) / 16384.0 - ((float)dig_T1) / 1024.0) * ((float)dig_T2);
    float var2 = ((((float)adc_T) / 131072.0 - ((float)dig_T1) / 8192.0) *
                 (((float)adc_T) / 131072.0 - ((float)dig_T1) / 8192.0)) * ((float)dig_T3);
    t_fine = (int32_t)(var1 + var2);
    float T = (var1 + var2) / 5120.0;
    return T;
}

float BME280::getTemp() {
    int32_t raw = readRawTemp();
    if (raw == 0) return NAN;
    return compensateTemp(raw);
}
