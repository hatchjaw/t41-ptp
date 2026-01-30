#pragma once

#include "ptp-base.h"

namespace qindesign
{
    namespace network
    {
        class EthernetUDP;
    }
}

class l3PTP : public PTPBase
{
public:
    l3PTP(ClockRole role, DelayMode mode);
private:
    void initSockets() override;
    void updateSockets() override;
    void sendPTPMessage(const uint8_t *buf, int size, bool generalMessage) override;

    qindesign::network::EthernetUDP *eventSocket;
    qindesign::network::EthernetUDP *generalSocket;
    qindesign::network::EthernetUDP *pEventSocket;
    qindesign::network::EthernetUDP *pGeneralSocket;
};
