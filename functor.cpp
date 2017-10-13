#include "functor.h"

void Functor::operator()(Command* cmd, int cmdNum) const
{
  cmd->execute();
}
