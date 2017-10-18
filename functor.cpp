#include "functor.h"

void Functor::operator()(Command* cmd) const
{
  if( m_delayTimer.isInited() )
  {
    if( m_delayTimer.isActive() )
      cmd->execute();
  }
  else
  {
    cmd->execute();
    m_delayTimer.start( Command::getDelay() - ( Command::getDelay() * 0.15 ) ); //15% is just from sky
  }
}
