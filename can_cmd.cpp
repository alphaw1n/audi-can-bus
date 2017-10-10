#include "can_cmd.h"
#define DEBUG_MSG

bool Command::execute() 
{
//          Serial.println("command::execute()");
  if( m_reply_timer.isActive() )
    return send_cmd();
  
  return false;
};

void Command::initCmd(unsigned int id_cmd, size_t cmd_size, long unsigned int each_ms, MCP_CAN* port)
{
  m_header = id_cmd;
  m_port = port;
  m_data_size = cmd_size;
  setTimer(each_ms);
};

size_t Command::getDataSize()
{
  return m_data_size;
};

uint8_t Command::getHeader()
{
  return m_header;
};

void Command::getData(uint8_t *d)
{
//          Serial.println("command::getData()");
  memset(d, 0, m_data_size);
};

void Command::setTimer(unsigned long   each_ms)
{
  m_reply_timer.start(each_ms);
};

bool Command::send_cmd()
{
//          Serial.println("  command::send_cmd()");
  uint8_t data[m_data_size];
  getData(data);
  byte sndResult = m_port->sendMsgBuf(m_header, 0, m_data_size, data);
  
  #ifdef DEBUG_MSG
    if( sndResult == CAN_OK )
    {
      Serial.print("TX[\t");
      Serial.print(millis());
      Serial.print("\t] -> ");
      Serial.print(m_header, HEX);
      for(size_t i = 0; i < m_data_size; i++)
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
};
