//
// Created by hua on 2016/6/8.
//

#ifndef JZXINTERFACE_IMPTRADESESSION_H
#define JZXINTERFACE_IMPTRADESESSION_H

#include <TradeAPI.h>

class ImpTradeSession: public TradeAPI::ITradeSession{
public:
    ImpTradeSession();

    ~ImpTradeSession();

    void start(const std::string &sessionName, const std::string &account, const std::string &accPassword,
               TradeAPI::EventReceiver *receiver);

    void stop(void);

    TradeAPI::InstrumentDetailsDict qryInstruments(const std::string &mkt,const std::string &secType);

    void subscribeEvents(const long events, TradeAPI::ResumeType rt);

    void cancelEvents(const long events);

    void reqExecReport(const TradeAPI::ResumeType type, const std::string &seqId);

    void cancelExeReport(void);

    TradeAPI::OrderPtr newOrderSingle(const TradeAPI::Order &ord);

    void cancelOrderSingle(const TradeAPI::OrderPtr &ord);

    TradeAPI::OrderSeq qryWorkingOrders(void);

    TradeAPI::PositionInfoSeq qryPositions(const std::string &mkt);
};


#endif //JZXINTERFACE_IMPTRADESESSION_H
