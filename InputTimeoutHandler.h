#include <time.h>
#include <boost/cstdint.hpp>

class InputTimeoutHandler
{
    struct timeval tv;
    
public:
    InputTimeoutHandler();

    // adapted from code at
    // http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
    int timeval_subtract (struct timeval* result, struct timeval* x, struct timeval* y);

    void update();
    uint32_t diff();
};

