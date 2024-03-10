#include "dot_block_tree.h"

namespace Peridot
{

    Variant DotBlockTree::getLeaf(const Vector3i& posInBlocks)
    {
        if(m_root == nullptr )
        {
            print_error("DotBlockTree is not initialized!");
            return Variant();
        }

        DotBlock* leaf = m_root->getLeaf(posInBlocks);

        if( leaf == nullptr )
        {
            return Variant();
        }
        else
        {
            PackedInt32Array packedArray;
            packedArray.resize(TreeNode::LEAF_LINEAR_SIZE);
            memcpy( packedArray.ptrw(), leaf, sizeof(DotBlock) * TreeNode::LEAF_LINEAR_SIZE );

            return packedArray;
        }
    }

    /// @brief Gets the DotBlock at the absolute position in blocks. 
    DotBlock DotBlockTree::get(const Vector3i& posInBlocks)
    {
        if(m_root == nullptr )
        {
            print_error("DotBlockTree is not initialized!");
            return DOT_BLOCK_NULL;
        }

        if(!m_root->inside(posInBlocks))
        {
            return DOT_BLOCK_NULL;
        }

        return m_root->get(posInBlocks);
    }

    /// @brief Sets the absolute position in blocks to the passed-in value. 
    void DotBlockTree::set(const Vector3i& posInBlocks, DotBlock value)
    {
        if(m_root == nullptr )
        {
            print_error("DotBlockTree is not initialized!");
            return;
        }

        if(m_root->inside(posInBlocks))
        {
            m_root->set(posInBlocks, value);
        }
    }

    void DotBlockTree::initialize(const Vector3i& dimensionsInLeafs )
    {
        m_root.reset(new TreeNode(dimensionsInLeafs));
    }

    int DotBlockTree::getNumLeafsDebug()
    {
        if( m_root == nullptr )
        {
            return -1;
        }

        return m_root->getChildLeafs();
    }

    int DotBlockTree::getRootStateDebug()
    {
        if( m_root == nullptr )
        {
            return -1;
        }

        return m_root->getStateDebug();
    }

    void DotBlockTree::_bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_leaf", "posInBlocks"), &DotBlockTree::getLeaf);
        ClassDB::bind_method(D_METHOD("get", "posInBlocks"), &DotBlockTree::get);
        ClassDB::bind_method(D_METHOD("set", "posInBlocks", "value"), &DotBlockTree::set);
        ClassDB::bind_method(D_METHOD("initialize", "dimensionsInLeafs"), &DotBlockTree::initialize);

        ClassDB::bind_method(D_METHOD("get_root_state_debug"), &DotBlockTree::getRootStateDebug);
        ClassDB::bind_method(D_METHOD("get_num_leafs_debug"), &DotBlockTree::getNumLeafsDebug);
    }
}


