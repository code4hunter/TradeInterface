//
// Created by hua on 2016/6/8.
//

#ifndef JZXINTERFACE_IMPTRADESESSION_H
#define JZXINTERFACE_IMPTRADESESSION_H

#include <TradeAPI.h>

class ImpTradeSession: public TradeAPI::ITradeSession, TradeAPI::EventReceiver {
public:
    ImpTradeSession();

    ~ImpTradeSession();

    void start(const std::string &sessionName, const std::string &account, const std::string &accPassword,
               TradeAPI::EventReceiver *receiver);

    void stop(void);

    TradeAPI::InstrumentDetailsDict qryInstrument(const std::string &filter);

    void subscribeEvents(const long events, TradeAPI::ResumeType rt);

    void cancelEvents(const long events);

    void reqExecReport(const TradeAPI::ResumeType type, const std::string &seqId);

    void cancelExeReport(void);

    TradeAPI::OrderPtr newOrderSingle(const TradeAPI::Order &ord);

    void cancelOrderSingle(const std::string &orderId);

    TradeAPI::OrderSeq qryWorkingOrders(void);

    TradeAPI::PositionInfoSeq qryPositions(const std::string &mkt);

private:
    virtual void onMessage(TradeAPI::MessageType type, const std::string &msg) override {

    }

    virtual void onOrderExecution(const long id, const TradeAPI::Order &order,
                                  const TradeAPI::ExecutionReportSeq &report) override {

    }

    virtual void onOrderStatus(const TradeAPI::Order &order) override {

    }

    virtual void onFundAccountChanged(const TradeAPI::AccountInfo &info) override {

    }

    virtual void onPositionChanged(const TradeAPI::PositionInfo &info) override {

    }
};


#endif //JZXINTERFACE_IMPTRADESESSION_H
