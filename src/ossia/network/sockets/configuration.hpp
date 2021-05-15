#pragma once
#include <cstdint>
#include <string>

namespace ossia::net
{

struct fd_configuration
{
  std::string fd;
};

struct socket_configuration
{
  std::string host;
  uint16_t port;
};

struct double_fd_configuration
{
  fd_configuration first;
  fd_configuration second;
};

struct double_socket_configuration
{
  socket_configuration local;
  socket_configuration remote;
};

struct serial_configuration
{
  // the serial device name ("COM1", "/dev/ttyUSB1"...)
  std::string port;

  int baud_rate {19200};
  int character_size {8};
  enum
  {
    no_flow_control,
    software,
    hardware
  } flow_control {no_flow_control};
  enum
  {
    no_parity,
    odd,
    even
  } parity {no_parity};
  enum
  {
    one,
    onepointfive,
    two
  } stop_bits {one};
};

struct ws_client_configuration
{
  std::string url;
};

struct ws_server_configuration
{
  int port {};
};

// first / second: the unix sockets name.
// Must be reverted between host and mirror as they are symmetrical.
struct unix_dgram_configuration
    : double_fd_configuration
{
};

struct unix_stream_configuration
    : fd_configuration
{
};

struct udp_configuration
    : double_socket_configuration
{
};

struct tcp_configuration
    : socket_configuration
{
};
}