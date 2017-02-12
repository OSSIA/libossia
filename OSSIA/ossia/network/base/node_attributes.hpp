#pragma once
#include <ossia/network/base/extended_types.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/detail/any_map.hpp>
#include <ossia_export.h>

#include <boost/any.hpp>
#include <hopscotch_map.h>
#include <vector>
#include <string>

/**
 * \file node_attributes.hpp
 *
 * This file defines default types and accessors for common extended
 * attributes.
 *
 * The attributes are defined as accessors to the
 * extended attributes container.
 *
 * The implementation of the functions is at the end of \ref node.cpp
 */
namespace ossia
{
namespace net
{
class node_base;
using extended_attributes = any_map;

//! How many instances a node can have
struct OSSIA_EXPORT instance_bounds
{
  instance_bounds() = default;
  instance_bounds(const instance_bounds&) = default;
  instance_bounds(instance_bounds&&) = default;
  instance_bounds& operator=(const instance_bounds&) = default;
  instance_bounds& operator=(instance_bounds&&) = default;

  instance_bounds(int32_t min, int32_t max)
      : min_instances{min}
      , max_instances{max} {}

  int32_t min_instances = 0;
  int32_t max_instances = std::numeric_limits<int32_t>::max();
};

//! Tags applied to a node: {"model", "interesting", ...}
using tags = std::vector<std::string>;

//! Human-readable description of a node
using description = std::string;

//! When a node must be sent before other
using priority = int32_t;

//! How often a node is refreshed
using refresh_rate = int32_t;

//! Granularity of the space
using value_step_size = double;

//! The value on which a node should revert by default
using default_value = ossia::value;

//! Means that the node is very important, e.g. a "play" message
using critical = bool;

//! Device attribute : the name of the software managed by this device
using app_name = std::string;

//! Device attribute : the version of the software managed by this device
using app_version = std::string;

//! Device attribute : the creator of the software managed by this device
using app_creator = std::string;


OSSIA_EXPORT ossia::string_view text_instance_bounds();
OSSIA_EXPORT optional<instance_bounds> get_instance_bounds(const extended_attributes& n);
OSSIA_EXPORT void set_instance_bounds(extended_attributes& n, optional<instance_bounds>);

OSSIA_EXPORT ossia::string_view text_tags();
OSSIA_EXPORT optional<tags> get_tags(const extended_attributes& n);
OSSIA_EXPORT void set_tags(extended_attributes& n, optional<tags> v);

OSSIA_EXPORT ossia::string_view text_description();
OSSIA_EXPORT optional<description> get_description(const extended_attributes& n);
OSSIA_EXPORT void set_description(extended_attributes& n, optional<description> v);
OSSIA_EXPORT void set_description(extended_attributes& n, const char* v);

OSSIA_EXPORT ossia::string_view text_priority();
OSSIA_EXPORT optional<priority> get_priority(const extended_attributes& n);
OSSIA_EXPORT void set_priority(extended_attributes& n, optional<priority> v);

OSSIA_EXPORT ossia::string_view text_refresh_rate();
OSSIA_EXPORT optional<refresh_rate> get_refresh_rate(const extended_attributes& n);
OSSIA_EXPORT void set_refresh_rate(extended_attributes& n, optional<refresh_rate> v);

OSSIA_EXPORT ossia::string_view text_value_step_size();
OSSIA_EXPORT optional<value_step_size> get_value_step_size(const extended_attributes& n);
OSSIA_EXPORT void set_value_step_size(extended_attributes& n, optional<value_step_size> v);

OSSIA_EXPORT ossia::string_view text_critical();
OSSIA_EXPORT optional<critical> get_critical(const extended_attributes& n);
OSSIA_EXPORT void set_critical(extended_attributes& n, optional<critical> v);

OSSIA_EXPORT ossia::string_view text_extended_type();
OSSIA_EXPORT optional<extended_type> get_extended_type(const ossia::net::node_base& n);
OSSIA_EXPORT void set_extended_type(extended_attributes& n, optional<extended_type> v);

OSSIA_EXPORT ossia::string_view text_app_name();
OSSIA_EXPORT optional<app_name> get_app_name(const extended_attributes& n);
OSSIA_EXPORT void set_app_name(extended_attributes& n, optional<app_name> v);

OSSIA_EXPORT ossia::string_view text_app_version();
OSSIA_EXPORT optional<app_version> get_app_version(const extended_attributes& n);
OSSIA_EXPORT void set_app_version(extended_attributes& n, optional<app_version> v);

OSSIA_EXPORT ossia::string_view text_app_creator();
OSSIA_EXPORT optional<app_creator> get_app_creator(const extended_attributes& n);
OSSIA_EXPORT void set_app_creator(extended_attributes& n, optional<app_creator> v);

OSSIA_EXPORT ossia::string_view text_default_value();
OSSIA_EXPORT optional<ossia::value> get_default_value(const extended_attributes& n);
OSSIA_EXPORT void set_default_value(extended_attributes& n, const ossia::value& v);
OSSIA_EXPORT void set_default_value(extended_attributes& n, ossia::value&& v);
OSSIA_EXPORT void set_default_value(extended_attributes& n, ossia::none_t v);

// These attributes require an address
OSSIA_EXPORT ossia::string_view text_value();
OSSIA_EXPORT optional<value> clone_value(const ossia::net::node_base& n);
OSSIA_EXPORT void set_value(ossia::net::node_base& n, value v);

OSSIA_EXPORT ossia::string_view text_value_type();
OSSIA_EXPORT optional<val_type> get_value_type(const ossia::net::node_base& n);
OSSIA_EXPORT void set_value_type(ossia::net::node_base& n, val_type v);

OSSIA_EXPORT ossia::string_view text_domain();
OSSIA_EXPORT optional<domain> get_domain(const ossia::net::node_base& n);
OSSIA_EXPORT void set_domain(ossia::net::node_base& n, domain v);

OSSIA_EXPORT ossia::string_view text_access_mode();
OSSIA_EXPORT optional<access_mode> get_access_mode(const ossia::net::node_base& n);
OSSIA_EXPORT void set_access_mode(ossia::net::node_base& n, access_mode v);

OSSIA_EXPORT ossia::string_view text_bounding_mode();
OSSIA_EXPORT optional<bounding_mode> get_bounding_mode(const ossia::net::node_base& n);
OSSIA_EXPORT void set_bounding_mode(ossia::net::node_base& n, bounding_mode v);

OSSIA_EXPORT ossia::string_view text_repetition_filter();
OSSIA_EXPORT optional<repetition_filter> get_repetition_filter(const ossia::net::node_base& n);
OSSIA_EXPORT void set_repetition_filter(ossia::net::node_base& n, repetition_filter v);

OSSIA_EXPORT ossia::string_view text_unit();
OSSIA_EXPORT optional<unit_t> get_unit(const ossia::net::node_base& n);
OSSIA_EXPORT void set_unit(ossia::net::node_base& n, unit_t v);


// Some macros to have minimal reflection facilities...
#define OSSIA_ATTRIBUTE(Type, Name) \
  struct OSSIA_EXPORT Name ## _attribute \
  { \
    using type = Type; \
    static constexpr const auto text = ossia::net::text_ ## Name ; \
    static constexpr const auto getter = ossia::net::get_ ## Name ; \
    static constexpr const auto setter = ossia::net::set_ ## Name ; \
  };


// Attributes of an address
struct OSSIA_EXPORT value_attribute
{
  using type = ossia::value;
  static constexpr const auto text = ossia::net::text_value;
  static constexpr const auto getter = ossia::net::clone_value;
  static constexpr const auto setter = ossia::net::push_value;
};

OSSIA_ATTRIBUTE(ossia::val_type, value_type)
OSSIA_ATTRIBUTE(ossia::net::domain, domain)
OSSIA_ATTRIBUTE(ossia::access_mode, access_mode)
OSSIA_ATTRIBUTE(ossia::bounding_mode, bounding_mode)
OSSIA_ATTRIBUTE(ossia::unit_t, unit)

struct OSSIA_EXPORT default_value_attribute
{
  using type = ossia::value;
  static constexpr const auto text = ossia::net::text_default_value;
  static constexpr const auto getter = ossia::net::get_default_value;
  static constexpr const auto setter = static_cast<void (*)(ossia::net::extended_attributes&, ossia::value&&)>(ossia::net::set_default_value);
};

// Metadata attributes
OSSIA_ATTRIBUTE(ossia::net::tags, tags)
OSSIA_ATTRIBUTE(ossia::net::refresh_rate, refresh_rate)
OSSIA_ATTRIBUTE(ossia::net::priority, priority)
OSSIA_ATTRIBUTE(ossia::net::value_step_size, value_step_size)
OSSIA_ATTRIBUTE(ossia::net::instance_bounds, instance_bounds)
OSSIA_ATTRIBUTE(ossia::net::critical, critical)
OSSIA_ATTRIBUTE(ossia::net::extended_type, extended_type)
OSSIA_ATTRIBUTE(ossia::repetition_filter, repetition_filter)
OSSIA_ATTRIBUTE(ossia::net::app_name, app_name)
OSSIA_ATTRIBUTE(ossia::net::app_creator, app_creator)
OSSIA_ATTRIBUTE(ossia::net::app_version, app_version)

struct OSSIA_EXPORT description_attribute
{
  using type = net::description;
  static constexpr const auto text = ossia::net::text_description;
  static constexpr const auto getter = ossia::net::get_description;
  static constexpr const auto setter = static_cast<void(*)(ossia::net::extended_attributes&,optional<net::description>)>(ossia::net::set_description);
};


}}
