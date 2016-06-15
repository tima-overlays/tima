
#include "inet/applications/tima/tima.h"
#include "inet/applications/tima/mailbox.h"


using namespace std;
using namespace tima;

namespace broadcasting {


void
statistic_message_sent (TimaNativeContext* ctx)
{
	ctx->report_sent_message();
}


void
statistic_message_received (TimaNativeContext* ctx)
{
	ctx->report_received_message();
}

} /* namespace */
