//
// Created by hua on 2016/6/8.
//

#include "ImpTradeSession.h"
#include "../com/time_utlity.h"
#include "encode_dll_wrapper.h"

#include <c++/sstream>
#include <c++/atomic>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/xml_parser.hpp>

std::atomic<long> gidseed = ATOMIC_VAR_INIT(1);

ImpTradeSession::ImpTradeSession() : _worker_running(false), _startTime(83000), _endTime(153000),
                                     _heartBtInt(30), _numOfTraders(1), _nProtocol(0), _nPort(0), _timeout(0),
                                     _encryptType(0), _is_login(false), _last_exec_time(0) {
}

ImpTradeSession::~ImpTradeSession() {
    stop();
}

void ImpTradeSession::stop(void) {
    std::unique_lock<std::mutex> lck(_mtx_ss);

    if (_session_manager != NULL) {
        _worker_running = false;
        _cv.notify_all();
        _session_manager->join();
        _session_manager.reset();

        for (std::list<THREADPTR>::iterator it = _traders.begin(); it != _traders.end(); ++it) {
            (*it)->join();
        }
        _traders.clear();
    }

    if (_event_receiver) {
        _event_receiver.reset();
    }
}

void ImpTradeSession::start(const std::string &sessionName, const std::string &account, const std::string &accPassword,
                            TradeAPI::EventReceiverPtr &receiver) {
    std::unique_lock<std::mutex> lck(_mtx_ss);

    // check if thread is running
    if (_session_manager) throw TradeAPI::api_issue_error("Session has started.");

    // init params
    boost::filesystem::path xmlfile(boost::filesystem::initial_path().append("JZXInterface.xml"));
    if (boost::filesystem::exists(xmlfile)) {
        namespace pt = boost::property_tree;
        pt::ptree tree;
        try {
            pt::read_xml(xmlfile.string(), tree);
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
            _orgid = tree.get<std::string>(sessionName + ".orgid");
            _operway = tree.get<std::string>(sessionName + ".operway");
            _netaddr = tree.get<std::string>(sessionName + ".netaddr");
            _username = tree.get<std::string>(sessionName + ".UserName");
            _password = tree.get<std::string>(sessionName + ".Password");
        }
        catch (std::exception &e) {
            throw TradeAPI::api_issue_error(std::string("Load config file error:") + e.what());
        }
    }
    // set event receiver
    _event_receiver = receiver;
    _sessionName = sessionName;
    _account = account;
    _accountPassword = accPassword;

    _worker_running = true;

    // start traders
    if (_numOfTraders > 10) _numOfTraders = 10;
    for (int i = 0; i < _numOfTraders; i++) {
        _traders.push_back(THREADPTR(new std::thread(&ImpTradeSession::trader_procedure, this)));
    }

    // start thread
    _session_manager.reset(new std::thread(&ImpTradeSession::session_manager_procedure, this));
}

