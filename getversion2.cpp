/******************************************************************************
*******************************************************************************
*******************************************************************************

    Copyright (C) 2015 Ben Martin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    For more details see the COPYING file in the root directory of this
    distribution.

*******************************************************************************
*******************************************************************************
******************************************************************************/

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
