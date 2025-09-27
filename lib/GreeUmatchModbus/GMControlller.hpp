#ifndef GREE_UMATCH_MODBUS_HPP
#define GREE_UMATCH_MODBUS_HPP

#include <Arduino.h>
#include <ModbusMaster.h>

class GreeUmatchModbus {
public:
    GreeUmatchModbus(ModbusMaster& modbus, uint8_t slaveId);
    ~GreeUmatchModbus() = default;

    // Cihaz Kontrolü
    bool powerOn();
    bool powerOff();
private:
    // Modbus iletişiminde kullanılacak sabitler
    static const uint16_t REG_POWER = 0x0002;          // Güç (Aç/Kapa) register adresi 
    static const uint16_t VAL_POWER_ON = 0xAA;         // Açma komutu için değer 
    static const uint16_t VAL_POWER_OFF = 0x55;        // Kapatma komutu için değer 

    // Sınıf üyeleri
    ModbusMaster& _modbus;
    uint8_t _slaveId;
};

#endif // GMCONTROLLER_HPP