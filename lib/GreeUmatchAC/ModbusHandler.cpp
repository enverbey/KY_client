#include "ModbusHandler.h"

// Modbus RTU fonksiyon kodları
constexpr uint8_t FUNC_READ_COILS = 0x01;
constexpr uint8_t FUNC_READ_HOLDING_REGISTERS = 0x03;
constexpr uint8_t FUNC_WRITE_MULTIPLE_COILS = 0x0F;
constexpr uint8_t FUNC_WRITE_MULTIPLE_REGISTERS = 0x10;


ModbusHandler::ModbusHandler() : _serial(nullptr), _slaveAddress(1), _deRePin(-1) {
    // Kurucu fonksiyon, başlangıçta boş olabilir.
}

void ModbusHandler::begin(HardwareSerial& serial, int8_t rxPin, int8_t txPin, int8_t deRePin, uint8_t slaveAddress) {
    _serial = &serial;
    _slaveAddress = slaveAddress;
    _deRePin = deRePin;

    // Belirtilen pinler ile seri portu başlat
    // Gree klimanın dokümanı 9600 baud rate, 8 data bits, no parity, 1 stop bit (8N1) belirtir.
    _serial->begin(9600, SERIAL_8N1, rxPin, txPin);

    // DE/RE kontrol pinini çıkış olarak ayarla ve başlangıçta LOW (alıcı modunda) yap.
    if (_deRePin != -1) {
        pinMode(_deRePin, OUTPUT);
        digitalWrite(_deRePin, LOW);
    }
}

bool ModbusHandler::writeRegister(uint16_t regAddress, uint16_t value) {
    uint8_t request[11]; // Yazma isteği çerçevesi (tek register için)

    // İstek paketini Modbus RTU formatına göre oluştur
    request[0] = _slaveAddress;
    request[1] = FUNC_WRITE_MULTIPLE_REGISTERS;
    request[2] = highByte(regAddress);
    request[3] = lowByte(regAddress);
    request[4] = 0x00; // Yazılacak register sayısı (High Byte)
    request[5] = 0x01; // Yazılacak register sayısı (Low Byte) -> 1 adet
    request[6] = 0x02; // Gönderilen verinin byte sayısı -> 1 register * 2 byte = 2
    request[7] = highByte(value);
    request[8] = lowByte(value);
    
    uint16_t crc = calculateCRC(request, 9);
    request[9] = lowByte(crc);
    request[10] = highByte(crc);

    sendRequest(request, 11);

    // Başarılı bir yazma işlemine gelen yanıt 8 byte olmalıdır.
    uint8_t response[8];
    if (!receiveResponse(response, 8)) {
        return false; // Yanıt alınamadı veya CRC hatalı
    }
    
    // Gelen yanıtın doğruluğunu kontrol et (slave adresi, fonksiyon kodu vb.)
    if (response[0] != _slaveAddress || response[1] != FUNC_WRITE_MULTIPLE_REGISTERS) {
        return false;
    }

    return true;
}

bool ModbusHandler::readRegister(uint16_t regAddress, uint16_t& value) {
    uint8_t request[8]; // Okuma isteği çerçevesi

    request[0] = _slaveAddress;
    request[1] = FUNC_READ_HOLDING_REGISTERS;
    request[2] = highByte(regAddress);
    request[3] = lowByte(regAddress);
    request[4] = 0x00; // Okunacak register sayısı (High Byte)
    request[5] = 0x01; // Okunacak register sayısı (Low Byte) -> 1 adet
    
    uint16_t crc = calculateCRC(request, 6);
    request[6] = lowByte(crc);
    request[7] = highByte(crc);

    sendRequest(request, 8);

    // 1 register okuma isteğine gelen yanıt 7 byte olmalıdır.
    // (SlaveID, FuncCode, ByteCount, DataHi, DataLo, CRCLo, CRCHi)
    uint8_t response[7];
    if (!receiveResponse(response, 7)) {
        return false;
    }

    // Gelen yanıtın doğruluğunu ve beklenen veri miktarını (2 byte) kontrol et
    if (response[0] != _slaveAddress || response[1] != FUNC_READ_HOLDING_REGISTERS || response[2] != 2) {
        return false;
    }

    // Okunan veriyi birleştir ve referans parametresine ata
    value = (response[3] << 8) | response[4];
    return true;
}

