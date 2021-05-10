#pragma once
#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <asio/local/datagram_protocol.hpp>
#include <asio/placeholders.hpp>

namespace ossia::net
{
class udp_socket
{
  using proto = asio::ip::udp;

public:
  udp_socket(std::string_view ip, uint16_t port, asio::io_context& ctx)
      : m_context {ctx}
      , m_endpoint {asio::ip::make_address(ip), port}
      , m_socket {ctx}
  {
  }

  void open()
  {
    m_socket.open(asio::ip::udp::v4());
    m_socket.bind(m_endpoint);
  }

  void connect()
  {
    m_socket.open(asio::ip::udp::v4());
  }

  template <typename F>
  void close(F f)
  {
    m_context.post([this, f] {
      m_socket.close();
      f();
    });
  }

  template <typename F>
  void receive(F f)
  {
    m_socket.async_receive_from(
        asio::buffer(m_data), m_endpoint,
        [this, f](std::error_code ec, std::size_t sz) {
          if (ec == asio::error::operation_aborted)
            return;

          if (!ec && sz > 0)
            try
            {
              f(m_data, sz);
            }
            catch (...)
            {
            }

          this->receive(f);
        });
  }

  void write(const char* data, std::size_t sz)
  {
    m_socket.send_to(asio::buffer(data, sz), m_endpoint);
  }

  asio::io_context& m_context;
  proto::endpoint m_endpoint;
  proto::socket m_socket;
  alignas(16) char m_data[65535];
};

}
