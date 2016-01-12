#include "automata.h"

#include <stdexcept>

namespace tima {

void*
State::operator new ( std::size_t count )
{
  throw std::runtime_error("You cannot create instantes of state using the new operator");
}

void*
State::operator new[] ( std::size_t count )
{
  throw std::runtime_error("You cannot create instantes of state using the new operator");
}

}
