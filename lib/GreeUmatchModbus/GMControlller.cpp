#include "GreeUmatchModbus.hpp"

GreeUmatchModbus::GreeUmatchModbus(ModbusMaster& modbus, uint8_t slaveId)
    : _modbus(modbus), _slaveId(slaveId) {}

bool GreeUmatchModbus::powerOn() {
    uint8_t result;
    result = _modbus.writeSingleRegister(REG_POWER, VAL_POWER_ON);
    return (result == ModbusMaster::ku8MBSuccess);
}

bool GreeUmatchModbus::powerOff() {
    uint8_t result;
    result = _modbus.writeSingleRegister(REG_POWER, VAL_POWER_OFF);
    return (result == ModbusMaster::ku8MBSuccess);
}