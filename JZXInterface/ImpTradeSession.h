//
// Created by hua on 2016/6/8.
//

#ifndef JZXINTERFACE_IMPTRADESESSION_H
#define JZXINTERFACE_IMPTRADESESSION_H

#include "../com/TradeAPI.h"
#include "KCBPCli.h"

#include <thread>

class ImpTradeSession: public TradeAPI::ITradeSession{
public:
    ImpTradeSession();

    ~ImpTradeSession();

    void start(const std::string &sessionName, const std::string &account, const std::string &accPassword,
               TradeAPI::EventReceiverPtr &receiver);

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

private:
    KCBPCLIHANDLE _handle;  //hcxp handle
    bool _worker_running;
    TradeAPI::EventReceiverPtr _event_receiver;
    std::string _sessionName ;
    std::string _account;
    std::string _accountPassword;
    int _startTime;
    int _endTime;
    std::string _storePath;
    int _heartBtInt;
    int _numOfTraders;
    std::string _szServerName;
    int _nProtocol;
    std::string _szAddress;
    int _nPort;
    std::string _szSendQName;
    std::string _szReceiveQName;
    std::string _szReserved;
    int _timeout;
    int _encryptType;

    std::shared_ptr<std::thread> _worker;
    void raise_api_error(const std::string &sender);

    void worker_procedure(void);
};


#endif //JZXINTERFACE_IMPTRADESESSION_H
