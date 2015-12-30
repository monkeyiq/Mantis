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

#include "MantisMovement.h"


void
MantisMovement::updateMotorDuty()
{
    updateMotorDutyOnlyForwards( 0.6 );
//    updateMotorDutyWithReverseAllowed();
}


void
MantisMovement::updateMotorDutyOnlyForwards( float dampen )
{
    d1 = speed;
    d2 = speed;

    // heading ranges from -1 to 1.
    // for d1 we convert that to 0 to 2 as a multiplication factor
    // for d2 we have headingRangeDelta - the above multi factor 
    float headingRangeOffset = 1;
    float headingRangeDelta  = 2;
    float h = heading * dampen;
    d1 = ( h + headingRangeOffset ) * d1;
    d2 = ( headingRangeDelta - (h + headingRangeOffset)) * d2;
}


void
MantisMovement::updateMotorDutyWithReverseAllowed()
{
    d1 = speed;
    d2 = speed;

    float headingRangeOffset = 1;
    float headingRangeMult   = 2;
    float headingRangeDelta  = headingRangeMult+1;
    // heading ranges from -1 to 1.
    // move h to be starting at 0,   [0..2]
    float h = heading + headingRangeOffset;
    // expand the range of h to      [0..2*headingRangeMult]
    h *= headingRangeMult;

    // assuming headingRangeMult=2
    // for d1 we convert that to -1 to 3 as a multiplication factor
    // move range back to            [-1...3] mid point of 1
    d1 = ( h - (headingRangeMult-1)) * d1;
    // for d2 we have headingRangeDelta - the above multi factor 
    d2 = ( headingRangeDelta - h ) * d2;
}
    
MantisMovement::MantisMovement()
    : speed( 0 )
    , heading( 0 )
    , d1( 0 )
    , d2( 0 )
{
}

void
MantisMovement::adjustSpeed( float v )
{
    speed += v;
    if( speed > 100 )  speed = 100;
    if( speed < -100 ) speed = -100;
    updateMotorDuty();
}

void
MantisMovement::adjustHeading( float v )
{
    heading += v;
    if( heading > 1 )  heading = 1;
    if( heading < -1 ) heading = -1;
    updateMotorDuty();
}

void
MantisMovement::setSpeed( float v )
{
    speed = v;
    if( speed > 100 )  speed = 100;
    if( speed < -100 ) speed = -100;
    updateMotorDuty();
}

void
MantisMovement::setHeading( float v )
{
    heading = v;
    if( heading > 1 )  heading = 1;
    if( heading < -1 ) heading = -1;
    updateMotorDuty();
}

float
MantisMovement::getSpeed()
{
    return speed;
}

float
MantisMovement::getHeading()
{
    return heading;
}

std::pair< float, float >
MantisMovement::getActiveDuty()
{
    return make_pair( d1, d2 );
}
    



