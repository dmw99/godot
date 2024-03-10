#include "register_types.h"

#include "core/object/class_db.h"
#include "dot_block_tree.h"

using namespace Peridot;

void initialize_peridot_module(ModuleInitializationLevel p_level )
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }
    ClassDB::register_class<DotBlockTree>();
}

void uninitialize_peridot_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }
   // Nothing to do here in this example.
}
