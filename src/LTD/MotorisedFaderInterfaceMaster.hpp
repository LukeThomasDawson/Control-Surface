#pragma once

#include <stdint.h>

class MotorisedFaderInterfaceMasterSingle {
  private:
    MotorisedFaderInterfaceMasterSingle() = default;  // Make constructor private

  public:
    static MotorisedFaderInterfaceMasterSingle &getInstance();

    MotorisedFaderInterfaceMasterSingle(const MotorisedFaderInterfaceMasterSingle &) = delete;  // no copying
    MotorisedFaderInterfaceMasterSingle &operator=(const MotorisedFaderInterfaceMasterSingle &) = delete;

  public:
    void begin();

    void setFaderValueFromDAW(int faderIndex, uint16_t faderValue);
    uint16_t getFaderValue(int faderIndex);
    bool getFaderTouched(int faderIndex);

    void readAllFadersFromSlave();
    void sendAllDAWFadersToSlave();
    void sendDAWFaderToSlave(int faderIndex, uint16_t setPointToSend);

  protected:
    bool touched[4];
    uint16_t faderPositions[4] = {0};
    uint16_t setpoint[4] = {512};
};

extern MotorisedFaderInterfaceMasterSingle &MotorisedFaderInterfaceMaster;