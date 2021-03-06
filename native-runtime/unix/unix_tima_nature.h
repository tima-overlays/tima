#ifndef __UNIX_TIMA_NATURE__
#define __UNIX_TIMA_NATURE__

#include "tima_nature.h"

#include <string>
#include <sys/epoll.h>
#include <netinet/in.h>	/* needed for sockaddr_in */

class UnixTimaNature : public tima::AbstractTimaNature  {
public:
  UnixTimaNature(std::string device_name): AbstractTimaNature(device_name) {}
  virtual void initialize();
  virtual void configure_communication(int port);
  virtual void configure_timer(uint64_t nanoseconds);
  virtual EventType identify_event(int* msg_id, char** payload);
  virtual void schedule_events();
  virtual void handle_error(const char * msg);
  virtual void send_network_message(const std::string& dst, int port, const std::string& msg);
  virtual void broadcast(int port, const std::string& msg);
  virtual void print_trace(const std::string& msg);
private:
  std::string device_name;
  struct epoll_event events[10];
  int listen_sock, conn_sock, nfds, epollfd, fd;
};

#endif
