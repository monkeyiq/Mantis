
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp> 
#include <boost/bind.hpp>
#include <boost/integer.hpp>
using namespace boost;
using namespace boost::asio;

#include <string>
#include <iostream>
using namespace std;

#include "RoboClaw.h"

int main( int argc, char** argv )
{
    std::string serialDev = "/dev/roboclaw";
    
    if( argc > 1 )
    {
        serialDev = argv[1];
    }
    cerr << "serialDevice: " << serialDev << endl;
    
    boost::asio::io_service io;
    RoboClaw rc( io, serialDev );

    for( int i=0; i<10; i++ ) 
    {
        cout << "version        : " << rc.getVersion() << endl;
        cout << "battery voltage: " << rc.getBatteryVoltageMain() << endl;
//        std::pair< float, float > motorAmps = rc.getMotorCurrents();
//        cout << "amps A         : " << motorAmps.first << endl;
//        cout << "amps B         : " << motorAmps.second << endl;
        cout << "temperature    : " << rc.getTemperature() << endl;
        sleep(1);
    }
    
    return 0;
}
