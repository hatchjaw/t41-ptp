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
    eventSocket = new qindesign::network::EthernetUDP;
    generalSocket = new qindesign::network::EthernetUDP;

    eventSocket->beginMulticast(adr, eventPort, true);
    generalSocket->beginMulticast(adr, generalPort, true);

    if(delayMode == DelayMode::P2P){
        pEventSocket = new qindesign::network::EthernetUDP;
        pGeneralSocket = new qindesign::network::EthernetUDP;
        pEventSocket->beginMulticast(pAdr, eventPort, true);
        pGeneralSocket->beginMulticast(pAdr, generalPort, true);
    }
}

void l3PTP::updateSockets()
{
    const int esize = eventSocket->parsePacket();
    if (esize > 0)
    {
        uint8_t ebuf[esize];
        timespec erecv_ts;

        if (eventSocket->readWithTimestamp(ebuf, esize, &erecv_ts) > 0) {
            parsePTPMessage(ebuf, esize, erecv_ts);
        }
    }

    const int gsize = generalSocket->parsePacket();
    if (gsize > 0)
    {
        uint8_t gbuf[gsize];
        timespec grecv_ts;

        if (generalSocket->readWithTimestamp(gbuf, gsize, &grecv_ts) > 0) {
            parsePTPMessage(gbuf, gsize, grecv_ts);
        }
    }
    if(delayMode == DelayMode::P2P){
        const int esize = pEventSocket->parsePacket();
        if (esize > 0)
        {
            uint8_t ebuf[esize];
            timespec erecv_ts;

            if (eventSocket->readWithTimestamp(ebuf, esize, &erecv_ts) > 0) {
                parsePTPMessage(ebuf, esize, erecv_ts);
            }
        }

        const int gsize = pGeneralSocket->parsePacket();
        if (gsize > 0)
        {
            uint8_t gbuf[gsize];
            timespec grecv_ts;

            if (generalSocket->readWithTimestamp(gbuf, gsize, &grecv_ts) > 0) {
                parsePTPMessage(gbuf, gsize, grecv_ts);
            }
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
