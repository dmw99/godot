#include "dot_block_tree.h"

void DotBlockTree::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("is_positive", "value"), &DotBlockTree::isPositive);
}

bool DotBlockTree::isPositive(int value)
{
    return value > 0;
}

