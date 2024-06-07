#include "MotorisedFaderInterfaceMaster.hpp"
// #include <i2c_driver_wire.h>
#include <Wire.h>

const uint8_t number_of_faders = 3;
const uint8_t slave_addr = 8;

void MotorisedFaderInterfaceMasterSingle::begin() {
}

void MotorisedFaderInterfaceMasterSingle::setFaderValueFromDAW(int faderIndex, uint16_t faderValue) {
    setpoint[faderIndex] = faderValue;
}

uint16_t MotorisedFaderInterfaceMasterSingle::getFaderValue(int faderIndex) {
    return faderPositions[faderIndex];
}

bool MotorisedFaderInterfaceMasterSingle::getFaderTouched(int faderIndex) {
    return touched[faderIndex];
}

void MotorisedFaderInterfaceMasterSingle::readAllFadersFromSlave() {

    // // Request the positions from the slave over I²C
    uint8_t maxlen = 1 + (number_of_faders)*2;
    Wire.requestFrom(slave_addr, maxlen);
    uint8_t buf[maxlen];
    uint8_t i = 0;
    while (Wire.available() && i < maxlen)
        buf[i++] = Wire.read();

    // If we received the “touch” byte (was there more than one byte returned from the motor controller)
    if (i >= 1) {

        // extract the data :)
        for (int fader_index = 0; fader_index < number_of_faders; fader_index++) {

            bool is_touched = buf[0] & (1 << fader_index);
            touched[fader_index] = is_touched;

            // now extract the position values for each of the faders
            uint16_t fader_position = buf[1 + fader_index * 2];
            fader_position = (fader_position << 8) | buf[2 + fader_index * 2];
            faderPositions[fader_index] = fader_position;
        }
    }
}

void MotorisedFaderInterfaceMasterSingle::sendDAWFaderToSlave(int faderIndex, uint16_t setPointToSend) {

    // Send it to the slave over I²C
    uint16_t idx = faderIndex;
    uint16_t data = setPointToSend;
    data |= idx << 12;
    uint16_t retrieve_data = data;
    retrieve_data &= 0x03FF;

    Wire.beginTransmission(slave_addr);
    Wire.write(reinterpret_cast<const uint8_t *>(&data), 2);
    Wire.endTransmission();
}





MotorisedFaderInterfaceMasterSingle &MotorisedFaderInterfaceMasterSingle::getInstance() {
    static MotorisedFaderInterfaceMasterSingle instance;
    return instance;
}

MotorisedFaderInterfaceMasterSingle &MotorisedFaderInterfaceMaster = MotorisedFaderInterfaceMaster.getInstance();
