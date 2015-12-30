#include <linux/joystick.h>

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
#include "MantisMovement.h"
#include "InputTimeoutHandler.h"

#include <curses.h>

WINDOW* w = 0;






int main( int argc, char** argv )
{
    initscr();
    noecho();
    w = newwin( 0, 0, 0, 0 );
    keypad( w, true );
    timeout(1);
    wtimeout(w,1);

    int joyfd = open ("/dev/input/js0", O_RDONLY | O_NONBLOCK);
    
    std::string serialDev = "/dev/roboclaw";
    if( argc > 1 )
    {
        serialDev = argv[1];
    }
    cerr << "serialDevice: " << serialDev << endl;
    boost::asio::io_service io;
    RoboClaw rc( io, serialDev );
    MantisMovement mm;

    InputTimeoutHandler timeoutHandler;
    cout << "version        : " << rc.getVersion() << endl;
    
    struct js_event e;
    bzero( &e, sizeof(struct js_event));
    struct js_event ev_forwardback;
    bzero( &ev_forwardback, sizeof(struct js_event));
    struct js_event ev_leftright;
    bzero( &ev_leftright, sizeof(struct js_event));

    
    for( int iter=0; true; ++iter )
    {
//        mvwprintw( w,10,1,"iter %d  ", iter );
//        wrefresh(w);
        
        uint32_t diff = timeoutHandler.diff();
        if( diff > 1 )
        {
            mvwprintw( w,1,1,"                 " );
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

        struct js_event ne;
        if( ::read (joyfd, &ne,
                  sizeof(struct js_event)) == sizeof(struct js_event) )
        {
            timeoutHandler.update();
            e = ne;

            if( e.type == JS_EVENT_BUTTON )
            {
#define PS3_BUTTON_TRIANGLE 12
                if( e.number == PS3_BUTTON_TRIANGLE ) 
                {
                    std::pair< float, float > d = mm.getActiveDuty();
                    rc.rampDown( d.first, d.second );
                    break;
                }
            }
            
            if( e.type == JS_EVENT_AXIS )
            {
                switch( e.number ) 
                {
                    case 3:
                        ev_forwardback = ne;
                        break;
                    case 0:
                        ev_leftright = ne;
                        break;
                }
            }
            
        }

        // use the stored value for interesting axis readings.
        {
            const float incrSpeed = 1;
            const float incrHeading = 0.01;

            bool adjustAcceleration = true;
            bool directSpeed = true;

            if( adjustAcceleration && directSpeed )
            {
                float v = ev_forwardback.value;
                v /= 32767.0;
                v *= -1;
                mvwprintw( w,1,1,"FWD/BACK   %f   %d", v, iter );
                mm.adjustSpeed( incrSpeed * v );

                v = ev_leftright.value;
                v /= 32767.0;
                v *= 1.0;
                mvwprintw( w,1,1,"LEFT/RIGHT %f ", v );
                mm.setHeading( v );
            }
            else if( adjustAcceleration )
            {
                float v = ev_forwardback.value;
                v /= 32767.0;
                v *= -1;
                mvwprintw( w,1,1,"FWD/BACK   %f   %d", v, iter );
                mm.adjustSpeed( incrSpeed * v );
                
                v = ev_leftright.value;
                v /= 32767.0;
                mvwprintw( w,1,1,"LEFT/RIGHT %f ", v );
                mm.adjustHeading( incrHeading * v );
            }
            else
            {
                float v = ev_forwardback.value;
                v /= 32767.0;
                v *= -1.0;
                v *= 100.0;
                mvwprintw( w,1,1,"FWD/BACK   %f   %d", v, iter );
                mm.setSpeed( v );
                
                v = ev_leftright.value;
                v /= 32767.0;
                v *= 1.0;
                mvwprintw( w,1,1,"LEFT/RIGHT %f ", v );
                mm.setHeading( v );
            }
            
        }
    
            
        std::pair< float, float > d = mm.getActiveDuty();
        mvwprintw( w,0,0,"speed: %+3.2f  heading: %+1.1f  d1:%+3f d2:%+3f",
                   mm.getSpeed(),
                   mm.getHeading(),
                   d.first, d.second );
        wrefresh(w);
        
        rc.setMotorDuty( d.first, d.second );
        
        
//        cout << "temperature    : " << rc.getTemperature() << endl;
        usleep( 20 * 1000 );
    }

    endwin();
    return 0;
}
