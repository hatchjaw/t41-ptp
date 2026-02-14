#include <QNEthernet.h>

#include "l3ptp.h"

const IPAddress adr{224, 0, 1, 129};
const IPAddress pAdr{224, 0, 0, 107};
const int eventPort = 319;
const int generalPort = 320;

l3PTP::l3PTP(ClockRole role, DelayMode mode, LogLevel logLevel)
    : PTPBase(role, mode, logLevel)
{
}

void l3PTP::initSockets()
{
    eventSocket = new qindesign::network::EthernetUDP{4};
    generalSocket = new qindesign::network::EthernetUDP{4};

    eventSocket->beginMulticast(adr, eventPort, true);
    generalSocket->beginMulticast(adr, generalPort, true);

    if(delayMode == DelayMode::P2P){
        pEventSocket = new qindesign::network::EthernetUDP{4};
        pGeneralSocket = new qindesign::network::EthernetUDP{4};
        pEventSocket->beginMulticast(pAdr, eventPort, true);
        pGeneralSocket->beginMulticast(pAdr, generalPort, true);
    }
}

void l3PTP::updateSockets()
{
    int esize{eventSocket->parsePacket()};
    while (esize > 0) {
        uint8_t ebuf[esize];
        timespec erecv_ts;

        if (eventSocket->readWithTimestamp(ebuf, esize, &erecv_ts) > 0) {
            parsePTPMessage(ebuf, esize, erecv_ts);
        }
        esize = eventSocket->parsePacket();
    }

    int gsize{generalSocket->parsePacket()};
    while (gsize > 0) {
        uint8_t gbuf[gsize];
        timespec grecv_ts;

        if (generalSocket->readWithTimestamp(gbuf, gsize, &grecv_ts) > 0) {
            parsePTPMessage(gbuf, gsize, grecv_ts);
        }
        gsize = generalSocket->parsePacket();
    }

    if(delayMode == DelayMode::P2P){
        int esize{pEventSocket->parsePacket()};
        while (esize > 0)
        {
            uint8_t ebuf[esize];
            timespec erecv_ts;

            if (pEventSocket->readWithTimestamp(ebuf, esize, &erecv_ts) > 0) {
                parsePTPMessage(ebuf, esize, erecv_ts);
            }
            esize = pEventSocket->parsePacket();
        }

        int gsize{pGeneralSocket->parsePacket()};
        while (gsize > 0)
        {
            uint8_t gbuf[gsize];
            timespec grecv_ts;

            if (pGeneralSocket->readWithTimestamp(gbuf, gsize, &grecv_ts) > 0) {
                parsePTPMessage(gbuf, gsize, grecv_ts);
            }
            gsize = pGeneralSocket->parsePacket();
        }
    }
}

void l3PTP::sendPTPMessage(const uint8_t *buf, int size, bool generalMessage){
    if(generalMessage){
        generalSocket->send(adr,generalPort,buf,size);
    }else{
        eventSocket->send(adr,eventPort,buf,size);
    }
}
