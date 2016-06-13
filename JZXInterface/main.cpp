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

using namespace boost::filesystem;

using namespace std;

int main(int argc, char* argv[]){
    KCBPCLIHANDLE handle;
    if( KCBPCLI_Init(&handle)==0 ){
        int version=0;
        KCBPCLI_GetVersion(handle,&version);
        std::cout << version << std::endl;
        KCBPCLI_Exit(handle);
    }
    boost::filesystem::path xmlfile(boost::filesystem::initial_path().append("JZXInterface.xml"));
    if(boost::filesystem::exists(xmlfile)) {
        boost::property_tree::ptree pt;
        boost::property_tree::xml_parser::read_xml(xmlfile.string(), pt);
        int b = pt.get("session1.EndTime1", 0);
        int a = pt.get("session1.StartTime", 0);

        std::cout << a << std::endl;
    }
    return 0;
}