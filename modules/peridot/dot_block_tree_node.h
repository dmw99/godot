#ifndef DOTBLOCKTREENODE_H
#define DOTBLOCKTREENODE_H

#include "core/math/geometry_3d.h"
#include <vector>

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
    const static int LEAF_DIM_BITS = 4;
    const static int NUM_CHILD_NODES = 2;

private:

    Vector3i originInLeafs;  //origin in leafs, in absolute coordinates.
    Vector3i extentInLeafs;  //exclusive extent in leafs, in absolute coordinates (origin+size)
    Vector3i leafPosition;   //if leaf present, this is the absolute leaf coordinate.

    /// @brief Child nodes. THese will be uninitialized memory if leaf.
    TreeNode* childNodes;

    /// @brief C-style array in linearized XYZ order, with size leafDimensions
    DotBlock* leafBlocks; 

    enum NodeState
    {
        EMPTY,        //nothing in this TreeNode.
        LEAF,         //Inside this TreeNode, there is a single floating Leaf. leaf_offset tells you where it is relative to originInLeafs.
        SHRINK_LEAF,  //This TreeNode is exactly one leaf in size.
        PARENT,       //this TreeNode has 2 children TreeNodes.
    };

    NodeState state;

public:

    TreeNode():
        childNodes(nullptr),
        leafBlocks(nullptr),
        state(NodeState::EMPTY)
    {
    }

    ~TreeNode()
    {
        if( childNodes != nullptr ) { delete[] childNodes; childNodes=nullptr; }
        if( leafBlocks != nullptr ) { delete[] leafBlocks; leafBlocks=nullptr; }
    }

    static bool insideRect(const Vector3i& posInBlocks, const Vector3i& originInLeafs, const Vector3i& extentInLeafs)
    {
        int x = posInBlocks.x >> LEAF_DIM_BITS, 
            y = posInBlocks.y >> LEAF_DIM_BITS,  
            z = posInBlocks.z >> LEAF_DIM_BITS;

        return x >= originInLeafs.x && y >= originInLeafs.y && z >= originInLeafs.z &&
               x <  extentInLeafs.x && y <  extentInLeafs.y && z <  extentInLeafs.z;

    }

    /// @brief  Takes a DotBlock coordinate and tests whether it is inside this TreeNode.
    bool inside(const Vector3i& pos)
    {
        return insideRect(pos, originInLeafs, extentInLeafs);
    }

    void split(const Vector3i& posInBlocks )
    {
        DEV_ASSERT( state == NodeState::LEAF );

        Vector3i posInLeafs( posInBlocks.x >> LEAF_DIM_BITS, posInBlocks.y >> LEAF_DIM_BITS, posInBlocks.z >> LEAF_DIM_BITS);

        int searchOrder[3] {Vector3i::AXIS_Y, Vector3i::AXIS_X, Vector3i::AXIS_Z};

        childNodes = new TreeNode[2];

        for( int i = 0 ; i < 3 ; ++i )
        {
            int pos = posInLeafs[i];
            int origin = leafPosition[i];
            
            if( pos != origin )
            {
                childNodes[0].originInLeafs = originInLeafs;
                childNodes[0].extentInLeafs = extentInLeafs;
                childNodes[0].extentInLeafs[i] = origin;

                childNodes[1].originInLeafs = originInLeafs;
                childNodes[1].extentInLeafs = extentInLeafs;
                childNodes[1].originInLeafs[i] = origin;

            }
        }
        
        


    }

    

    int coord(Vector3i pos) { return pos.y*LEAF_DIM_SIZE*LEAF_DIM_SIZE + pos.z*LEAF_DIM_SIZE + pos.x; }

    /// @brief Gets the BotBlock at this position. Returns DOT_BLOCK_NULL if not found.
    /// @param pos block position relative to origin (lower left).
    /// The caller must call "inside" to confirm that this position is inside the TreeNode first!
    DotBlock get(Vector3i pos)
    {
        DotBlock* pLeaf = getLeaf(pos);
        if( pLeaf == nullptr )
        {
            return DOT_BLOCK_NULL;
        }

        return pLeaf[coord(pos)];
    }

    void set(Vector3i pos, DotBlock value)
    {
        DotBlock* pLeaf = getLeaf(pos);
        if( pLeaf != nullptr )
        {
            pLeaf[coord(pos)] = value;
        }
        else
        {
            
        }

    }

    /// @brief Get the leaf that encloses this position, or null if it doesn't exist yet. 
    /// The caller must call "inside" to confirm that this position is inside the TreeNode first!
    DotBlock* getLeaf(Vector3i pos)
    {
        if(state == NodeState::SHRINK_LEAF )
        {
            return leafBlocks;
        }
        else if(state == NodeState::LEAF)
        {
            Vector3i o = originInLeafs + leafPosition;
            Vector3i ext(o.x+1, o.y+1, o.z+1);

            return insideRect(pos, o, ext) ? leafBlocks : nullptr;
        }
        else
        {
            if( childNodes[0].inside(pos)) { childNodes[0].getLeaf(pos); }
            else { childNodes[1].getLeaf(pos); }
        }
    }



    






};


#endif //DOTBLOCKTREENODE_H