bool ModbusHandler::writeCoil(uint16_t coilAddress, bool value) {
    // Doküman, birden çok coil yazma (0x0F) fonksiyonunu destekliyor.
    // Tek bir coil yazmak için de bu fonksiyon kullanılır.
    uint8_t request[10];
    uint16_t valueToWrite = value ? 0xFF00 : 0x0000;

    request[0] = _slaveAddress;
    request[1] = FUNC_WRITE_MULTIPLE_COILS;
    request[2] = highByte(coilAddress);
    request[3] = lowByte(coilAddress);
    request[4] = 0x00; // Yazılacak coil sayısı (High)
    request[5] = 0x01; // Yazılacak coil sayısı (Low) -> 1 adet
    request[6] = 0x01; // Veri byte sayısı -> 1 coil için 1 byte yeterli
    request[7] = value ? 0x01 : 0x00; // Değer (Packed bits)

    uint16_t crc = calculateCRC(request, 8);
    request[8] = lowByte(crc);
    request[9] = highByte(crc);

    sendRequest(request, 10);
    
    // Başarılı bir coil yazma işlemine gelen yanıt 8 byte olmalıdır.
    uint8_t response[8];
    if (!receiveResponse(response, 8)) {
        return false;
    }
    
    if (response[0] != _slaveAddress || response[1] != FUNC_WRITE_MULTIPLE_COILS) {
        return false;
    }

    return true;
}

bool ModbusHandler::readCoil(uint16_t coilAddress, bool& value) {
    uint8_t request[8];

    request[0] = _slaveAddress;
    request[1] = FUNC_READ_COILS;
    request[2] = highByte(coilAddress);
    request[3] = lowByte(coilAddress);
    request[4] = 0x00; // Okunacak coil sayısı (High)
    request[5] = 0x01; // Okunacak coil sayısı (Low) -> 1 adet

    uint16_t crc = calculateCRC(request, 6);
    request[6] = lowByte(crc);
    request[7] = highByte(crc);

    sendRequest(request, 8);

    // 1 coil okuma isteğine gelen yanıt 6 byte olmalıdır.
    // (SlaveID, FuncCode, ByteCount, Data, CRCLo, CRCHi)
    uint8_t response[6];
    if (!receiveResponse(response, 6)) {
        return false;
    }
    
    if (response[0] != _slaveAddress || response[1] != FUNC_READ_COILS || response[2] != 1) {
        return false;
    }

    value = (response[3] & 0x01) ? true : false;
    return true;
}

// =========================================================================
// PRIVATE YARDIMCI FONKSİYONLAR
// =========================================================================

void ModbusHandler::sendRequest(const uint8_t* requestBuffer, uint8_t length) {
    clearSerialBuffer();

    if (_deRePin != -1) {
        digitalWrite(_deRePin, HIGH); // Gönderim modunu aktif et
        delayMicroseconds(100);       // Transceiver'ın mod değiştirmesi için kısa bir bekleme
    }

    _serial->write(requestBuffer, length);
    _serial->flush(); // Tüm byte'ların gönderildiğinden emin ol

    if (_deRePin != -1) {
        delayMicroseconds(100);       // Son byte'ın hattan tamamen çıkması için bekle
        digitalWrite(_deRePin, LOW);  // Alıcı modunu aktif et
    }
}

bool ModbusHandler::receiveResponse(uint8_t* responseBuffer, uint8_t expectedLength) {
    unsigned long startTime = millis();
    while (_serial->available() < expectedLength) {
        if (millis() - startTime > _timeout) {
            return false; // Zaman aşımı
        }
        yield(); // ESP32 için diğer task'lara zaman tanı
    }

    _serial->readBytes(responseBuffer, expectedLength);

    // CRC kontrolü
    uint16_t receivedCrc = (responseBuffer[expectedLength - 1] << 8) | responseBuffer[expectedLength - 2];
    uint16_t calculatedCrc = calculateCRC(responseBuffer, expectedLength - 2);

    return receivedCrc == calculatedCrc;
}


uint16_t ModbusHandler::calculateCRC(const uint8_t* buffer, uint8_t length) {
    uint16_t crc = 0xFFFF;
    for (uint8_t pos = 0; pos < length; pos++) {
        crc ^= (uint16_t)buffer[pos];
        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    // Düşük ve yüksek byte'ların yerini değiştir
    return (crc << 8) | (crc >> 8);
}

void ModbusHandler::clearSerialBuffer() {
    while (_serial->available()) {
        _serial->read();
    }
}