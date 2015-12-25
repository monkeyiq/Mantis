#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp> 
#include <boost/bind.hpp>
#include <boost/integer.hpp>
using namespace boost;
using namespace boost::asio;

#include <string>
#include <iostream>
using namespace std;

int main( int argc, char** argv )
{
    std::string serialDev = "/dev/roboclaw";
    const uint8_t roboclawAddress = 0x80;
    
    if( argc > 1 )
    {
        serialDev = argv[1];
    }
    cerr << "serialDevice:" << serialDev << endl;
    boost::asio::io_service io;
    boost::asio::serial_port serial( io, serialDev );

    // issue the "Read Firmware Version" command 21
    // to the RoboClaw at 0x80
    uint8_t commands[] = { roboclawAddress, 21 };
    write( serial, boost::asio::buffer(commands, 2));

    // give the RoboClaw heaps of time to reply.
    sleep(1);

    // read the result
    string ver = "";
    char c = 0;
    bool reading = true;
    while( reading )
    {
        asio::read(serial,asio::buffer(&c,1));
        switch(c)
        {
            case '\0':
                // also read the crc
                asio::read(serial,asio::buffer(&c,1));
                asio::read(serial,asio::buffer(&c,1));
                reading = false;
                break;
            default:
                ver+=c;
        }
    }
    cout << "version: " << ver << endl;
    return 0;
}
