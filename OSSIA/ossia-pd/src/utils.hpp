#pragma once

#include <ossia-pd/src/client.hpp>
#include <ossia-pd/src/model.hpp>
#include <ossia-pd/src/ossia-pd.hpp>
#include <ossia-pd/src/parameter.hpp>
#include <ossia-pd/src/remote.hpp>
#include <ossia-pd/src/view.hpp>
#include <ossia-pd/src/ossia_obj_base.hpp>
#include <ossia/network/domain/domain.hpp>

#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <fmt/format.h>

namespace ossia
{
namespace pd
{

#pragma mark Value type convertion helper

struct value2atom
{
  std::vector<t_atom>& data;
  void operator()(impulse) const
  {
    t_atom a;
    SETSYMBOL(&a, gensym("bang"));
    data.push_back(a);
  }

  void operator()(int32_t i) const
  {
    t_atom a;
    SETFLOAT(&a, (t_float)i);
    data.push_back(a);
  }

  void operator()(float f) const
  {
    t_atom a;
    SETFLOAT(&a, f);
    data.push_back(a);
  }

  void operator()(bool b) const
  {
    t_atom a;
    t_float f = b ? 1. : 0.;
    SETFLOAT(&a, f);
    data.push_back(a);
  }

  void operator()(const std::string& str) const
  {
    t_symbol* s = gensym(str.c_str());
    t_atom a;
    SETSYMBOL(&a, s);
    data.push_back(a);
  }

  void operator()(char c) const
  {
    t_atom a;
    SETFLOAT(&a, (float)c);
    data.push_back(a);
  }

  template <std::size_t N>
  void operator()(std::array<float, N> vec) const
  {
    data.reserve(data.size() + N);
    for (std::size_t i = 0; i < N; i++)
    {
      t_atom a;
      SETFLOAT(&a, vec[i]);
      data.push_back(a);
    }
  }

  void operator()(const std::vector<ossia::value>& t) const
  {
    data.reserve(data.size() + t.size());
    for (const auto& v : t)
      v.apply(*this);
  }

  void operator()() const
  {
  }
};

template <typename T>
struct value_visitor
{
  T* x;

  void operator()(impulse) const
  {
    // TODO how to deal with impulse ? in Pd bang object doesn't have [set ...(
    // message
    // and sending a bang to the bang object connected to the inlet of the
    // sender will lead to stack overflow...
    if (x->m_dataout)
      outlet_bang(x->m_dataout);

    if (x->m_setout)
      outlet_bang(x->m_setout);
  }
  void operator()(int32_t i) const
  {
    t_atom a;
    SETFLOAT(&a, (t_float)i);
    if (x->m_dataout)
      outlet_float(x->m_dataout, (t_float)i);

    if (x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::instance().sym_set, 1, &a);
  }
  void operator()(float f) const
  {
    t_atom a;
    SETFLOAT(&a, f);
    if (x->m_dataout)
      outlet_float(x->m_dataout, (t_float)f);

    if (x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::instance().sym_set, 1, &a);
  }
  void operator()(bool b) const
  {
    t_atom a;
    t_float f = b ? 1. : 0.;
    SETFLOAT(&a, f);
    if (x->m_dataout)
      outlet_float(x->m_dataout, f);

    if (x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::instance().sym_set, 1, &a);
  }
  void operator()(const std::string& str) const
  {
    t_symbol* s = gensym(str.c_str());
    t_atom a;
    SETSYMBOL(&a, s);
    if (x->m_dataout)
      outlet_symbol(x->m_dataout, s);

    if (x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::instance().sym_set, 1, &a);
  }
  void operator()(char c) const
  {
    t_atom a;
    SETFLOAT(&a, (float)c);
    if (x->m_dataout)
      outlet_float(x->m_dataout, (float)c);

    if (x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::instance().sym_set, 1, &a);
  }

  template <std::size_t N>
  void operator()(std::array<float, N> vec) const
  {
    t_atom a[N];

    for (std::size_t i = 0; i < N; i++)
    {
      SETFLOAT(a + i, vec[i]);
    }

    if (x->m_dataout)
      outlet_list(x->m_dataout, gensym("list"), N, a);

    if (x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::instance().sym_set, N, a);
  }

