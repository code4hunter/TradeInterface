//
// Created by hua on 2016/6/8.
//

#include <c++/iostream>
#include <c++/sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "ImpTradeSession.h"

ImpTradeSession::ImpTradeSession() : _handle(0),_worker_running(false),_startTime(83000),_endTime(153000),
                                     _heartBtInt(30),_numOfTraders(1),_nProtocol(0),_nPort(0),_timeout(0),
                                     _encryptType(0){
    int ret = KCBPCLI_Init(&_handle);
    if( ret==0 ) {
        std::cout << "KCBPCLI_Init:" << _handle << std::endl;
    }
    else {
        raise_api_error("KCBPCLI_Init");
    }
}

ImpTradeSession::~ImpTradeSession() {
    if(_handle!=NULL){
        KCBPCLIHANDLE t = _handle;
        _handle = NULL;
        KCBPCLI_Exit(t);
        std::cout << "KCBPCLI_Exit:" << t << std::endl;
    }
}

void ImpTradeSession::stop(void) {
    if(_worker == NULL) throw TradeAPI::api_issue_error("Session is not exists.");
    _worker_running = false;
    _worker->join();
    _worker.reset();
    _event_receiver.reset();
}

void ImpTradeSession::start(const std::string &sessionName, const std::string &account, const std::string &accPassword,
                            TradeAPI::EventReceiverPtr &receiver) {
    // check if thread is running
    if(_worker) throw TradeAPI::api_issue_error("Session has started.");

    // init params
    namespace pt = boost::property_tree;
    pt::ptree tree;
    pt::read_ini("ZJXInterface.ini",tree);
    _startTime = tree.get<int>(sessionName + ".StartTime");
    _endTime = tree.get<int>(sessionName + ".EndTime");

    // set event receiver
    _event_receiver = receiver;
    _sessionName = sessionName;
    _account = account;
    _accountPassword = accPassword;

    // start thread
    _worker.reset(new std::thread(&ImpTradeSession::worker_procedure,this));
    _worker_running = true;
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

void ImpTradeSession::cancelOrderSingle(const TradeAPI::OrderPtr &ord) {

}

void ImpTradeSession::reqExecReport(const TradeAPI::ResumeType type, const std::string &seqId) {

}

void ImpTradeSession::cancelEvents(const long events) {

}

void ImpTradeSession::subscribeEvents(const long events, TradeAPI::ResumeType rt) {

}

TradeAPI::InstrumentDetailsDict ImpTradeSession::qryInstruments(const std::string &mkt,const std::string &secType) {
    return TradeAPI::InstrumentDetailsDict();
}

void ImpTradeSession::cancelExeReport(void) {

}

void ImpTradeSession::raise_api_error(const std::string &sender) {
    if( _handle == NULL) return;
    int errCode;
    char *errMsg= NULL;
    std::ostringstream os;
    if( 0 == KCBPCLI_GetErr(_handle,&errCode,errMsg)){
        os << "HANDLE:" << _handle << " FUNCTION:" << sender << " CODE:" << errCode << " MSG:" << ((errMsg==NULL)?"":errMsg);
    }
    else{
        os << "HANDLE:" << _handle << " FUNCTION:" << sender << " KCBPCLI_GetErr failed!";
    }
    throw TradeAPI::api_issue_error(os.str());
}

void ImpTradeSession::worker_procedure(void) {

}





