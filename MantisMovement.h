
#include <string>
#include <iostream>
using namespace std;

class MantisMovement 
{
    float speed;
    float heading;

    float d1;
    float d2;

    void updateMotorDuty();
    void updateMotorDutyWithReverseAllowed();
    // dampen is between 0 and 1, with 1 being to have no dampening effect,
    // it's a percentage of the heading to maintain.
    void updateMotorDutyOnlyForwards( float dampen = 1.0 );
    
public:
    MantisMovement();
    void adjustSpeed( float v );
    void adjustHeading( float v );
    void setSpeed( float v );
    void setHeading( float v );
    float getSpeed();
    float getHeading();
    std::pair< float, float > getActiveDuty();
};

