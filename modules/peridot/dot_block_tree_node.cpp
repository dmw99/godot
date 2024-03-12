#include "dot_block_tree_node.h"



#include "core/math/geometry_3d.h"
#include <vector>


namespace Peridot
{
    TreeNode::TreeNode():
        m_childNodes(nullptr),
        m_leafBlocks(nullptr),
        m_state(NodeState::EMPTY)
    {}

    TreeNode::TreeNode(const Vector3i& extentInLeafs):
        m_extentInLeafs(extentInLeafs),
        m_childNodes(nullptr),
        m_leafBlocks(nullptr),
        m_state(NodeState::EMPTY)

    {}

    TreeNode::~TreeNode()
    {
        if( m_childNodes != nullptr ) { delete[] m_childNodes; m_childNodes=nullptr; }
        if( m_leafBlocks != nullptr ) { delete[] m_leafBlocks; m_leafBlocks=nullptr; }
    }

    bool TreeNode::insideRect(const Vector3i& posInBlocks, const Vector3i& originInLeafs, const Vector3i& extentInLeafs)
    {
        int x = posInBlocks.x >> LEAF_DIM_BITS, 
            y = posInBlocks.y >> LEAF_DIM_BITS,  
            z = posInBlocks.z >> LEAF_DIM_BITS;

        return x >= originInLeafs.x && y >= originInLeafs.y && z >= originInLeafs.z &&
                x <  extentInLeafs.x && y <  extentInLeafs.y && z <  extentInLeafs.z;

    }

    bool TreeNode::inside(const Vector3i& pos)
    {
        return insideRect(pos, m_originInLeafs, m_extentInLeafs);
    }

    void TreeNode::split(const Vector3i& posInBlocks )
    {
        DEV_ASSERT( m_state == NodeState::LEAF );

        Vector3i posInLeafs( posInBlocks.x >> LEAF_DIM_BITS, posInBlocks.y >> LEAF_DIM_BITS, posInBlocks.z >> LEAF_DIM_BITS);

        int searchOrder[3] {Vector3i::AXIS_Y, Vector3i::AXIS_X, Vector3i::AXIS_Z};

        m_childNodes = new TreeNode[2];

        for( int i = 0 ; i < 3 ; ++i )
        {
            int axis = searchOrder[i];

            int pos = posInLeafs[axis];
            int origin = m_leafPosition[axis];
            
            if( pos != origin )
            {
                m_childNodes[0].m_originInLeafs = m_originInLeafs;
                m_childNodes[0].m_extentInLeafs = m_extentInLeafs;
                m_childNodes[0].m_extentInLeafs[axis] = origin;

                m_childNodes[1].m_originInLeafs = m_originInLeafs;
                m_childNodes[1].m_extentInLeafs = m_extentInLeafs;
                m_childNodes[1].m_originInLeafs[axis] = origin;

                bool node0HasBlock = m_childNodes[0].inside(posInBlocks);

                //we create one node which is empty, but encloses the position of the new block.
                //To the other node, we give our leaf (making us a PARENT). 
                TreeNode& nodeThatTakesTheLeaf  = node0HasBlock ? m_childNodes[1] : m_childNodes[0];

                //hand off our leaf to our child.
                nodeThatTakesTheLeaf.m_state = NodeState::LEAF;
                nodeThatTakesTheLeaf.m_leafBlocks = m_leafBlocks;
                nodeThatTakesTheLeaf.m_leafPosition = m_leafPosition;
                m_leafBlocks = nullptr;
                m_leafPosition = Vector3i(0,0,0);
                m_state = NodeState::PARENT;

                break;
            }
        }
    }


    DotBlock TreeNode::get(const Vector3i& pos)
    {
        DotBlock* pLeaf = getLeaf(pos);
        if( pLeaf == nullptr )
        {
            return DOT_BLOCK_NULL;
        }

        Vector3i moduloPos(pos.x % LEAF_DIM_SIZE, pos.y % LEAF_DIM_SIZE, pos.z % LEAF_DIM_SIZE);
        return pLeaf[coord(moduloPos)];
    }

    int TreeNode::getChildLeafs()
    {
        if( m_state == NodeState::EMPTY)
        {
            return 0;
        }
        else if( m_state == NodeState::LEAF )
        {
            return 1;
        }
        else if( m_state == NodeState::PARENT )
        {
            int sum = 0;
            for( int i = 0; i < NUM_CHILD_NODES ; i++ )
            {
                sum += m_childNodes[i].getChildLeafs();
            }

            return sum;
        }
        else
        {
            return -1;
        }
    }


    void TreeNode::set(const Vector3i& posInBlocks, DotBlock value)
    {
        if( m_state == NodeState::PARENT )
        {
            TreeNode& target = m_childNodes[0].inside(posInBlocks) ? m_childNodes[0] : m_childNodes[1];
            target.set(posInBlocks, value);
        }
        else if( m_state == NodeState::EMPTY )
        {
            m_leafBlocks = new DotBlock[LEAF_DIM_SIZE*LEAF_DIM_SIZE*LEAF_DIM_SIZE];
            Vector3i posInLeafs( posInBlocks.x >> LEAF_DIM_BITS, posInBlocks.y >> LEAF_DIM_BITS, posInBlocks.z >> LEAF_DIM_BITS);
            m_leafPosition = posInLeafs;
            m_state = NodeState::LEAF;
            setInLeaf(posInBlocks, value);
        }
        else if( m_state == NodeState::LEAF )
        {
            if( insideRect(posInBlocks, m_originInLeafs, m_extentInLeafs))
            {
                setInLeaf(posInBlocks, value );
            }
            else
            {
                //we must split ourselves. Then we will have one child node that contains the leaf, and one empty node.
                split(posInBlocks);
                set(posInBlocks, value); //now we've changed to a PARENT, try again.
            }
        }
    }


    void TreeNode::setInLeaf(const Vector3i& posInBlocks, DotBlock value )
    {
        DEV_ASSERT(m_state == NodeState::LEAF );

        Vector3i relativeBlockPos(posInBlocks.x % LEAF_DIM_SIZE, posInBlocks.y % LEAF_DIM_SIZE, posInBlocks.z % LEAF_DIM_SIZE);
        m_leafBlocks[coord(relativeBlockPos)] = value;
    }


    DotBlock* TreeNode::getLeaf(const Vector3i& posInBlocks)
    {
        if(m_state == NodeState::LEAF)
        {
            Vector3i o = m_originInLeafs + m_leafPosition;
            Vector3i ext(o.x+1, o.y+1, o.z+1);

            return insideRect(posInBlocks, o, ext) ? m_leafBlocks : nullptr;
        }
        else if(m_state == NodeState::EMPTY )
        {
            return nullptr;
        }
        else if(m_state == NodeState::PARENT)
        {
            TreeNode& target = m_childNodes[0].inside(posInBlocks) ? m_childNodes[0] : m_childNodes[1];
            return target.getLeaf(posInBlocks);
        }
        else
        {
            DEV_ASSERT(false); //missing a NodeState??
            return nullptr;
        }
    }

}







