#ifndef __HEADER_COMMAND
#define __HEADER_COMMAND 1

#include "timer.h"

//#include <StandardCplusplus.h>
#include <ArduinoSTL.h>
#include <vector>
#include <algorithm>

#include <mcp_can_mcp2515.h>
//#include <mcp_can_dfs_mcp2515.h>
//#include <mcp_can_orig.h>

#include <SPI.h>

#define IGNITION_STATE  0x271
#define IGNITION_SIZE   1
#define IGNITION_PIN    5
#define IGNITION_TIME   100
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
#define LIGHT_SIZE      3
#define LIGHT_PIN       3
#define LIGHT_TIME      100
/*
  h635 Light, 3 byte, 100ms
  -h 00 00 00: Light OFF, 58d 100% Dimmung
  -h 64 00 00: Light OFF, 58d 0% Dimmung
  -h 64 64 00: Light ON, 58d 0% Dimmung
  ID 635 : Licht Dimmung (noch nicht fertig)
*/
#define SPEED_STATE     0x351
#define SPEED_SIZE      8
#define SPEED_PIN       4
#define SPEED_TIME      100
/*
  351h 8 45 00 00 00 00 75 7B 10 100 1795 Geschwindigkeit speed

  h351, Geschwindigkeit, 8 byte, 100ms
  - byte 0: h00 vorwaerts (forward), h02 rueckwaerts (backward)
  - byte 1/2: Geschwindigkeit (Speed)
  - byte 4/5: Wegstreckenimpuls (distance pulse)
*/
//----------------------------------------------------------------------
class Command
{
  public :
    Command();
    virtual ~Command() {};
    bool execute();
    void initCmd(unsigned int id_cmd, size_t cmd_size, long unsigned int each_ms);
    
    static unsigned long getDelay();
    static bool initCAN(const byte mcp_clock);

    size_t getDataSize();
    uint8_t getHeader();
  
  protected :
    static int            m_total_cmd;
    static unsigned long  m_total_delay;
    static MCP_CAN*       m_port;

  private:
    Timer             m_reply_timer;
    size_t            m_data_size;
    unsigned int      m_header;
    long unsigned int m_last_millis;
    
    MCP_CAN*          getPort();

    virtual void    getData(uint8_t *d);
    void setTimer(unsigned long   each_ms);
    bool send_cmd();
};
//----------------------------------------------------------------------
class LightCommand: public Command
{
  public :
    LightCommand( );
    ~LightCommand() {};

  private :
    bool m_light_state;
    void    getData(uint8_t *d);
};
//----------------------------------------------------------------------
class IgnitionCommand: public Command
{
  public :
    IgnitionCommand( );
    ~IgnitionCommand() {};

  private :
    bool m_light_state;
    void    getData(uint8_t *d);
};
//----------------------------------------------------------------------
class SpeedCommand: public Command
{
  public :
    SpeedCommand( );
    ~SpeedCommand() {};

  private :
    bool m_light_state;
    void    getData(uint8_t *d);
};
//----------------------------------------------------------------------
#endif