void ImpTradeSession::session_manager_procedure(void) {
    std::cout << "-> SESSION_MANAGER_PROCEDURE STARTUP." << std::endl;
    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    while (_worker_running) {
        try {
            if (_cv.wait_for(lck, std::chrono::seconds(2)) == std::cv_status::timeout) {
                int day, tm;
                get_current_dt(day, tm);
                if (tm >= this->_startTime && tm < this->_endTime) {
                    if (!this->_is_login) {
                        this->login();
                        this->_is_login = true;
                    }
                    else {
                        // check _heartBtInt
                        if (_last_exec_time > tm) _last_exec_time = tm;
                        if (second_between_time(_last_exec_time, tm) >= _heartBtInt) {
                            //调用心跳
                            std::cout << "-> WORKER_PROCEDURE HEARTBEATS:" << tm << std::endl;
                            pub_message(TradeAPI::MessageType::MTInfo, "WORKER_PROCEDURE HEARTBEATS AT: %d", tm);
                            call_410502();
                        }
                    }
                }
                else {
                    if (this->_is_login) {
                        this->_is_login = false;
                        this->logout();
                    }
                }
            }
            else {
                //执行查询成交，查询资金等的任务
            }
        }
        catch (std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }
    std::cout << "-> SESSION_MANAGER_PROCEDURE STOPPED." << std::endl;
}

TradeAPI::OrderSeq ImpTradeSession::qryWorkingOrders(void) {
    return TradeAPI::OrderSeq();
}

TradeAPI::PositionInfoSeq ImpTradeSession::qryPositions(const std::string &mkt) {
    return TradeAPI::PositionInfoSeq();
}

void ImpTradeSession::newOrderSingle(TradeAPI::OrderPtr &ord) {
    check_and_add_order(ord);
    _cv.notify_one();
}

void ImpTradeSession::cancelOrderSingle(const TradeAPI::OrderPtr &ord) {
    _cv.notify_one();
}

void ImpTradeSession::reqExecReport(const TradeAPI::ResumeType type, const std::string &seqId) {

}

void ImpTradeSession::cancelEvents(const long events) {

}

void ImpTradeSession::subscribeEvents(const long events, TradeAPI::ResumeType rt) {

}

TradeAPI::InstrumentDetailsDict ImpTradeSession::qryInstruments(const std::string &mkt, const std::string &secType) {
    std::string program = "410203";
    KCBPCLIHANDLE handle = connect_gateway();
    int_request(handle, program);
    // set function request parameter
    KCBPCLI_SetValue(handle, "market", "1");
    KCBPCLI_SetValue(handle, "stkcode", "");
    // execute request
    record_set result;
    exec_request(handle, program, result);
    disconnect_gateway(handle);
    // process answer
    result.show_data();

    return TradeAPI::InstrumentDetailsDict();
}

void ImpTradeSession::cancelExeReport(void) {

}

void ImpTradeSession::raise_api_error(const std::string &sender, KCBPCLIHANDLE handle, int code) {
    if (handle == NULL) return;
    std::ostringstream os;
    os << "HANDLE:" << handle << " FUNCTION:" << sender << " CODE:" << code;

    throw TradeAPI::api_issue_error(os.str());
}

void ImpTradeSession::raise_api_remote_error(const std::string &sender, KCBPCLIHANDLE handle) {
    if (handle == NULL) return;
    int errCode;
    char *errMsg = NULL;
    std::ostringstream os;
    if (0 == KCBPCLI_GetErr(handle, &errCode, errMsg)) {
        os << "HANDLE:" << handle << " FUNCTION:" << sender << " CODE:" << errCode
        << " MSG:" << ((NULL == errMsg) ? "" : errMsg);
    }
    else {
        os << "HANDLE:" << handle << " FUNCTION:" << sender << " KCBPCLI_GetErr failed!";
    }
    throw TradeAPI::api_issue_error(os.str());
}

void ImpTradeSession::login(void) {
    std::string program = "410301";
    KCBPCLIHANDLE handle = connect_gateway();
    int_request(handle, program);
    // set function request parameter
    KCBPCLI_SetValue(handle, "inputtype", "Z");//登录类型	inputtype	char(1)	Y	见备注
    KCBPCLI_SetValue(handle, "inputid", (char *) _account.c_str());//登录标识	inputid	char(64)	Y	见备注
    // execute request
    record_set result;
    exec_request(handle, program, result);
    disconnect_gateway(handle);
    // process answer
    result.show_data();
    _cust_id = result.get_field_value("custid");
    char enpassword[255];
    memset(enpassword, 0, 255);
    encode_dll_wrapper::instance()->Encrypt(_accountPassword.c_str(), enpassword, _cust_id.c_str(), _encryptType);
    _encode_password.assign(enpassword);
}

void ImpTradeSession::logout(void) {
    _is_login = false;
    _cust_id = "";
    _encode_password = "";
}

void ImpTradeSession::int_request(KCBPCLIHANDLE handle, const std::string &funcId) {
    int ret;
    if ((ret = KCBPCLI_BeginWrite(handle)) != 0) {
        disconnect_gateway(handle);
        this->raise_api_error("KCBPCLI_BeginWrite", handle, ret);
    }

    if (funcId == "410301") {
        KCBPCLI_SetValue(handle, "funcid", (char *) funcId.c_str());//功能号, 必须送,不可以为空
        KCBPCLI_SetValue(handle, "custid", "");//客户代码,  可以为空 28014444,8237964
        KCBPCLI_SetValue(handle, "custorgid", (char *) _orgid.c_str());//客户机构, 可以为空
        char enpassword[255];
        memset(enpassword, 0, 255);
        encode_dll_wrapper::instance()->Encrypt(_accountPassword.c_str(), enpassword, funcId.c_str(), _encryptType);
        KCBPCLI_SetValue(handle, "trdpwd", enpassword);//交易密码, 可以为空
        KCBPCLI_SetValue(handle, "netaddr", (char *) _netaddr.c_str());//操作站点, 必须送，不可以为空
        KCBPCLI_SetValue(handle, "orgid", (char *) _orgid.c_str());//操作机构, 必须送，不可以为空
        KCBPCLI_SetValue(handle, "operway", (char *) _operway.c_str()/*"0"*/);//操作方式, 必须送，不可以为空
        KCBPCLI_SetValue(handle, "ext", "");//扩展字段, 必须送，可以为空
    }
    else {
        KCBPCLI_SetValue(handle, "funcid", (char *) funcId.c_str());//功能号, 必须送,不可以为空
        KCBPCLI_SetValue(handle, "custid", (char *) _cust_id.c_str());//客户代码,  可以为空119353,517486
        KCBPCLI_SetValue(handle, "custorgid", (char *) _orgid.c_str());//客户机构, 可以为空
        KCBPCLI_SetValue(handle, "trdpwd", (char *) _encode_password.c_str());//交易密码, 可以为空
        KCBPCLI_SetValue(handle, "netaddr", (char *) _netaddr.c_str());//操作站点, 必须送，不可以为空
        KCBPCLI_SetValue(handle, "orgid", (char *) _orgid.c_str());//操作机构, 必须送，不可以为空
        KCBPCLI_SetValue(handle, "operway", (char *) _operway.c_str());//操作方式, 必须送，不可以为空
        KCBPCLI_SetValue(handle, "ext", "");//扩展字段, 必须送，可以为空
    }
}

KCBPCLIHANDLE ImpTradeSession::connect_gateway(void) {
    KCBPCLIHANDLE handle;
    int ret;
    if ((ret = KCBPCLI_Init(&handle)) != 0) {
        raise_api_error("KCBPCLI_Init", handle, ret);
    }

    tagKCBPConnectOption stKCBPConnection;
    memset(&stKCBPConnection, 0, sizeof(stKCBPConnection));
    strncpy(stKCBPConnection.szServerName, _szServerName.c_str(), KCBP_SERVERNAME_MAX);
    stKCBPConnection.nProtocal = _nProtocol;
    strncpy(stKCBPConnection.szAddress, _szAddress.c_str(), KCBP_DESCRIPTION_MAX);
    stKCBPConnection.nPort = _nPort;
    strncpy(stKCBPConnection.szSendQName, _szSendQName.c_str(), KCBP_DESCRIPTION_MAX);
    strncpy(stKCBPConnection.szReceiveQName, _szReceiveQName.c_str(), KCBP_DESCRIPTION_MAX);
    strncpy(stKCBPConnection.szReserved, _szReserved.c_str(), KCBP_DESCRIPTION_MAX);

    if ((ret = KCBPCLI_SetConnectOption(handle, stKCBPConnection)) != 0) {
        KCBPCLI_Exit(handle);
        this->raise_api_error("KCBPCLI_SetConnectOption", handle, ret);
    }

    if ((ret = KCBPCLI_SetCliTimeOut(handle, _timeout)) != 0) {
        KCBPCLI_Exit(handle);
        this->raise_api_error("KCBPCLI_SetCliTimeOut", handle, ret);
    }

    if ((ret = KCBPCLI_SetSystemParam(handle, KCBP_PARAM_RESERVED, (char *) _orgid.c_str()))) {   //必须设置营业部代码
        KCBPCLI_Exit(handle);
        this->raise_api_error("KCBPCLI_SetSystemParam", handle, ret);
    }

    if ((ret = KCBPCLI_ConnectServer(handle, (char *) _szServerName.c_str(), (char *) _username.c_str(),
                                     (char *) _password.c_str())) != 0) {
        KCBPCLI_Exit(handle);
        this->raise_api_error("KCBPCLI_ConnectServer", handle, ret);
    }

    return handle;
}

void ImpTradeSession::disconnect_gateway(KCBPCLIHANDLE handle) {
    KCBPCLI_DisConnect(handle);
    KCBPCLI_Exit(handle);
}

void ImpTradeSession::exec_request(KCBPCLIHANDLE handle, const std::string &program, record_set &records) {
    int ret;
    if ((ret = KCBPCLI_SQLExecute(handle, (char *) program.c_str())) != 0) {
        disconnect_gateway(handle);
        this->raise_api_error("KCBPCLI_SQLExecute", handle, ret);
    }
    int day, tm;
    get_current_dt(day, tm);
    _last_exec_time = tm;

    int errCode;
    KCBPCLI_GetErrorCode(handle, &errCode);
    if (errCode != 0) {
        disconnect_gateway(handle);
        raise_api_remote_error(program, handle);
    }

    if ((ret = KCBPCLI_RsOpen(handle)) != 0) {
        disconnect_gateway(handle);
        this->raise_api_error("KCBPCLI_RsOpen", handle, ret);
    }

    KCBPCLI_SQLMoreResults(handle);

    KCBPCLI_SQLFetch(handle);

    char text[1024];
    if ((ret = KCBPCLI_RsGetColByName(handle, "CODE", text)) != 0) {
        disconnect_gateway(handle);
        this->raise_api_error("KCBPCLI_RsGetColByName", handle, ret);
    }

    if (atoi(text) != 0) {
        std::ostringstream os;
        os << "HANDLE:" << handle << " FUNCTION:" << program;
        os << " CODE :" << text;
        KCBPCLI_RsGetColByName(handle, "LEVEL", text);
        os << " LEVEL:" << text;
        KCBPCLI_RsGetColByName(handle, "MSG", text);
        os << " MSG:" << text;
        disconnect_gateway(handle);
        throw TradeAPI::api_issue_error(os.str());
    }

    if (KCBPCLI_SQLMoreResults(handle) == 0) {
        int nCol;
        int nRow;
        KCBPCLI_RsGetColNum(handle, &nCol);
        KCBPCLI_RsGetRowNum(handle, &nRow);
        records.resize(nCol, nRow);
        size_t iRow = 1;
        while (!KCBPCLI_RsFetchRow(handle)) {
            for (size_t i = 1; i <= nCol; i++) {
                if (iRow == 1) {
                    char colName[512];
                    KCBPCLI_RsGetColName(handle, i, colName, sizeof(colName));
                    records.add_field_name(colName);
                }
                char value[512];
                KCBPCLI_RsGetCol(handle, i, value);
                records.set_field_value(i - 1, value, iRow);
            }
            iRow++;
        }
    }

    KCBPCLI_SQLCloseCursor(handle);
}

void ImpTradeSession::call_410502(void) {
    std::string program = "410502";
    KCBPCLIHANDLE handle = connect_gateway();
    int_request(handle, program);
    // set function request parameter
    KCBPCLI_SetValue(handle, "fundid", "");
    KCBPCLI_SetValue(handle, "moneytype", "");
    // execute request
    record_set result;
    exec_request(handle, program, result);
    disconnect_gateway(handle);
    // process answer
    result.show_data();
}

void ImpTradeSession::trader_procedure(void) {
    std::cout << "-> TRADER_PROCEDURE STARTUP." << std::endl;
    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    while (_worker_running) {
        _cv.wait(lck);
        if (this->_is_login) {
            try {
                //获得命令并执行
                std::unique_lock<std::mutex> lck(_mtx_all_orders);
                TradeAPI::OrderPtr &ord =_all_orders.front();
                _all_orders.pop();
                lck.unlock();
                this->exec_order(ord);
            }
            catch (std::exception &e) {
                std::cout << e.what() << std::endl;
            }
        }
    }
    std::cout << "-> TRADER_PROCEDURE STOPPED." << std::endl;
}

void ImpTradeSession::pub_message(TradeAPI::MessageType type, const char *__format, ...) {
    if (_event_receiver == NULL) return;

    va_list argptr;
    char *text;
    va_start(argptr, __format);
    int len = _vscprintf(__format, argptr) + 1; // _vscprintf doesn't count+ 1; // terminating '\0'
    text = (char *) malloc(len * sizeof(char));
    vsnprintf(text, len, __format, argptr);
    va_end(argptr);
    std::string msg(text);
    free(text);
    _event_receiver->onMessage(type, msg);
}

void ImpTradeSession::check_and_add_order(TradeAPI::OrderPtr &ord) {
    assert(ord != NULL);
    ord->id = gidseed++;
    if (ord->account.size() == 0)
        ord->account = this->_account;
    else {
        if (ord->account != this->_account)
            throw TradeAPI::api_issue_error("Can not find this account:" + ord->account);
    }
    int d, t;
    get_current_dt(d, t);
    ord->createDate = d;
    ord->createTime = t;
    if (ord->instId.size() == 0) {
        throw TradeAPI::api_issue_error("Can not find InstId");
    }
    if (ord->ordQty <= 0) {
        throw TradeAPI::api_issue_error("Bad ordQty!");
    }
    if (ord->type != "Limit" || ord->type != "Market") {
        throw TradeAPI::api_issue_error("Bad Order type:" + ord->type);
    }
    if (ord->type == "Limit") {
        if (ord->lmtPrice <= 0) {
            throw TradeAPI::api_issue_error("Bad lmtPrice!");
        }
    }
    if (ord->posEfct != "Open" || ord->posEfct != "Close" || ord->posEfct != "CloseToday" || ord->posEfct.size() != 0) {
        throw TradeAPI::api_issue_error("Bad posEfct:" + ord->posEfct);
    }
    if (ord->side != "Buy" || ord->side != "Sell" || ord->side != "Subscribe" || ord->side != "Redeem" ||
        ord->side != "LendBuy" ||
        ord->side != "BorrowSell" || ord->side != "SellRepayment" ||
        ord->side != "BuyGiveBack" || ord->side != "GiveBack" || ord->side != "Repayment") {
        throw TradeAPI::api_issue_error("Bad side:" + ord->side);
    }
    if( ord->report.ordStatus != TradeAPI::OrderStatus::OSNew){
        throw TradeAPI::api_issue_error("Bad ordStatus, not OSNew!");
    }
    std::unique_lock<std::mutex> lck(_mtx_all_orders);
    _all_orders.push(ord);
}

void ImpTradeSession::exec_order(TradeAPI::OrderPtr &ord) {
    //发送委托到三方接口
    //获得委托号
    //更新状态
    ord->report.ordStatus = OrderStatus::OSPendingNew;
}


























