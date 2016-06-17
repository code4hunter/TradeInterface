#include "ImpTradeSession.h"

using namespace TradeAPI;

extern "C" ITradeSession *create(void){
    return new ImpTradeSession();
}

extern "C" void destroy(ITradeSession * pSession) {
    if( pSession ){
        delete pSession;
    }
}


#include <iostream>
#include "KCBPCli.h"
#include <boost/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

class event_proxy: public TradeAPI::EventReceiver{

public:
    virtual void onMessage(MessageType type, const std::string &msg) override {
        std::cout << "*ONMESSAGE:" << type << msg << std::endl;
    }

    virtual void onOrderExecution(const long id, const Order &order, const ExecutionReportSeq &report) override {

    }

    virtual void onOrderStatus(const Order &order) override {

    }

    virtual void onFundAccountChanged(const AccountInfo &info) override {

    }

    virtual void onPositionChanged(const PositionInfo &info) override {

    }
};


int main(int argc, char* argv[]){
    EventReceiverPtr ep(new event_proxy());
    ITradeSession * session = create();
    if(session) {
        try {
            session->start("session1", "1005859", "123321", ep);
            while (1) {
                std::string cmd;
                std::cin >> cmd;
                if (cmd == "quit") {
                    break;
                }
            }
        }
        catch(std::exception &e){
            std::cout << e.what() << std::endl;
        }
        destroy(session);
    }
    return 0;
}