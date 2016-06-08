#include "ImpTradeSession.h"

using namespace std;
using namespace TradeAPI;

extern "C" ITradeSession *create(void){
    return new ImpTradeSession();
}

extern "C" void destroy(ITradeSession * pSession) {
    if( pSession ){
        delete pSession;
    }
}
