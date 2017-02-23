#include "remote.hpp"
#include "device.hpp"
#include "parameter.hpp"

namespace ossia { namespace pd {

static t_eclass *remote_class;

static void remote_free(t_remote* x);

bool t_remote :: register_node(ossia::net::node_base* node){
    bool res = do_registration(node);
    if(res){
        obj_quarantining<t_remote>(this);
    } else obj_quarantining<t_remote>(this);

    return res;
}

bool t_remote :: do_registration(ossia::net::node_base* node){

    if (x_node && x_node->getParent() == node ) {
        return true; // already register to this node;
    }

    if(node){
        ossia::net::node_base*  nodePtr = node->findChild(x_name->s_name);
        if (nodePtr){
            x_node = nodePtr;
            x_callbackit = x_node->getAddress()->add_callback([=](const ossia::value& v){
                setValue(v);
            });
            x_node->aboutToBeDeleted.connect<t_remote, &t_remote::isDeleted>(this);
            setValue(x_node->getAddress()->cloneValue());

            return true;
        }
    }
    return false;
}

bool t_remote :: unregister(){
    if (x_callbackit != boost::none) {
        if (x_node && x_node->getAddress()) x_node->getAddress()->remove_callback(*x_callbackit);
        x_callbackit = boost::none;
    }
    obj_quarantining<t_remote>(this);

    x_node = nullptr;
    return true;
}

static void remote_float(t_remote *x, t_float val){
    if ( x->x_node && x->x_node->getAddress() ){
        x->x_node->getAddress()->pushValue(float(val));
    } else {
        pd_error(x,"[ossia.remote %s] is not registered to any parameter", x->x_name->s_name);
    }
}

static void *remote_new(t_symbol *name, int argc, t_atom *argv)
{
    t_remote *x = (t_remote *)eobj_new(remote_class);

    if(x)
    {
        x->x_setout = outlet_new((t_object*)x, nullptr);
        x->x_dataout = outlet_new((t_object*)x,nullptr);
        x->x_dumpout = outlet_new((t_object*)x,gensym("dumpout"));
        x->x_callbackit = boost::none;

        if (argc != 0 && argv[0].a_type == A_SYMBOL) {
            x->x_name = atom_getsymbol(argv);
            if (x->x_name != osym_empty && x->x_name->s_name[0] == '/') x->x_absolute = true;

        } else {
            error("You have to pass a name as the first argument");
            x->x_name = gensym("untitledRemote");
        }

        obj_register<t_remote>(x);
    }

    return (x);
}

static void remote_free(t_remote *x)
{
    x->unregister();
    obj_dequarantining<t_remote>(x);
    outlet_free(x->x_setout);
    outlet_free(x->x_dataout);
    outlet_free(x->x_dumpout);
}

extern "C" void setup_ossia0x2eremote(void)
{
    t_eclass *c = eclass_new("ossia.remote", (method)remote_new, (method)remote_free, (short)sizeof(t_remote), CLASS_DEFAULT, A_GIMME, 0);

    if(c)
    {
        eclass_addmethod(c, (method) obj_setFloat<t_param>,    "float",      A_FLOAT, 0);
        eclass_addmethod(c, (method) obj_setSymbol<t_param>,   "symbol",     A_SYMBOL, 0);
        eclass_addmethod(c, (method) obj_setList<t_param>,     "list",       A_GIMME, 0);
        eclass_addmethod(c, (method) obj_bang<t_remote>,       "bang",       A_NULL, 0);
        eclass_addmethod(c, (method) obj_dump<t_remote>,       "dump",       A_NULL, 0);
    }

    remote_class = c;
}

} } // namespace
