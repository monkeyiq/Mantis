
#include <string>
#include <iostream>
using namespace std;

#include <sys/time.h>

#include "InputTimeoutHandler.h"

#include <curses.h>

extern WINDOW* w;


InputTimeoutHandler::InputTimeoutHandler()
{
    tv.tv_sec = 0;
    tv.tv_usec = 0;
}

// adapted from code at
// http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
int
InputTimeoutHandler::timeval_subtract (struct timeval* result, struct timeval* x, struct timeval* y)
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

void
InputTimeoutHandler::update()
{
    gettimeofday( &tv, 0 );
}
    
uint32_t
InputTimeoutHandler::diff()
{
    struct timeval z;
    struct timeval diff;
    gettimeofday( &z, 0 );
    if( tv.tv_sec )
    {
        timeval_subtract( &diff, &z, &tv );
        mvwprintw( w,5,0,"sec: %02d", diff.tv_sec );
        return diff.tv_sec;
    }
    return 0;
}
    
