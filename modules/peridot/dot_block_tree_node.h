#ifndef DOTBLOCKTREENODE_H
#define DOTBLOCKTREENODE_H

#include "core/math/geometry_3d.h"
#include <vector>


namespace Peridot
{
    //b0-9  : material type
    //b10-13: block_shape
    //b14-15: Y-rotation. 
    //b16   : is hidden (server).
    //b17   : is_ground (above defaults to sky, below defaults to procedural ground).

    using DotBlock = unsigned int;

    const int DOT_BLOCK_NULL = 0; //material 0 is "null" (not even air).

    /// @brief A node in the DotBlockTree (spatial graph of DotBlocks)
    class TreeNode
    {
    public:

        const static int LEAF_DIM_SIZE = 16;
        const static int LEAF_LINEAR_SIZE = LEAF_DIM_SIZE*LEAF_DIM_SIZE*LEAF_DIM_SIZE;
        const static int LEAF_DIM_BITS = 4;
        const static int NUM_CHILD_NODES = 2;

    private:

        Vector3i m_originInLeafs;  //origin in leafs, in absolute coordinates.
        Vector3i m_extentInLeafs;  //exclusive extent in leafs, in absolute coordinates (origin+size)
        Vector3i m_leafPosition;   //if leaf present, this is the absolute leaf coordinate.

        /// @brief Child nodes. THese will be uninitialized memory if leaf.
        TreeNode* m_childNodes;

        /// @brief C-style array in linearized XYZ order, with size leafDimensions^3
        DotBlock* m_leafBlocks; 

        enum NodeState
        {
            EMPTY,        //nothing in this TreeNode.
            LEAF,         //This TreeNode has one leaf in it. m_leafPosition tells you where it is. For a shrink-wrapped leaf m_originInLefas==m_leafPosition, and m_extentInLeafs==Vector3(1,1,1).
            PARENT,       //this TreeNode has 2 children TreeNodes.
        };

        NodeState m_state;

        void printDebugLine(const char* context, const Vector3i& blockPos);

    public:

        TreeNode();

        TreeNode(const Vector3i& extentInLeafs);

        ~TreeNode();

        static bool insideRect(const Vector3i& posInBlocks, const Vector3i& originInLeafs, const Vector3i& extentInLeafs);

        /// @brief  Takes a DotBlock coordinate and tests whether it is inside this TreeNode.
        bool inside(const Vector3i& pos);

        static int coord(const Vector3i& pos) { return pos.y*LEAF_DIM_SIZE*LEAF_DIM_SIZE + pos.z*LEAF_DIM_SIZE + pos.x; }

        /// @brief Gets the BotBlock at this position. Returns DOT_BLOCK_NULL if not found.
        /// @param pos block position relative to origin (lower left).
        /// The caller must call "inside" to confirm that this position is inside the TreeNode first!
        DotBlock get(const Vector3i& pos);

        /// @brief Sets a dot-block value at the given position. 
        /// @param posInBlocks Position in absolute block coordinates. 
        /// @param value The value to set at this position. 
        void set(const Vector3i& posInBlocks, DotBlock value);

        /// @brief Get the leaf that encloses this position, or null if it doesn't exist yet. 
        /// The caller must call "inside" to confirm that this position is inside the TreeNode first!
        DotBlock* getLeaf(const Vector3i& posInBlocks);

        /// @brief Returns the number of child (and grandchild etc) leafs in this node. This is intended for testing.
        int getChildLeafs();

        /// @brief Gets the internal state of the node. For testing purposes.
        int getStateDebug() { return m_state; }


    private:

        /// @brief Splits this TreeNode. This TreeNode MUST be a leaf, and the calling scope must have already confirmed that 
        /// the submitted position is not inside the leaf. The TreeNode will be split so that one child has the existing leaf, 
        /// and the other child is empty (and will contain the new block, when it is set). 
        /// @param posInBlocks In absolute coordinates.
        void split(const Vector3i& posInBlocks );


        //helper method for set that only works to set a value on a LEAF node, when the block position has already been confirmed
        //to fall inside the leaf.
        void setInLeaf(const Vector3i& posInBlocks, DotBlock value );

    };
}





#endif //DOTBLOCKTREENODE_H