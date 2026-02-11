#pragma once

#include "ptp-base.h"

class l2PTP : public PTPBase
{
public:
    l2PTP(ClockRole role, DelayMode mode, bool forwardable_ = false, LogLevel logLevel = None);
private:
    void initSockets() override;
    void updateSockets() override;
    void sendPTPMessage(const uint8_t *buf, int size, bool generalMessage) override;

    /**
    * If true, use L2 Forwardable MAC address, 01:1B:19:00:00:00.
    * if false, use L2 Non-Forwardable, 01:80:C2:00:00:0E.
    */
    bool forwardable{false};
};
