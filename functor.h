#ifndef __HEADER_FUNCTOR
#define __HEADER_FUNCTOR 1

#include "can_cmd.h"

struct Functor
{
  typedef Command*  first_argument_type;
  typedef int       second_argument_type;
  typedef void      result_type;

  result_type operator()(first_argument_type cmd) const;
};

#endif
