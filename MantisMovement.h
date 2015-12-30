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

