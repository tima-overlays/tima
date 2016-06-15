

#include "inet/applications/tima/tima.h"
#include "inet/applications/tima/mailbox.h"


using namespace std;
using namespace tima;

namespace neighbors {

int
compute_distance(TimaNativeContext* ctx, int x1, int y1, int x2, int y2)
{
	return (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
}


} /* namespace */
