
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

#include <curses.h>

WINDOW* w = 0;

#include "MantisMovement.h"
#include "InputTimeoutHandler.h"





int main( int argc, char** argv )
{

    initscr();
    noecho();
    w = newwin( 0, 0, 0, 0 );
    keypad( w, true );
    timeout(1);
    wtimeout(w,1);
    
    std::string serialDev = "/dev/roboclaw";
    if( argc > 1 )
    {
        serialDev = argv[1];
    }
    cerr << "serialDevice: " << serialDev << endl;

    try 
    {
        boost::asio::io_service io;
        RoboClaw rc( io, serialDev );
        MantisMovement mm;
        InputTimeoutHandler timeoutHandler;
        cout << "version        : " << rc.getVersion() << endl;
    
        while( true )
        {
            uint32_t diff = timeoutHandler.diff();
            if( diff > 1 )
            {
                mvwprintw( w,1,1,"      " );
            }
            if( diff > 5 )
            {
                mvwprintw( w,1,1,"TIMEOUT  " );
                wrefresh(w);

                std::pair< float, float > d = mm.getActiveDuty();
                rc.rampDown( d.first, d.second );
            
                sleep(5);
                break;
            }
        
            int c = wgetch( w );
            if( c > 0 )
            {
                timeoutHandler.update();
            
                const float incrSpeed = 0.5;
                const float incrHeading = 0.05;
                if( c == '0' )
                    break;
                switch( c )
                {
                    case KEY_LEFT:
                        mvwprintw( w,1,1,"LEFT  " );
                        mm.adjustHeading( -1 * incrHeading );
                        break;
                    case KEY_RIGHT:
                        mvwprintw( w,1,1,"RIGHT " );
                        mm.adjustHeading(  1 * incrHeading );
                        break;
                    case KEY_UP:
                        mvwprintw( w,1,1,"UP    " );
                        mm.adjustSpeed(  1 * incrSpeed );
                        break;
                    case KEY_DOWN:
                        mvwprintw( w,1,1,"DOWN  " );
                        mm.adjustSpeed( -1 * incrSpeed );
                        break;
                    default:
                        mvwprintw( w,5,0,". have char: %d", c );
                        break;
                }            
            }
        
            std::pair< float, float > d = mm.getActiveDuty();
            mvwprintw( w,0,0,"speed: %+3.2f  heading: %+1.1f  d1:%+3f d2:%+3f",
                       mm.getSpeed(),
                       mm.getHeading(),
                       d.first, d.second );
        
            rc.setMotorDuty( d.first, d.second );
        
        
//        cout << "temperature    : " << rc.getTemperature() << endl;
            usleep( 20 * 1000 );
        }
    }
    catch( exception& e )
    {
        // reset comms with the RoboClaw and try to stop it!
        RoboClaw::emergencyStop();
    }
    
    endwin();
    return 0;
}