  void operator()(const std::vector<ossia::value>& t) const
  {
    std::vector<t_atom> va;
    value2atom vm{va};
    for (const auto& v : t)
      v.apply(vm);

    t_atom* list_ptr = !va.empty() ? va.data() : nullptr;
    if (x->m_dataout)
      outlet_list(x->m_dataout, gensym("list"), va.size(), list_ptr);

    if (x->m_setout)
      outlet_anything(x->m_setout, ossia_pd::instance().sym_set, va.size(), list_ptr);
  }

  void operator()() const
  {
    pd_error(x, "%s received invalid data", x->m_name->s_name);
  }
};

#pragma mark Prototype

std::vector<std::string> parse_tags_symbol(t_symbol* tags_symbol);
std::string string_from_path(const std::vector<std::string>& vs, fmt::MemoryWriter& fullpath);

/**
 * @brief register_quarantinized Try to register all quarantinized objects
 */
void register_quarantinized();

/**
 * @fn                static t_class* find_parent(t_eobj* x, t_symbol*
 * classname)
 * @brief             Find the first instance of classname beside or above (in
 * a parent patcher) context.
 * @details           The function iterate all objects at the same level or
 * above x and return the first instance of classname found.
 * @param x           The object around which to search.
 * @param classname   The name of the object object we are looking for.
 * @param start_level Level above current object where to start. 0 for current
 * patcher, 1 start searching in parent canvas.
 * @param level       Return level of the found object
 * @return The instance of the found object.
 */
t_object_base* find_parent(t_eobj* x, std::string classname, int start_level, int* level);

/**
 * @brief replace_brackets Replace '<' ans '>' with '{' and '}'
 * @return the processed string
 */
std::string replace_brackets(std::string);

/**
 * @brief find_parent_alive
 * @details Find a parent that is not being removed soon
 * @param x
 * @param classname
 * @param start_level
 * @return
 */
static t_object_base* find_parent_alive(
    t_eobj* x, std::string classname, int start_level, int* level)
{
  t_object_base* obj = find_parent(x, classname, start_level, level);
  if (obj)
  {
    while (obj && obj->m_dead)
    {
      obj = find_parent_alive(&obj->m_obj, classname, 1, level);
    }
  }
  return obj;
}

#pragma mark template

/**
 * @brief get_absolute_path
 * @param
 *
 *
 * @return std::string with full path to object from root device in an OSC
 * style (with '/')
 */

template<typename T>
std::string get_absolute_path(T* x, typename T::is_model* = nullptr)
{
  fmt::MemoryWriter fullpath;
  std::vector<std::string> vs;

  t_model* model = nullptr;
  int model_level = 0;

  int start_level = 0;
  if (std::is_same<T, t_model>::value)
    start_level = 1;

  model = (t_model*)find_parent_alive(
      &x->m_obj, "ossia.model", start_level, &model_level);
  t_model* tmp = nullptr;

  while (model)
  {
    vs.push_back(model->m_name->s_name);
    tmp = model;
    model = (t_model*)find_parent_alive(
        &tmp->m_obj, "ossia.model", 1, &model_level);
  }

  t_eobj* obj = tmp ? &tmp->m_obj : &x->m_obj;

  int device_level = 0;
  int client_level = 0;

  // FIXme TODO use get root device instead
  auto device = (t_device*)find_parent(obj, "ossia.device", 0, &device_level);
  auto client = (t_client*)find_parent(obj, "ossia.client", 0, &client_level);

  if (client)
    fullpath << client->m_name->s_name << ":";
  if (device)
    fullpath << device->m_name->s_name << ":";
  else
    fullpath << ossia_pd::instance().get_default_device()->get_name() << ":";

  return string_from_path(vs, fullpath);
}

template<typename T>
std::string get_absolute_path(T* x, typename T::is_view* = nullptr)
{
  fmt::MemoryWriter fullpath;
  std::vector<std::string> vs;

  t_view* view = nullptr;
  int view_level = 0;

  int start_level = 0;
  if (std::is_same<T, t_view>::value)
    start_level = 1;

  view =  (t_view*)find_parent_alive(
      &x->m_obj, "ossia.view", start_level, &view_level);
  t_view* tmp = nullptr;

  while (view)
  {
    vs.push_back(view->m_name->s_name);
    tmp = view;
    view
        = (t_view*) find_parent_alive(&tmp->m_obj, "ossia.view", 1, &view_level);
  }

  t_eobj* obj = tmp ? &tmp->m_obj : &x->m_obj;


  int device_level = 0;
  int client_level = 0;

  // FIXme TODO use get root device instead
  auto device = (t_device*)find_parent(obj, "ossia.device", 0, &device_level);
  auto client = (t_client*)find_parent(obj, "ossia.client", 0, &client_level);

  if (client)
    fullpath << client->m_name->s_name << ":";
  if (device)
    fullpath << device->m_name->s_name << ":";
  else
    fullpath << ossia_pd::instance().get_default_device()->get_name() << ":";

  return string_from_path(vs, fullpath);
}

/**
 * @brief find_parent_node : find first active node above
 * @param x : starting object object
 * @return active node pointer if found or nullptr
 */
std::vector<ossia::net::node_base*> find_parent_node(t_object_base* x);

/**
 * @brief Find all objects [classname] in the current patcher starting at
 * specified level.
 * @param list : list in which we are looking for objecfts
 * @param classname : name of the object to search (ossia.model or ossia.view)
 * @return std::vector<t_pd*> containing pointer to t_pd struct of the
 * corresponding classname
 */
std::vector<t_object_base*> find_child_to_register(
    t_object_base* x, t_gobj* start_list, const std::string& classname, bool* found_dev = nullptr);

/**
 * @brief find_peer: iterate through patcher's object list to find a peer
 * @param x
 * @return true if a peer have been found, false otherwise
 */
bool find_peer(t_object_base* x);

/**
 * @brief find_global_node: find nodes matching address with a 'device:' prefix
 * @param addr : address string
 * @return vector of pointers to matching nodes
 */
std::vector<ossia::net::node_base*> find_global_nodes(const std::string& addr);


/**
 * @brief get_address_scope: return address scope (relative, absolute or globale)
 * @param addr: the address to process
 * @return the scope
 */
ossia::pd::address_scope get_address_scope(const std::string& addr);

/**
 * @brief attribute2value : convert t_atom array from attribute to vector of ossia::value
 * @param atom : array of atom
 * @param size : number of value to take
 * @return array of ossia::value
 */
std::vector<ossia::value> attribute2value(t_atom* atom, long size);

/**
 * @brief symbol2val_type Convert a t_symbol into ossia::val_type
 * @param s
 * @return ossia::val_type
 */
ossia::val_type symbol2val_type(t_symbol* s);
t_symbol* val_type2symbol(ossia::val_type t);

#pragma mark Templates

template<typename T>
/**
 * @brief copy : utility function to return a copy of an object
 * @param v : object to copy
 */
auto copy(const T& v) { return v; }

template<typename T>
// self registering (when creating the object)
bool obj_register(T* x)
{
  if (x->m_dead)
    return false; // object will be removed soon

  auto node = find_parent_node(x);

  return x->register_node(node);
}

/*
template <typename T>
extern void obj_bang(T* x);
*/

template <typename T>
bool obj_is_quarantined(T* x)
{
  return x->quarantine().contains(x);
}

template <typename T>
void obj_quarantining(T* x)
{
  if (!obj_is_quarantined(x))
    x->quarantine().push_back(x);
}

template <typename T>
void obj_dequarantining(T* x)
{
  x->quarantine().remove_all(x);
}

template <typename T>
void obj_dump(T* x)
{
  t_atom a;
  std::string fullpath;
  ossia::net::node_base* node{};
  if (x->m_otype == object_class::remote || x->m_otype == object_class::param)
  {
    t_object_base* remote = (t_object_base*) x;
    if (remote->m_matchers.size() == 1)
      node = remote->m_matchers[0].get_node();
  }

  if (node)
  {
    fullpath = ossia::net::address_string_from_node(*node);
    SETSYMBOL(&a, gensym(fullpath.c_str()));
    outlet_anything(x->m_dumpout, gensym("fullpath"), 1, &a);
  }
  fullpath = get_absolute_path<T>(x);
  if (fullpath.back() != '/')
    fullpath += "/";
  fullpath += x->m_name->s_name;
  SETSYMBOL(&a, gensym(fullpath.c_str()));
  outlet_anything(x->m_dumpout, gensym("pdpath"), 1, &a);

  if (node)
  {
    SETFLOAT(&a, 1.);
  }
  else
  {
    SETFLOAT(&a, 0.);
  }
  outlet_anything(x->m_dumpout, gensym("registered"), 1, &a);

  SETFLOAT(&a, obj_is_quarantined<T>(x));
  outlet_anything(x->m_dumpout, gensym("quarantined"), 1, &a);

  if (node)
  {
    ossia::net::parameter_base* param = node->get_parameter();
    if (param)
    {
      // type
      t_symbol* type = val_type2symbol(param->get_value_type());

      SETSYMBOL(&a, type);
      outlet_anything(x->m_dumpout, gensym("type"), 1, &a);

      // domain
      ossia::domain domain = param->get_domain();
      int i = 0;
      if (domain)
      {
       SETSYMBOL(&a, gensym(domain.to_pretty_string().c_str()));
       i++;
      }
      outlet_anything(x->m_dumpout, gensym("domain"), i, &a);


      // bounding mode
      std::string bounding_mode;
      switch (param->get_bounding())
      {
        case ossia::bounding_mode::FREE:
          bounding_mode = "free";
          break;
        case ossia::bounding_mode::CLIP:
          bounding_mode = "clip";
          break;
        case ossia::bounding_mode::WRAP:
          bounding_mode = "wrap";
          break;
        case ossia::bounding_mode::FOLD:
          bounding_mode = "fold";
          break;
        case ossia::bounding_mode::LOW:
          bounding_mode = "low";
          break;
        case ossia::bounding_mode::HIGH:
          bounding_mode = "high";
          break;
        default:
          bounding_mode = "unknown";
      }
      SETSYMBOL(&a, gensym(bounding_mode.c_str()));
      outlet_anything(x->m_dumpout, gensym("bounding_mode"), 1, &a);

      // access mode
      std::string access_mode;
      switch (param->get_access())
      {
        case ossia::access_mode::BI:
          access_mode = "bi";
          break;
        case ossia::access_mode::GET:
          access_mode = "get";
          break;
        case ossia::access_mode::SET:
          access_mode = "set";
          break;
        default:
          access_mode = "unknown";
      }
      SETSYMBOL(&a, gensym(access_mode.c_str()));
      outlet_anything(x->m_dumpout, gensym("access_mode"), 1, &a);

      // repetition filter
      bool rep = param->get_repetition_filter();
      SETFLOAT(&a, rep);
      outlet_anything(x->m_dumpout, gensym("repetition_filter"), 1, &a);

      // unit
      std::string pretty_unit
          = ossia::get_pretty_unit_text(param->get_unit());
      SETSYMBOL(&a, gensym(pretty_unit.c_str()));
      outlet_anything(x->m_dumpout, gensym("unit"), 1, &a);
    }

    // description
    auto description = ossia::net::get_description(*node);
    if (description)
    {
      SETSYMBOL(&a, gensym((*description).c_str()));
      outlet_anything(x->m_dumpout, gensym("description"), 1, &a);
    }
    else
      outlet_anything(x->m_dumpout, gensym("description"), 0, nullptr);

    // tags
    auto tags = ossia::net::get_tags(*node);
    if (tags)
    {
      std::size_t N = (*tags).size();
      std::vector<t_atom> l(N);
      for (std::size_t i = 0; i < N; i++)
      {
        SETSYMBOL(&l[i], gensym((*tags)[i].c_str()));
      }
      outlet_anything(x->m_dumpout, gensym("tags"), N, l.data());
    }
    else
    {
      outlet_anything(x->m_dumpout, gensym("tags"), 0, nullptr);
    }

    auto priority = ossia::net::get_priority(*node);
    if (priority)
    {
      t_atom a;
      SETFLOAT(&a, *priority);
      outlet_anything(x->m_dumpout, gensym("priority"), 1, &a);
    }
  }
}

} // namespace pd
} // namespace ossia
