#include "can_cmd.h"
#define DEBUG_MSG

unsigned long Command::m_total_delay = 0;
int Command::m_total_cmd = 0;
MCP_CAN* Command::m_port = 0;
//----------------------------------------------------------------------
bool Command::execute()
{
  //Serial.println("command::execute()");
  if (  m_reply_timer.isActive() )
  {
    if( m_port != 0)
      return send_cmd();
    else
      Serial.println("CAN BUS not inited");
  }

  return false;
};
//----------------------------------------------------------------------
Command::Command()
{
  
}
//----------------------------------------------------------------------
static bool Command::initCAN(const byte mcp_clock)
{
  if( m_port != 0 )
    return false;
    
  const int SPI_CS_PIN = 10;
  m_port = new MCP_CAN(SPI_CS_PIN);

  while (CAN_OK != m_port->begin(CAN_100KBPS, mcp_clock))
    //    byte mcp_clock = 1;
    //    while (CAN_OK != CAN.begin(CAN_100KBPS))
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println(" Init CAN BUS Shield again");
    delay(100);
  }
  Serial.print("CAN BUS Shield init ok, with clock = ");
  Serial.print( 16 / mcp_clock );
  Serial.println(" MHz");

  return true;
}
//----------------------------------------------------------------------
static MCP_CAN* Command::getPort()
{
  return m_port;
}
//----------------------------------------------------------------------
static unsigned long Command::getDelay()
{
  return ( m_total_delay / m_total_cmd ) / m_total_cmd;
}
//----------------------------------------------------------------------
void Command::initCmd(unsigned int id_cmd, size_t cmd_size, long unsigned int each_ms)
{
  m_header = id_cmd;
  m_data_size = cmd_size;
  setTimer(each_ms);
  m_last_millis = 0;

  m_total_cmd++;
  m_total_delay = m_total_delay + each_ms;
}
//----------------------------------------------------------------------
size_t Command::getDataSize()
{
  return m_data_size;
}
//----------------------------------------------------------------------
uint8_t Command::getHeader()
{
  return m_header;
}
//----------------------------------------------------------------------
void Command::getData(uint8_t *d)
{
  //Serial.println("command::getData()");
  memset(d, 0, m_data_size);
}
//----------------------------------------------------------------------
void Command::setTimer(unsigned long   each_ms)
{
  m_reply_timer.start(each_ms);
}
//----------------------------------------------------------------------
bool Command::send_cmd()
{
  //Serial.println("  command::send_cmd()");
  uint8_t data[m_data_size];
  getData(data);

  long unsigned int resDelay = millis() - m_last_millis;
  if ( m_last_millis != 0 )
    m_reply_timer.setAdjust(resDelay);
  m_last_millis = millis();

  byte sndResult = getPort()->sendMsgBuf(m_header, 0, m_data_size, data);

#ifdef DEBUG_MSG
  if ( sndResult == CAN_OK )
  {
    Serial.print("TX[\t");
    Serial.print(millis());
    Serial.print("\t] Delay = \t");

    Serial.print(resDelay);
    Serial.print("\t");
    Serial.print("-> ");
    Serial.print(m_header, HEX);
    for (size_t i = 0; i < m_data_size; i++)
    {
      Serial.print(" ");
      Serial.print(data[i], HEX);
    }
    Serial.println("");
  }
  else
  {
    Serial.print("CAN BUS Shield - sent fail, error = ");
    Serial.println(sndResult);
  }
#endif
  return sndResult;
}
//----------------------------------------------------------------------
LightCommand::LightCommand( )
{
  Serial.println("light_cmd created");
  initCmd(LIGHT_STATE, LIGHT_SIZE, LIGHT_TIME);
}
//----------------------------------------------------------------------
void LightCommand::getData(uint8_t *d)
{
  //Serial.println("light_cmd::getData()");
  uint8_t data[3] = {0, 0x64, 0};
  memcpy(d, data, getDataSize());
}
//----------------------------------------------------------------------
IgnitionCommand::IgnitionCommand( )
{
  Serial.println("ignition_cmd created");
  initCmd(IGNITION_STATE, IGNITION_SIZE, IGNITION_TIME);
}
//----------------------------------------------------------------------
void IgnitionCommand::getData(uint8_t *d)
{
  //Serial.println("light_cmd::getData()");
  uint8_t data[1] = {0x7};
  memcpy(d, data, getDataSize());
}
//----------------------------------------------------------------------
SpeedCommand::SpeedCommand( )
{
  Serial.println("speed_cmd created");
  initCmd(SPEED_STATE, SPEED_SIZE, SPEED_TIME);
}
//----------------------------------------------------------------------
void SpeedCommand::getData(uint8_t *d)
{
  //Serial.println("speed_cmd::getData()");
  //uint8_t data[getDataSize()] = {0, 0, 0, 0, 0, 0, 0, 0}; //TODO
  uint8_t data[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //TODO
  memcpy(d, data, getDataSize());
}
//----------------------------------------------------------------------
