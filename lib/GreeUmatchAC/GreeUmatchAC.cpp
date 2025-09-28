#include "GreeUmatchAC.h"

GreeUmatchAC::GreeUmatchAC() : _slaveAddress(0) {
    // Kurucu fonksiyon
}

void GreeUmatchAC::begin(HardwareSerial& serial, int8_t rxPin, int8_t txPin, int8_t deRePin, uint8_t slaveAddress) {
    _slaveAddress = slaveAddress;
    _modbus.begin(serial, rxPin, txPin, deRePin, _slaveAddress);
}

// --- TEMEL KONTROLLER ---

bool GreeUmatchAC::turnOn() {
    return _modbus.writeRegister(GreeUmatch::REG_POWER, GreeUmatch::POWER_ON);
}

bool GreeUmatchAC::turnOff() {
    return _modbus.writeRegister(GreeUmatch::REG_POWER, GreeUmatch::POWER_OFF);
}

bool GreeUmatchAC::setPower(bool on) {
    uint16_t powerValue = on ? GreeUmatch::POWER_ON : GreeUmatch::POWER_OFF;
    return _modbus.writeRegister(GreeUmatch::REG_POWER, powerValue);
}

bool GreeUmatchAC::setMode(uint16_t mode) {
    // Gelen mod değerinin geçerli aralıkta olup olmadığını kontrol etmek iyi bir pratiktir.
    if (mode >= GreeUmatch::MODE_COOL && mode <= GreeUmatch::MODE_AUTO) {
        return _modbus.writeRegister(GreeUmatch::REG_MODE, mode);
    }
    return false;
}

bool GreeUmatchAC::setFanSpeed(uint16_t speed) {
    if (speed >= GreeUmatch::FAN_AUTO && speed <= GreeUmatch::FAN_TURBO_FAN) {
        return _modbus.writeRegister(GreeUmatch::REG_FAN_SPEED, speed);
    }
    return false;
}

bool GreeUmatchAC::setTemperature(uint16_t temp) {
    // Sıcaklık değerini mantıklı bir aralıkta sınırla
    if (temp >= 16 && temp <= 30) {
        return _modbus.writeRegister(GreeUmatch::REG_SET_TEMP, temp);
    }
    return false;
}

// --- GELİŞMİŞ KONTROLLER VE ÖZELLİKLER ---

bool GreeUmatchAC::setSwingVertical(uint16_t swingMode) {
    return _modbus.writeRegister(GreeUmatch::REG_SWING_VERTICAL, swingMode);
}

bool GreeUmatchAC::setSwingHorizontal(uint16_t swingMode) {
    return _modbus.writeRegister(GreeUmatch::REG_SWING_HORIZONTAL, swingMode);
}

bool GreeUmatchAC::setQuietMode(bool enabled) {
    return setBoolFeature(GreeUmatch::COIL_QUIET, enabled);
}

bool GreeUmatchAC::setTurboMode(bool enabled) {
    return setBoolFeature(GreeUmatch::COIL_TURBO, enabled);
}

bool GreeUmatchAC::setXFan(bool enabled) {
    return setBoolFeature(GreeUmatch::COIL_XFAN, enabled);
}

bool GreeUmatchAC::enableDryMode() {
    return setMode(GreeUmatch::MODE_DRY);
}

// --- ENERJİ YÖNETİMİ VE LİMİTLER ---

bool GreeUmatchAC::setTempLimits(uint16_t coolMin, uint16_t heatMax) {
    bool success = true;
    success &= _modbus.writeRegister(GreeUmatch::REG_ENERGY_SAVE_COOL_MIN, coolMin);
    success &= _modbus.writeRegister(GreeUmatch::REG_ENERGY_SAVE_HEAT_MAX, heatMax);
    return success;
}

bool GreeUmatchAC::enableTempLimits(bool enabled) {
    // PDF dokümanı bu iki bitin aynı anda setlenmesini öneriyor.
    bool success = true;
    success &= setBoolFeature(GreeUmatch::COIL_ENERGY_SAVE_COOL_ON, enabled);
    success &= setBoolFeature(GreeUmatch::COIL_ENERGY_SAVE_HEAT_ON, enabled);
    return success;
}

// --- DURUM OKUMA FONKSİYONLARI ---

float GreeUmatchAC::getRoomTemperature() {
    uint16_t rawValue;
    if (_modbus.readRegister(GreeUmatch::REG_AMBIENT_TEMP, rawValue)) {
        // Dokümana göre okunan değerin 10'a bölünmesi gerekiyor.
        return (float)rawValue / 10.0f;
    }
    return -999.0f; // Hata durumunda geçersiz bir değer döndür
}

float GreeUmatchAC::getOutdoorTemperature() {
    uint16_t rawValue;
    if (_modbus.readRegister(GreeUmatch::REG_OUTDOOR_TEMP, rawValue)) {
        // Bu register da 10'a bölünerek kullanılır.
        return (float)rawValue / 10.0f;
    }
    return -999.0f; // Hata durumunda geçersiz bir değer döndür
}

bool GreeUmatchAC::isCompressorOn() {
    bool value = false;
    getBoolFeature(GreeUmatch::COIL_STATE_COMPRESSOR_ON, value);
    return value;
}

bool GreeUmatchAC::getPowerState(bool& isOn) {
    uint16_t rawValue;
    if (_modbus.readRegister(GreeUmatch::REG_POWER, rawValue)) {
        isOn = (rawValue == GreeUmatch::POWER_ON);
        return true;
    }
    return false;
}

bool GreeUmatchAC::getMode(uint16_t& mode) {
    return _modbus.readRegister(GreeUmatch::REG_MODE, mode);
}

bool GreeUmatchAC::getFanSpeed(uint16_t& speed) {
    return _modbus.readRegister(GreeUmatch::REG_FAN_SPEED, speed);
}

bool GreeUmatchAC::getSetTemperature(uint16_t& temp) {
    return _modbus.readRegister(GreeUmatch::REG_SET_TEMP, temp);
}

// --- PRIVATE YARDIMCI FONKSİYONLAR ---

bool GreeUmatchAC::setBoolFeature(uint16_t coilAddress, bool enabled) {
    return _modbus.writeCoil(coilAddress, enabled);
}

bool GreeUmatchAC::getBoolFeature(uint16_t coilAddress, bool& value) {
    return _modbus.readCoil(coilAddress, value);
}