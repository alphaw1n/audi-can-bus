//#include <StandardCplusplus.h>
//#include <vector>
//#include <algorithm>

#include "can_cmd.h"

//#define DEBUG_MSG

std::vector<Command*> commands;

void executer(Command *cmd)
{
  cmd->execute();
}

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

    commands.push_back(new LightCommand(     LIGHT_STATE,    100, &CAN));
    commands.push_back(new IgnitionCommand(  IGNITION_STATE, 100, &CAN));
//    commands.push_back(new speed_cmd(     SPEED,          100, &CAN));
}

void loop()
{
  std::for_each(commands.begin(), commands.end(), executer);
}
