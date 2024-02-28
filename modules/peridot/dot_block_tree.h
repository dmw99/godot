//DotBlockTree.h

#ifndef DOTBLOCKTREE_H
#define DOTBLOCKTREE_H

#include "core/object/ref_counted.h"
#include "core/math/geometry_3d.h"
#include "core/math/bvh.h"
#include <vector>


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

    //, 2, true, 128, UserPairTestFunction<GodotCollisionObject3D>, UserCullTestFunction<GodotCollisionObject3D>> bvh;
    BVH_Manager<DotBlock, 

    Vector3 m_leafDimensions

    Vector3 m_dimensions;

    struct TreeNode
    {
        Vector3i origin;

        std::vector<TreeNode> children;


        /// @brief C-style array in linearized XYZ order, with size leafDimensions
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