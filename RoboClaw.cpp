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


RoboClaw::RoboClaw( boost::asio::io_service& io,
                    const std::string& ttyDevice )
    : io( io )
    , serial( io, ttyDevice )
{
}


string
RoboClaw::getVersion()
{
    uint8_t commands[] = { roboclawAddress, 21 };
    write( serial, boost::asio::buffer(commands, 2));

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

    return ver;
}


float
RoboClaw::getBatteryVoltageMain()
{
    float ret = issueCommandU16( 24 );
    return ret / 10.0;
}

std::pair< float, float >
RoboClaw::getMotorCurrents()
{
    uint8_t commands[] = { roboclawAddress, 49 };
    writeTrackCRC( boost::asio::buffer(commands, 2));
    float a = read2();
    float b = read2();
    a /= 100.0;
    b /= 100.0;
    crcOK();
    return make_pair( a, b );
}

float
RoboClaw::getTemperature()
{
    float ret = issueCommandU16( 82 );
    return ret / 10.0;
}

#define SetU16(arg) (uint8_t)(arg>>8),(uint8_t)arg

void
RoboClaw::setMotorDuty( float a, float b, float deadzone )
{
    if( a < -100 )  a = -100;
    if( b < -100 )  b = -100;
    if( a > 100 )   a =  100;
    if( b > 100 )   b =  100;
    if( a > (-1*deadzone) && a < deadzone ) a = 0;
    if( b > (-1*deadzone) && b < deadzone ) b = 0;
    
    int16_t d1 = a > 0 ? a * 327.67 : a * 327.68;
    int16_t d2 = b > 0 ? b * 327.67 : b * 327.68;
    uint8_t commands[] = { roboclawAddress, 34, SetU16(d1), SetU16(d2), SetU16(0) };
    writeWithCRC( boost::asio::buffer(commands, 8));
    char c = 0;
    asio::read(serial,asio::buffer(&c,1));
}

///////////

/**
 * This is based on the crc function from
 * the Arduino Library and Examples released by ION Motion Control.
 * The RoboClaw.cpp file from Arduino Library and Examples did not include
 * any specific license information.
 * http://ionmc.com/RoboClaw-2x45A-Motor-Controller_p_26.html
 */
uint16_t
RoboClaw::crc_update( uint16_t crc, uint8_t data)
{
	int i;
	crc = crc ^ ((uint16_t)data << 8);
	for (i=0; i<8; i++)
	{
		if (crc & 0x8000)
			crc = (crc << 1) ^ 0x1021;
		else
			crc <<= 1;
	}
    return crc;
}

uint16_t
RoboClaw::calcCRC( boost::asio::mutable_buffers_1 buf, int sz )
{
    uint16_t crc = 0;
    const unsigned char* p = boost::asio::buffer_cast<const unsigned char*>(buf);
    for( int i=0; i<sz; i++ )
        crc = crc_update( crc, p[i] );
    return crc;
}


void
RoboClaw::writeWithCRC( boost::asio::mutable_buffers_1 buf )
{
    int sz = boost::asio::buffer_size( buf );
    m_crc = calcCRC( buf, sz - 2 );
    unsigned char* p = boost::asio::buffer_cast<unsigned char*>(buf);
    p[ sz - 2 ] = m_crc >> 8;
    p[ sz - 1 ] = m_crc;
    write( serial, buf );
}

void
RoboClaw::writeTrackCRC( boost::asio::mutable_buffers_1 buf )
{
    m_crc = calcCRC( buf, boost::asio::buffer_size( buf ) );
    write( serial, buf );
}



uint16_t
RoboClaw::read2( bool updatemcrc )
{
    uint16_t c = 0;
    uint16_t ret = 0;
    asio::read(serial,asio::buffer(&c,1));
    if( updatemcrc ) 
        m_crc = crc_update( m_crc, c );
    ret = c << 8;
    asio::read(serial,asio::buffer(&c,1));
    if( updatemcrc ) 
        m_crc = crc_update( m_crc, c );
    ret |= c;
    return ret;
}

void
RoboClaw::crcOK()
{
    uint16_t c = read2( false );
    if( c != m_crc )
    {
        cerr << endl << "bad result crc found! expected:" << m_crc << " got:" << c << endl;
    }
}


uint16_t
RoboClaw::issueCommandU16( uint8_t cmd )
{
    uint8_t commands[] = { roboclawAddress, cmd };
    writeTrackCRC( boost::asio::buffer(commands, 2));
    uint16_t ret = read2();
    crcOK();
    return ret;
}

void
RoboClaw::rampDown( float d1, float d2 )
{
    // ramp the motors down instead of jerk stopping
    std::pair< float, float > d = make_pair( d1, d2 );
    int numberOfIterations = 10;
    std::pair< float, float > incr =
        make_pair( fabsf( d.first  / numberOfIterations ),
                   fabsf( d.second / numberOfIterations ));
    if( d.first > 0 )   incr.first  *= -1;
    if( d.second > 0 )  incr.second *= -1;
            
    for( int i=0; i < numberOfIterations; i++ )
    {
        d.first  += incr.first;
        d.second += incr.second;
        setMotorDuty( d.first, d.second );
        usleep( (500 * 1000) / numberOfIterations );
    }
    setMotorDuty( 0, 0 );
}

