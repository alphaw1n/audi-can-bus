#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>

#include <StandardCplusplus.h>
#include <vector>
#include <algorithm>

//#define DEBUG_MSG

#define IGNITION_STATE  0x271
#define IGNITION_PIN    5
/*
h271 Ignition state, 1 byte, 100ms 
-h10: Fzg. unverschlossen, Schluessel steckt nicht Without Key
-h11: Fzg. unverschlossen, Schluessel steckt in Pos. 0, Zuendung aus 
-h01: Fzg. unverschlossen, Schluessel steckt in Pos. 1, Zuendung aus 
-h05: Fzg. unverschlossen, Schluessel steckt in Pos. 2, Zuendung aus 
-h07: Fzg. unverschlossen, Schluessel steckt in Pos. 3, Ignition ON, Starter stopped
-h0B: Fzg. unverschlossen, Schluessel steckt in Pos. 4, Ignition ON, Starter rotating
*/
#define LIGHT_STATE     0x635
#define LIGHT_PIN       
/*
h635 Light, 3 byte, 100ms 
-h 00 00 00: Light OFF, 58d 100% Dimmung 
-h 64 00 00: Light OFF, 58d 0% Dimmung
-h 64 64 00: Light ON, 58d 0% Dimmung 
ID 635 : Licht Dimmung (noch nicht fertig) 
*/
#define SPEED           0x351
#define SPEED_PIN       4
/*
351h 8 45 00 00 00 00 75 7B 10 100 1795 Geschwindigkeit speed

h351, Geschwindigkeit, 8 byte, 100ms 
- byte 0: h00 vorwaerts (forward), h02 rueckwaerts (backward)
- byte 1/2: Geschwindigkeit (Speed)
- byte 4/5: Wegstreckenimpuls (distance pulse)
*/

const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

class timer_
{
public:
    timer_() : each_ms(0), alarm_after_ms(0) {}
    ~timer_() {}
    void start(unsigned long toStart_ms)
    {
//        Serial.println("timer_::start()");
        each_ms = toStart_ms;
        alarm_after_ms = each_ms + millis();
    }

    bool isActive()
    {
        if ( alarm_after_ms <= millis() )
        {
            alarm_after_ms = each_ms + millis();
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    unsigned long alarm_after_ms;
    unsigned long each_ms;
};

class command
{
    public :
        command() {};
        ~command();
//----------------------------------------------------------------------        
        bool execute() 
        {
//          Serial.println("command::execute()");
          if( m_reply_timer.isActive() )
            return this->send_cmd();
        };
//----------------------------------------------------------------------        
        void initCmd(int id_cmd, size_t cmd_size, unsigned long each_ms, MCP_CAN* port)
        {
          m_header = id_cmd;
          m_port = port;
          m_data_size = cmd_size;
          setTimer(each_ms);
        };
//----------------------------------------------------------------------        
        size_t getDataSize()
        {
          return m_data_size;
        };
//----------------------------------------------------------------------
        uint8_t getHeader()
        {
          return m_header;
        };
//----------------------------------------------------------------------        
        
    private:
        unsigned long   m_each_ms;
        timer_          m_reply_timer;
        size_t          m_data_size;
        int             m_header;
        MCP_CAN*        m_port;
//----------------------------------------------------------------------        
        virtual void    getData(uint8_t *d)
        {
//          Serial.println("command::getData()");
          memset(d, 0, m_data_size);
        };
//----------------------------------------------------------------------        
        void setTimer(unsigned long   each_ms)
        {
          m_each_ms = each_ms;
          m_reply_timer.start(each_ms);
        };
//----------------------------------------------------------------------
        bool send_cmd()
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
//----------------------------------------------------------------------        
};
//----------------------------------------------------------------------        
class light_cmd: public command
{
  public :
//----------------------------------------------------------------------          
      light_cmd(int id_cmd, unsigned long each_ms, MCP_CAN* port) 
      {
        Serial.println("light_cmd created");
        initCmd(id_cmd, 3, each_ms, port);
      }
//----------------------------------------------------------------------              
      ~light_cmd(){};
      
  private :
      bool m_light_state;
//----------------------------------------------------------------------              
      void    getData(uint8_t *d)
      {
//        Serial.println("light_cmd::getData()");
        uint8_t data[getDataSize()] = {0x64, 0, 0};
        memcpy(d, data, getDataSize());
      };
//----------------------------------------------------------------------              
};
//----------------------------------------------------------------------        

class ignition_cmd: public command
{
  public :
//----------------------------------------------------------------------          
      ignition_cmd(int id_cmd, unsigned long each_ms, MCP_CAN* port) 
      {
        Serial.println("ignition_cmd created");
        initCmd(id_cmd, 1, each_ms, port);
      }
//----------------------------------------------------------------------              
      ~ignition_cmd(){};
      
  private :
      bool m_light_state;
//----------------------------------------------------------------------              
      void    getData(uint8_t *d)
      {
//        Serial.println("light_cmd::getData()");
        uint8_t data[getDataSize()] = {0x7};
        memcpy(d, data, getDataSize());
      };
//----------------------------------------------------------------------              
};
//----------------------------------------------------------------------        

class speed_cmd: public command
{
  public :
//----------------------------------------------------------------------          
      speed_cmd(int id_cmd, unsigned long each_ms, MCP_CAN* port) 
      {
        Serial.println("speed_cmd created");
        initCmd(id_cmd, 8, each_ms, port);
      }
//----------------------------------------------------------------------              
      ~speed_cmd(){};
      
  private :
      bool m_light_state;
//----------------------------------------------------------------------              
      void    getData(uint8_t *d)
      {
//        Serial.println("speed_cmd::getData()");
        uint8_t data[getDataSize()] = {0, 0, 0, 0, 0, 0, 0, 0};
        memcpy(d, data, getDataSize());
      };
//----------------------------------------------------------------------              
};
//----------------------------------------------------------------------        


std::vector<command*> commands;

void setup()
{
    Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_100KBPS, MCP_16MHz))
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");

    commands.push_back(new light_cmd(     LIGHT_STATE,    100, &CAN));
    commands.push_back(new ignition_cmd(  IGNITION_STATE, 100, &CAN));
//    commands.push_back(new speed_cmd(     SPEED,          100, &CAN));
}

void executer(command *cmd)
{
//  Serial.println("Tick in executer");
  cmd->execute();
}

void loop()
{
  std::for_each(commands.begin(), commands.end(), executer);
}
