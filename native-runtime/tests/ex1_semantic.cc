
#include "ex1.h"
#include "mailbox.h"
// #include "tima_utils.h"

#include <iostream>

void
init_device_data(std::string& device_name,std::map<std::string, std::string>& options, void** ptr)
{
	// empty
}

bool C::b(std::string& name, tima::TimaNativeContext* context)
{
  return false;
}

bool C::c(std::string& name, tima::TimaNativeContext* context)
{
  return false;
}

bool C::d(std::string& name, tima::TimaNativeContext* context)
{
  return false;
}

bool C::True(std::string& name, tima::TimaNativeContext* context)
{
  return true;
}

bool C::False(std::string& name, tima::TimaNativeContext* context)
{
  return false;
}

void D::x(std::string& name, tima::TimaNativeContext* context)
{
  auto ctx = (tima::ActionContext*)context;
  std::cout << "Entering the state because I received a tick" << std::endl;
  ctx->send_to(std::string("pepe"), 1234, std::string("maria"));
}

void D::y(std::string& name, tima::TimaNativeContext* context)
{
  auto ctx = (tima::ActionContext*)context;
  if (ctx->msg_received) {
    std::cout << "Entering the state because I received a MESSAGE : " << ctx->msg.msg_id << " with payload: "  << std::endl << "\t";
    for (auto it = ctx->msg.payload.begin(), itEnd = ctx->msg.payload.end(); it != itEnd ; ++it) {
      std::cout << *it;
    }
    std::cout << std::endl;
  }
}
