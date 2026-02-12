#pragma once

#include <Arduino.h>
#include <TimeLib.h>

using NanoTime = int64_t;

constexpr NanoTime NS_PER_S = 1000*1000*1000;

inline void printTime(const NanoTime t)
{
    NanoTime x = t;
    const int ns = x % 1000;
    x /= 1000;
    const int us = x % 1000;
    x /= 1000;
    const int ms = x % 1000;
    x /= 1000;

    tmElements_t tme;
    breakTime((time_t)x, tme);

    Serial.printf("%02d.%02d.%04d %02d:%02d:%02d::%03d:%03d:%03d\n", tme.Day, tme.Month, 1970 + tme.Year, tme.Hour, tme.Minute, tme.Second, ms, us, ns);
}

enum class ClockRole
{
    Authority,
    Subscriber
};

enum class DelayMode
{
    E2E,
    P2P
};

enum LogLevel {
    None = 0,
    Low = 1,
    Medium,
    High
};

class PTPBase : public Printable
{
public:
    PTPBase(ClockRole role, DelayMode mode, LogLevel logLevel = None);
    void begin();
    void update();
    void reset();
    void setKi(double val);
    void setKp(double val);
    ClockRole getClockRole() const;
    NanoTime getOffset() const;
    NanoTime getDelay() const;
    double getAdjust() const;
    double getDrift() const;
    int getAccumulatedOffset() const;
    void syncMessage();
    void announceMessage();
    void ppsInterruptTriggered(NanoTime pps_ts, NanoTime local_ts);
    int getLockCount() const;
    size_t printTo(Print &p) const override;
    void onControllerUpdated(const std::function<void(double state)> &callback);

protected:
    virtual void initSockets()=0;
    virtual void updateSockets()=0;
    virtual void sendPTPMessage(const uint8_t *buf, int size, bool generalMessage)=0;
    
    void parsePTPMessage(const uint8_t *buf, int size, const timespec &recv_ts);
    ClockRole clockRole;
    DelayMode delayMode;
    
private:
	void setT1(NanoTime ts);
	void setT2(NanoTime ts);
	void setT3(NanoTime ts);
	void setT4(NanoTime ts);
    void parseSyncMessage(const uint8_t *buf, const timespec &recv_ts);
    void parseFollowUpMessage(const uint8_t *buf);
    void parseDelayResponseMessage(const uint8_t *buf, const timespec &recv_ts);
    void parseDelayResponseFollowUpMessage(const uint8_t *buf);
    void parseDelayRequestMessage(const uint8_t *buf, const timespec &recv_ts);
    
    void delayRequestMessage();
    void followUpMessage(const timespec &send_ts);
    void delayResponseMessage(const uint8_t *request_buf, uint16_t sequenceID, const timespec &request_recv_ts);
    void initPTPMessage(uint8_t *buf, const uint16_t messageLength, const uint8_t messageType, const uint16_t sequenceID, const uint8_t controlField);
    void updateController();
    void updateTimer();
    void updatePPS();

    uint8_t clockID[8]{};
    bool initialised=false;
    uint16_t delayRequestSequenceID = 0;
	int lockcount=0;
    uint16_t syncSequenceID=0;
    uint16_t syncServerSequenceID = 0;
    uint16_t announceServerSequenceID = 0;
    uint16_t followUpSequenceID=0;
    NanoTime t1=-1;
    NanoTime t1last = -1;
    NanoTime t2 = -1;
    NanoTime t2last = -1;
    NanoTime t2new = -1;
    NanoTime t3 = -1;
    NanoTime t4 = -1;
    NanoTime t5 = -1;
    NanoTime t6 = -1;

    NanoTime t1s = -1;
    NanoTime t4s = -1;

    bool t1updated=false;
    bool t2updated=false;
    bool t3updated=false;
    bool t4updated=false;
    bool t5updated=false;
    bool t6updated=false;
    bool t1lastvalid=false;
    bool t2lastvalid=false;
    bool ppsupdated=false;

    NanoTime currentOffset=0;
    NanoTime currentDelay=0;
    int nspsAccu=0;
    double driftNSPS=0;
    double nspsAdjust=0;
    double KI=0.5;
    double KP=1.0;
    int updateCounter=0;
    LogLevel logging{None};

    std::function<void(double state)> controllerUpdatedCallback{nullptr};
};
