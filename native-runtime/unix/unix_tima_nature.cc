#include "unix_tima_nature.h"

#include "mailbox.h"

#include <iostream>

#include <time.h>
#include <sys/types.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>	/* needed for sockaddr_in */
#include <sys/time.h>
#include <unistd.h>
#include <cstring>


void
UnixTimaNature::initialize()
{
  // configure event poll
  epollfd = epoll_create1(0);
  if (epollfd == -1) {
    handle_error("epoll_create1");
  }
}

void
UnixTimaNature::send_network_message(const std::string& dst, int port, const std::string& msg)
{
  std::cout << "Sending NETWORK MESSAGE to " << dst << ":" << port << " with data " << msg << std::endl;
}

 void
UnixTimaNature::broadcast(int port, const std::string& msg)
{
}
 
void
UnixTimaNature::print_trace(const std::string& msg)
{
}

void
UnixTimaNature::configure_communication(int port)
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
UnixTimaNature::configure_timer(uint64_t nanoseconds)
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

tima::AbstractTimaNature::EventType
UnixTimaNature::identify_event(int* msg_id, char** payload)
{
  *msg_id = 0;
  *payload = nullptr;
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
		  (*payload) = new char[recvlen + 1];
		  *msg_id = get_msg_id_from_name(buf);
		  std::copy(&buf[j+1], &buf[recvlen + 2], (*payload));
          //tima::Mailbox::add_received_network_message(get_msg_id_from_name(buf), &buf[j+1]);
          return EventType::NETWORK_MESSAGE;
      }
  }
}

void
UnixTimaNature::schedule_events()
{
  // Wait FOR EVENT
  nfds = epoll_wait(epollfd, events, 1, -1);
  if (nfds == -1) {
      perror("epoll_wait");
      exit(EXIT_FAILURE);
  }
}


void UnixTimaNature::handle_error(const char *msg)
{
  do { perror(msg); exit(EXIT_FAILURE); } while (0);
}
