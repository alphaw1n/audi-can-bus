//#include <StandardCplusplus.h>
//#include <vector>
//#include <algorithm>

#include "can_cmd.h"

std::vector<Command*> commands;

const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

void executer(Command *cmd)
{
  cmd->execute();
}

void setup()
{
    Serial.begin(115200);

    byte mcp_clock = MCP_8MHz;
    while (CAN_OK != CAN.begin(CAN_100KBPS, mcp_clock))
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.print("CAN BUS Shield init ok, with clock = ");
    Serial.print( 16 / mcp_clock );
    Serial.println(" MHz");

    commands.push_back(new LightCommand(     LIGHT_STATE,    100, &CAN));
    commands.push_back(new IgnitionCommand(  IGNITION_STATE, 100, &CAN));
//    commands.push_back(new speed_cmd(     SPEED,          100, &CAN));
}

void loop()
{
  std::for_each(commands.begin(), commands.end(), executer);
}
