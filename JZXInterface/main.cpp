#include <TradeAPI.h>

using namespace std;
using namespace TradeAPI;

extern "C" ITradeSession *create(void){
    return NULL;
}

extern "C" void destroy(ITradeSession * pSession) {
    if( pSession ){
        delete pSession;
    }
}
