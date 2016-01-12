

#include <iostream>
#include <sstream>
#include <vector>

#include "automata.h"
#include "executor.h"
#include "mailbox.hpp"
#include "tima_utils.h"

#include <time.h>
#include <sys/types.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>	/* needed for sockaddr_in */
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstring>

#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

template <typename T>
void execute_protocol(T& parameters, int port)
{
  // create executor
  std::vector<tima::Automata*> automatas = parameters.build_stl_version();
  tima::Executor executor(automatas);

  // printing the automatas
  // debug
  std::cout << "Printing the state machine" << std::endl;
  parameters.print_automata(automatas);

  parameters.initialize();

  parameters.configure_communication(port);


  bool  was_tick = true;

  while (true) {
     int msec = 1;
     // SET TIMER
     if (was_tick) {
       msec = executor.global_deadline();
       std::cout << "next deadline is in " << msec << " milliseconds" << std::endl;
       if (msec == tima::never_timeout) {
         msec = 100;
       }
       parameters.configure_timer((uint64_t)(msec) * 1000000);
     }

     parameters.schedule_events();
     // SCHEDULE HANLDERS FOR EVENTS

     // IDENTIFY EVENT
     tima::TimaMethods::EventType event_type = parameters.identify_event();

     // CALL EXECUTOR WHEN YOU RECEIVE A TICK
     was_tick = false;
     if (event_type == tima::TimaMethods::TICK) {
       executor.tick(msec);
       was_tick = true;
     }
 }
}

class NativeTimaMethods : public tima::TimaMethods  {
public:
  void initialize();
  void configure_communication(int port);
  void configure_timer(uint64_t nanoseconds);
  EventType identify_event();
  void schedule_events();
private:
  struct epoll_event events[10];
  int listen_sock, conn_sock, nfds, epollfd, fd;
};

void
NativeTimaMethods::initialize()
{
  // configure event poll
  epollfd = epoll_create1(0);
  if (epollfd == -1) {
    handle_error("epoll_create1");
  }
}

void
NativeTimaMethods::configure_communication(int port)
{
  if ((listen_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    handle_error("Cannot create socket");
  }

  struct sockaddr_in myaddr;
  /* bind to an arbitrary return address */
  /* because this is the client side, we don't care about the address */
  /* since no application will initiate communication here - it will */
  /* just send responses */
  /* INADDR_ANY is the IP address and 0 is the socket */
  /* htonl converts a long integer (e.g. address) to a network representation */
  /* htons converts a short integer (e.g. port) to a network representation */
  memset((char *)&myaddr, 0, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port = htons(port);
  if (bind(listen_sock, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
    handle_error("bind failed");
  }

  struct epoll_event ev;
  // add to the epoll
  ev.events = EPOLLIN;
  ev.data.fd = listen_sock;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
     handle_error("epoll_ctl: listen_sock");
  }
}

void
NativeTimaMethods::configure_timer(uint64_t nanoseconds)
{
  struct itimerspec new_value;
  new_value.it_value.tv_sec = ( nanoseconds / 1000000000 );
  new_value.it_value.tv_nsec = (nanoseconds % 1000000000);
  new_value.it_interval.tv_sec = 0; // just once
  new_value.it_interval.tv_nsec = 0;

  fd = timerfd_create(CLOCK_MONOTONIC, 0);
  if (fd == -1) {
      handle_error("timerfd_create");
  }

  if (timerfd_settime(fd, 0, &new_value, NULL) == -1)
      handle_error("timerfd_settime");

  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = fd;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
     handle_error("epoll_ctl: timer");
  }
}

#define MSG_ID_GENERATOR(c)

tima::TimaMethods::EventType
NativeTimaMethods::identify_event()
{
  for (int i = 0; i < nfds; ++i) {
      if (events[i].data.fd == fd) {
        uint64_t missed;
        read(fd, &missed, sizeof(missed));
        if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
            handle_error("epoll_ctl: timer");
        }
        close(fd);
        return EventType::TICK;
      } else {
          //do_use_fd(events[n].data.fd);
          int buf_size = 1024;
          char buf[1024];
          struct sockaddr_in remaddr; /* remote address */
          socklen_t addrlen = sizeof(remaddr); /* length of addresses */
          size_t recvlen = recvfrom(events[i].data.fd, buf, buf_size, 0, (struct sockaddr *)&remaddr, &addrlen);
          size_t j;
          for (j = 0 ; j < recvlen; ++j) {
            if (buf[j] == ';') {
              buf[j] = 0;
              break;
            }
          }
          buf[recvlen + 1] = 0;
          tima::Mailbox::add_received_network_message(get_msg_id_from_name(buf), &buf[j+1]);
          return EventType::NETWORK_MESSAGE;
      }
  }
}

void
NativeTimaMethods::schedule_events()
{
  // Wait FOR EVENT
  nfds = epoll_wait(epollfd, events, 1, -1);
  if (nfds == -1) {
      perror("epoll_wait");
      exit(EXIT_FAILURE);
  }
}

int
main(int argc, char* const *argv)
{

  int port = 1567;
  char c;
  while ((c = getopt (argc, argv, "p:")) != -1) {
    std::stringstream str;
    switch (c) {
      case 'p':
         str << optarg;
         str >> port;
      break;
    }
  }

  std::cout << "The port is " << port << std::endl;

  NativeTimaMethods methods;
  execute_protocol(methods, port);

  return 0;
}
