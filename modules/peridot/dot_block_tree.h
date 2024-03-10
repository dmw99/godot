//DotBlockTree.h

#ifndef DOTBLOCKTREE_H
#define DOTBLOCKTREE_H

#include <memory>

#include "core/object/ref_counted.h"
#include "core/math/geometry_3d.h"

#include "dot_block_tree_node.h"


namespace Peridot
{
    class DotBlockTree : public RefCounted
    {
    private:
        GDCLASS(DotBlockTree, RefCounted);

        std::unique_ptr<TreeNode> m_root;

    protected:

        static void _bind_methods();

    public:

        void initialize(const Vector3i& dimensionsInLeafs );

        /// @brief gets the leaf enclosing this position, or null if no leaf there.
        /// @param posInLeafs Absolute position in blocks.
        Variant getLeaf(const Vector3i& posInBlocks);

        /// @brief Gets the DotBlock at the absolute position in blocks. 
        DotBlock get(const Vector3i& posInBlocks);

        /// @brief Sets the absolute position in blocks to the passed-in value. 
        void set(const Vector3i& posInBlocks, DotBlock value);

        /// @brief Gets the number of leafs in the tree, for debugging/unit testing.
        int getNumLeafsDebug();

        /// @brief Gets an internal enum number representing the state of the top root node (0=EMPTY, 1=LEAF, 2=PARENT).
        int getRootStateDebug();
    };
}




#endif //DOTBLOCKTREE_H