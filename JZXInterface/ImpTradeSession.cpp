//
// Created by hua on 2016/6/8.
//

#include "ImpTradeSession.h"
#include "../com/time_utlity.h"

#include <c++/iostream>
#include <c++/sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

ImpTradeSession::ImpTradeSession() : _handle(0),_worker_running(false),_startTime(83000),_endTime(153000),
                                     _heartBtInt(30),_numOfTraders(1),_nProtocol(0),_nPort(0),_timeout(0),
                                     _encryptType(0),_is_login(false){
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
    boost::filesystem::path xmlfile(boost::filesystem::initial_path().append("JZXInterface.xml"));
    if(boost::filesystem::exists(xmlfile)) {
        namespace pt = boost::property_tree;
        pt::ptree tree;
        try {
            pt::read_ini(xmlfile.string(), tree);
            _startTime = tree.get(sessionName + ".StartTime", _startTime);
            _endTime = tree.get(sessionName + ".EndTime", _endTime);
            _storePath = tree.get<std::string>(sessionName + ".StorePath");
            _heartBtInt = tree.get(sessionName + ".HeartBtInt", 10);
            _numOfTraders = tree.get(sessionName + ".NumOfTraders", 1);
            _szServerName = tree.get<std::string>(sessionName + ".szServerName");
            _nProtocol = tree.get(sessionName + ".nProtocol", 0);
            _szAddress = tree.get<std::string>(sessionName + ".szAddress");
            _nPort = tree.get(sessionName + ".nPort", 0);
            _szSendQName = tree.get<std::string>(sessionName + ".szSendQName");
            _szReceiveQName = tree.get<std::string>(sessionName + ".szReceiveQName");
            _szReserved = tree.get<std::string>(sessionName + ".szReserved");
            _timeout = tree.get(sessionName + ".Timeout", 0);
            _encryptType = tree.get(sessionName + ".EncryptType", 0);
        }
        catch(std::exception &e){
            throw TradeAPI::api_issue_error(std::string("Load config file error:") + e.what());
        }
    }
    // set event receiver
    _event_receiver = receiver;
    _sessionName = sessionName;
    _account = account;
    _accountPassword = accPassword;

    // start thread
    _worker.reset(new std::thread(&ImpTradeSession::worker_procedure,this));
    _worker_running = true;
}

void ImpTradeSession::worker_procedure(void) {
    std::cout << "-> worker_procedure startup." << std::endl;
    while(_worker_running){
        int day,tm;
        get_current_dt(day,tm);

    }
    std::cout << "-> worker_procedure stopped." << std::endl;
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




