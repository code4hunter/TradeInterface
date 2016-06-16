#include <c++/iostream>
#include "../com/TradeAPI.h"


using namespace TradeAPI;

class event_proxy: public TradeAPI::EventReceiver{

public:
    virtual void onMessage(MessageType type, const std::string &msg) override {

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
            session->start("session1", "2013970", "123321", ep);
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