#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>

class RoboClaw 
{
  public:
    RoboClaw( boost::asio::io_service& io, const std::string& ttyDevice = "/dev/roboclaw" );

    string getVersion();
    float  getBatteryVoltageMain();
    std::pair< float, float > getMotorCurrents();
    float  getTemperature();

    // Loosely, command 34.
    // For this command both parameters are values from -100 to +100 with
    // anything between -deadzone to +deadzone being no drive.
    void setMotorDuty( float a, float b, float deadzone = 1 );

    void rampDown( float d1, float d2 );

    
  private:
    boost::asio::io_service& io;
    boost::asio::serial_port serial;
    uint16_t m_crc;

    // get the CRC for the first sz bytes of buf
    uint16_t calcCRC( boost::asio::mutable_buffers_1 buf, int sz );

    // write with crc will calculate the crc and put it in the last
    // two bytes of the command before sending.
    void writeWithCRC( boost::asio::mutable_buffers_1 buf );
    // write and crc will clear and set the m_crc to the running crc value
    // for what was written.
    void writeTrackCRC( boost::asio::mutable_buffers_1 buf );

    // read a 2 byte result.
    uint16_t read2( bool updatemcrc = true );
    
    // read the crc and compare it to m_crc to see if it is ok.
    // if it is bad then throw an exception.
    void crcOK();

    // issue the given cmd to the RoboClaw and read a 2 byte result
    // which is returned.
    uint16_t issueCommandU16( uint8_t cmd );

    enum 
    {
        roboclawAddress = 0x80
    };
    
    uint16_t crc_update( uint16_t crc, uint8_t data);

};
