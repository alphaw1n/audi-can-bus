#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>
//#include <StandardCplusplus.h>

#define IGNITION_STATE  0x271
#define IGNITION_PIN    5
/*
h271 зажигание, 1 byte, 100ms 
-h10: Fzg. unverschlossen, Schluessel steckt nicht ключ снят
-h11: Fzg. unverschlossen, Schluessel steckt in Pos. 0, Zuendung aus 
-h01: Fzg. unverschlossen, Schluessel steckt in Pos. 1, Zuendung aus 
-h05: Fzg. unverschlossen, Schluessel steckt in Pos. 2, Zuendung aus 
-h07: Fzg. unverschlossen, Schluessel steckt in Pos. 3, зажигание включено
-h0B: Fzg. unverschlossen, Schluessel steckt in Pos. 4, зажигание включено, стартер крутит 
*/
#define LIGHT_STATE     0x635
#define LIGHT_PIN       
/*
h635 Licht, 3 byte, 100ms 
-h 00 00 00: Licht aus, 58d 100% Dimmung 
-h 64 00 00: Licht aus, 58d 0% Dimmung
-h 64 64 00: Licht an, 58d 0% Dimmung 
ID 635 : Licht Dimmung (noch nicht fertig) 
*/
#define SPEED           0x351
#define SPEED_PIN       4
/*
351h 8 45 00 00 00 00 75 7B 10 100 1795 Geschwindigkeit speed

h351, Geschwindigkeit, 8 byte, 100ms 
- byte 0: h00 vorwaerts, h02 rueckwaerts 
- byte 1/2: Geschwindigkeit 
- byte 4/5: Wegstreckenimpuls 
*/

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

class timer_
{
public:
    timer_() : each_ms(0), alarm_after_ms(0) {}
    ~timer_() {}
    void start(unsigned long toStart_ms)
    {
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
            return false;
    }

private:
    unsigned long alarm_after_ms;
    unsigned long each_ms;
};

//struct command
//{
//    public :
//        virtual command(uint8_t header, unsigned long each_ms, MCP_CAN* port) = 0;
//        ~command();
//
//    private:
//        unsigned long   m_each_ms;
//        timer_          m_reply_timer;
//        size_t          m_data_size;
//        uint8_t         m_header;
//        MCP_CAN*        m_port;
//};
//
//class light_cmd: public command
//{
//  public :
//      virtual command(uint8_t header, unsigned long each_ms, MCP_CAN* port){};
//}
//}

void setup()
{
    Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_100KBPS, MCP_16MHz))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");
}

unsigned char buffer_L[3] = {0x64, 0, 0};
unsigned char buffer_i[1] = {0x7};

void loop()
{
    // send data:  id = 0x00, standrad frame, data len = 8, stmp: data buf
/*    buffer[7] = buffer[7]+1;
    if(buffer[7] == 100)
    {
        buffer[7] = 0;
        buffer[6] = buffer[6] + 1;
        
        if(buffer[6] == 100)
        {
            buffer[6] = 0;
            buffer[5] = buffer[6] + 1;
        }
    }
    for(int i = 0; i<8; i++)
      {
        if(buffer[i] < 0x10) Serial.print("0");
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
      };
           Serial.println("");
           */
    byte temp = CAN.sendMsgBuf(LIGHT_STATE, 0, 3, buffer_L);
    if( temp == CAN_OK )
        Serial.println("CAN BUS Shield - LIGHT sent OK");
    else
    {
        Serial.print("CAN BUS Shield - sent fail, error = ");
        Serial.println(temp);
    }

    delay(50);
    temp = CAN.sendMsgBuf(IGNITION_STATE, 0, 1, buffer_i);
    if( temp == CAN_OK )
        Serial.println("CAN BUS Shield - IGNIT sent OK");
    else
    {
        Serial.print("CAN BUS Shield - sent fail, error = ");
        Serial.println(temp);
    }

    delay(50);                       // send data per 100ms
}

// END FILE
