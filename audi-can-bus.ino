#include "functor.h"
#include "functional"
#include "can_cmd.h"

std::vector<Command*> commands;

void setup()
{
  Serial.begin(115200);

  // 1 = 16 Mhz
  // 2 = 8 Mhz
  Command::initCAN( 1 );
  commands.push_back( new LightCommand() );
  commands.push_back( new IgnitionCommand() );
  //    commands.push_back(new speed_cmd( &CAN) );
}

void loop()
{
//  std::for_each( commands.begin(), commands.end(), std::bind2nd( Functor(), commands.size() ) );
  std::for_each( commands.begin(), commands.end(), Functor() );
}
