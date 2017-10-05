#include "timer.h"

#include <StandardCplusplus.h>
#include <vector>
#include <algorithm>

#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>

const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

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

class Command
{
    public :
        Command() {};
        virtual ~Command(){};
        bool execute();
        void initCmd(unsigned int id_cmd, size_t cmd_size, long unsigned int each_ms, MCP_CAN* port);

        size_t getDataSize();
        uint8_t getHeader();
 
    private:
        Timer           m_reply_timer;
        size_t          m_data_size;
        unsigned int    m_header;
        MCP_CAN*        m_port;

        virtual void    getData(uint8_t *d);
        void setTimer(unsigned long   each_ms);
        bool send_cmd();
};
//----------------------------------------------------------------------        
class LightCommand: public Command
{
  public :
//----------------------------------------------------------------------          
      LightCommand(int id_cmd, unsigned long each_ms, MCP_CAN* port) 
      {
        Serial.println("light_cmd created");
        initCmd(id_cmd, 3, each_ms, port);
      }
//----------------------------------------------------------------------              
      ~LightCommand(){};
      
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

class IgnitionCommand: public Command
{
  public :
//----------------------------------------------------------------------          
      IgnitionCommand(int id_cmd, unsigned long each_ms, MCP_CAN* port) 
      {
        Serial.println("ignition_cmd created");
        initCmd(id_cmd, 1, each_ms, port);
      }
//----------------------------------------------------------------------              
      ~IgnitionCommand(){};
      
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

class SpeedCommand: public Command
{
  public :
//----------------------------------------------------------------------          
      SpeedCommand(int id_cmd, unsigned long each_ms, MCP_CAN* port) 
      {
        Serial.println("speed_cmd created");
        initCmd(id_cmd, 8, each_ms, port);
      }
//----------------------------------------------------------------------              
      ~SpeedCommand(){};
      
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

