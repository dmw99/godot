//DotBlockTree.h

#ifndef DOTBLOCKTREE_H
#define DOTBLOCKTREE_H

#include "core/object/ref_counted.h"
#include "core/math/geometry_3d.h"

class DotBlockTree : public RefCounted
{
private:
    GDCLASS(DotBlockTree, RefCounted);

    //b0-9  : material type
    //b10-13: block_shape
    //b14-15: Y-rotation. 
    //b16   : is hidden (server).
    //b17   : is_ground (above defaults to sky, below defaults to procedural ground).

    typedef unsigned __int32 DotBlock;

    struct TreeNode
    {
        DotBlock* leafBlocks; 
    };

    




protected:

    static void _bind_methods();

public:

    /// @brief constructor
    /// @param dimensions Size of the area in blocks. All dimensions should be Power-Of-Two.
    /// @param leafDimensions size of a leaf. All dimensions should be Power-Of-Two.
    DotBlockTree(Vector3 dimensions, Vector3 leafDimensions );


    bool isPositive(int value);




};


#endif //DOTBLOCKTREE_H