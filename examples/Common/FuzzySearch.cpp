#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include <iostream>

int main(int argc, char** argv)
{
  ossia::net::generic_device device{"test"};

  auto vco_node = device.create_child("vco");
  vco_node->create_child("frequency")->create_parameter();
  vco_node->create_child("waveform")->create_parameter();
  vco_node->create_child("cyclic_ratio")->create_parameter();

  auto filter_node = device.create_child("filter");
  for(int i = 0; i<4; i++)
  {
    auto band = filter_node->create_child("band.1");
    band->create_child("Q")->create_parameter();
    band->create_child("frequency")->create_parameter();
    band->create_child("Gain")->create_parameter();
  }

  auto adsr_node = device.create_child("adsr");
  adsr_node->create_child("attack")->create_parameter();
  adsr_node->create_child("decay")->create_parameter();
  adsr_node->create_child("sustain")->create_parameter();
  adsr_node->create_child("release")->create_parameter();

  auto matches = ossia::net::fuzzysearch(&device.get_root_node(), {"frequency"});

  for(const auto& m : matches)
  {
    std::cout << std::get<0>(m) << "\t"
              << std::get<1>(m) << std::endl;
  }

  return 0;
}