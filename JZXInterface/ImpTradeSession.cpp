//
// Created by hua on 2016/6/8.
//

#include "ImpTradeSession.h"

ImpTradeSession::ImpTradeSession() {

}

TradeAPI::OrderSeq ImpTradeSession::qryWorkingOrders(void) {
    return TradeAPI::OrderSeq();
}

TradeAPI::PositionInfoSeq ImpTradeSession::qryPositions(const std::string &mkt) {
    return TradeAPI::PositionInfoSeq();
}

TradeAPI::OrderPtr ImpTradeSession::newOrderSingle(const TradeAPI::Order &ord) {
    return NULL;
}

void ImpTradeSession::cancelOrderSingle(const std::string &orderId) {

}

void ImpTradeSession::reqExecReport(const TradeAPI::ResumeType type, const std::string &seqId) {

}

void ImpTradeSession::cancelEvents(const long events) {

}

void ImpTradeSession::subscribeEvents(const long events, TradeAPI::ResumeType rt) {

}

TradeAPI::InstrumentDetailsDict ImpTradeSession::qryInstrument(const std::string &filter) {
    return TradeAPI::InstrumentDetailsDict();
}

void ImpTradeSession::stop(void) {

}

void ImpTradeSession::start(const std::string &sessionName, const std::string &account, const std::string &accPassword,
                                    TradeAPI::EventReceiver *receiver) {

}

ImpTradeSession::~ImpTradeSession() {

}

void ImpTradeSession::cancelExeReport(void) {

}

