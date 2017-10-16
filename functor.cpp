#include "functor.h"

void Functor::operator()(Command* cmd) const
{
  delay( Command::getDelay() );
  cmd->execute();
}
