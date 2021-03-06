#pragma once
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/context_functions.hpp>

namespace Leap
{
class Controller;
}

namespace ossia
{

class OSSIA_EXPORT leapmotion_protocol final : public ossia::net::protocol_base
{
public:
  leapmotion_protocol(ossia::net::network_context_ptr);
  ~leapmotion_protocol();

private:
  void set_device(ossia::net::device_base& dev) override;
  bool pull(net::parameter_base&) override;
  bool push(const net::parameter_base&, const ossia::value& v) override;
  bool push_raw(const net::full_parameter_data&) override;
  bool observe(net::parameter_base&, bool) override;
  bool update(net::node_base& node_base) override;

  ossia::net::network_context_ptr m_context;
  class leap_listener;
  std::unique_ptr<leap_listener> listener;
  std::unique_ptr<Leap::Controller> controller;
};
}
