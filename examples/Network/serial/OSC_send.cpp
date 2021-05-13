#include <ossia/protocols/osc/osc_factory.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <memory>
#include <functional>

int main(int argc, char** argv)
{
  auto ctx = std::make_shared<ossia::net::network_context>();

  using conf = ossia::net::osc_protocol_configuration;
  ossia::net::generic_device device{ossia::net::make_osc_protocol(ctx,
          {
            conf::MIRROR,
            conf::OSC1_1,
            conf::SLIP,
            ossia::net::serial_configuration{"/dev/ttyS11"}
          }),
        "P"};

  auto on_close = [&] {
    ossia::logger().info("Serial connection closed.");
    exit(0);
  };
  auto on_fail = [&] {
    ossia::logger().error("Serial connection failure.");
    exit(1);
  };
  auto on_connected = [&] {
    ossia::logger().info("Connected, sending : /foo list: [int: 123, float: 4.56]");
    device.get_protocol().push_raw({"/foo", std::vector<ossia::value>{123, 4.56}});
  };

  device.get_protocol().on_connection_open.connect(on_connected);
  device.get_protocol().on_connection_closed.connect(on_close);
  device.get_protocol().on_connection_failure.connect(on_fail);

  auto cb = [&] (ossia::string_view v, const ossia::value& val) {
    ossia::logger().info("{} => {}\n", v, ossia::value_to_pretty_string(val));
    ctx->context.stop();
  };
  device.on_unhandled_message.connect(&cb);

  ctx->run();
}
