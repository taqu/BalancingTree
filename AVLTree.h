#ifndef INC_TREE_AVLTREE_H_
#define INC_TREE_AVLTREE_H_
/**
@file AVLTree.h
@author t-sakai
@date 2008/11/13 create
*/
#include "common.h"
//#define TREE_AVLTREE_ENABLE_DEBUGPRINT

namespace tree
{
    enum AVLSub
    {
        AVLSub_Left=0,
        AVLSub_Right=1,
    };

    //---------------------------------------------------------------
    //---
    //--- AVLNode
    //---
    //---------------------------------------------------------------
    template<class T>
    class AVLNode
    {
    public:
        s32& getSub(s32 s){ return (s==AVLSub_Left)? left_ : right_;}

        s32 balance_;
        s32 left_;
        s32 right_;
        T value_;
    };

    struct DefaultAVLAllocator
    {
        DefaultAVLAllocator()
        {}

        template<class T>
        inline T* malloc(u32 size)
        {
            return reinterpret_cast<T*>(DefaultAllocator::malloc(size));
        }

        template<class T>
        inline void free(T* mem)
        {
            return DefaultAllocator::free(mem);
        }
    };

    //---------------------------------------------------------------
    //---
    //--- AVLTree
    //---
    //---------------------------------------------------------------
    /// AVL木
    template<class T, class Allocator=DefaultAVLAllocator, class Comparator=DefaultComparator<T> >
    class AVLTree
    {
    public:
        static const s32 MaxLevels = 32;

        typedef u32 size_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T value_type;
        typedef AVLTree this_type;
        typedef AVLNode<T> node_type;

        typedef Allocator allocator_type;
        typedef Comparator comparator_type;

        typedef s32 iterator_type;

        AVLTree();
        ~AVLTree();

        s32 size() const;

        iterator_type find(const value_type& value) const;
        iterator_type find(const value_type& value)
        {
            return static_cast<const this_type*>(this)->find(value);
        }

        iterator_type end() const;

        const value_type& get(iterator_type pos) const;
        value_type& get(iterator_type pos);

        void insert(value_type&& value);
        void remove(const value_type& value);
        void clear();

        void swap(AVLTree& rhs);

#ifdef TREE_AVLTREE_ENABLE_DEBUGPRINT
        void print();
#endif
    private:
        AVLTree(const AVLTree&) = delete;
        AVLTree& operator=(const AVLTree&) = delete;

        struct Step
        {
            s32 node_;
            s32 which_;
        };

        void updateBalance(s32 node);

        s32 insertInternal(s32 node, value_type&& value);
        s32 balanceInsert(s32 node, Step* path, s32 numLevels);

        s32 findInternal(s32 node, Step* path, s32& level, const value_type& value);

        void balanceRemove(Step* path, s32 numLevels);

        void clearInternal(s32 node);

#ifdef TREE_AVLTREE_ENABLE_DEBUGPRINT
        void printInternal(s32 node, s32 level) const;
#endif

        /// Rotate right
        s32 rotateRight(s32 node);

        /// Rotate left
        s32 rotateLeft(s32 node);

        s32 create(value_type&& value);
        void destroy(s32 node);

        class Array
        {
        public:
            Array()
                :capacity_(0)
                ,items_(NULL)
            {}

            const node_type& operator[](s32 index) const
            {
                TASSERT(0<=index && index<capacity_);
                return items_[index];
            }
            node_type& operator[](s32 index)
            {
                TASSERT(0<=index && index<capacity_);
                return items_[index];
            }

            s32 capacity_;
            node_type* items_;
        };
        s32 size_;
        s32 empty_;
        Array nodes_;

        s32 root_;
        allocator_type allocator_;
        comparator_type comparator_;
    };

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator>
    AVLTree<T,Allocator,Comparator>::AVLTree()
        :size_(0)
        ,empty_(-1)
        ,root_(-1)
    {
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator>
    AVLTree<T,Allocator,Comparator>::~AVLTree()
    {
        clear();
        allocator_.free(nodes_.items_);
        nodes_.items_ = NULL;
        nodes_.capacity_ = 0;
        empty_ = -1;
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator>
    typename AVLTree<T,Allocator,Comparator>::iterator_type
        AVLTree<T,Allocator,Comparator>::find(const value_type& value) const
    {
        s32 node = root_;
        while(0 <= node){
            s32 cmp = comparator_(nodes_[node].value_, value);
            if(cmp == 0){
                return node;
            }else if(cmp<0){
                node = nodes_[node].right_;
            }else{
                node = nodes_[node].left_;
            }
        }
        return node;
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator>
    typename AVLTree<T,Allocator,Comparator>::iterator_type
        AVLTree<T,Allocator,Comparator>::end() const
    {
        return -1;
    }

    template<class T, class Allocator, class Comparator>
    const typename AVLTree<T, Allocator, Comparator>::value_type&
        AVLTree<T, Allocator, Comparator>::get(iterator_type pos) const
    {
        return nodes_[pos].value_;
    }

    template<class T, class Allocator, class Comparator>
    typename AVLTree<T, Allocator, Comparator>::value_type&
        AVLTree<T, Allocator, Comparator>::get(iterator_type pos)
    {
        return nodes_[pos].value_;
    }

    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::updateBalance(s32 node)
    {
        TASSERT(0 <= node);

        s32 left = nodes_[node].left_;
        s32 right = nodes_[node].right_;
        TASSERT(0 <= left);
        TASSERT(0 <= right);

        if(1 == nodes_[node].balance_){
            nodes_[left].balance_ = 0;
            nodes_[right].balance_ = -1;

        }else if( -1 == nodes_[node].balance_){
            nodes_[left].balance_ = 1;
            nodes_[right].balance_ = 0;

        }else{
            nodes_[left].balance_ = 0;
            nodes_[right].balance_ = 0;
        }
        nodes_[node].balance_ = 0;
    }

    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::insert(value_type&& value)
    {
        root_ = insertInternal(root_, tree::move(value));
    }

    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::remove(const value_type& value)
    {
        s32 numLevels = 0;
        Step path[MaxLevels];
        s32 n = findInternal(root_, path, numLevels, value);
        if(n<0){
            return;
        }

        node_type& node = nodes_[n];
        s32 left = node.left_;
        s32 right = node.right_;

        if(right<0){
            if(0 < numLevels) {
                nodes_[path[numLevels-1].node_].getSub(path[numLevels-1].which_) = left;
            } else{
                root_ = left;
            }

        }else{
            if(nodes_[right].left_<0){
                nodes_[right].left_ = node.left_;
                nodes_[right].balance_ = node.balance_;

                if(0 < numLevels) {
                    nodes_[path[numLevels-1].node_].getSub(path[numLevels-1].which_) = right;
                } else{
                    root_ = right;
                }


                TASSERT(numLevels<MaxLevels);
                path[numLevels].node_ = right;
                path[numLevels].which_ = AVLSub_Right;
                ++numLevels;

            }else{
                s32 l = numLevels++;
                for(;;){
                    TASSERT(numLevels<MaxLevels);
                    path[numLevels].which_ = AVLSub_Left;
                    path[numLevels].node_ = right;
                    ++numLevels;
                    left = nodes_[right].left_;
                    if(nodes_[left].left_<0){
                        break;
                    }
                    right = left;
                }
                nodes_[left].left_ = node.left_;
                nodes_[right].left_ = nodes_[left].right_;

                nodes_[left].right_ = node.right_;
                nodes_[left].balance_ = node.balance_;

                if(0 < l) {
                    nodes_[path[l - 1].node_].getSub(path[l - 1].which_) = left;
                } else{
                    root_ = left;
                }
                path[l].which_ = AVLSub_Right;
                path[l].node_ = left;
            }
        }
        destroy(n);
        balanceRemove(path, numLevels);
        --size_;
    }

    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::clear()
    {
        clearInternal(root_);
        root_ = -1;
        size_ = 0;
    }

    template<class T, class Allocator, class Comparator>
    void AVLTree<T, Allocator, Comparator>::swap(AVLTree& rhs)
    {
        tree::swap(size_, rhs.size_);
        tree::swap(empty_, rhs.empty_);
        tree::swap(nodes_.capacity_, rhs.nodes_.capacity_);
        tree::swap(nodes_.items_, rhs.nodes_.items_);
        tree::swap(root_, rhs.root_);
        tree::swap(allocator_, rhs.allocator_);
        tree::swap(comparator_, rhs.comparator_);
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::clearInternal(s32 node)
    {
        if(node<0){
            return;
        }
        s32 left = nodes_[node].left_;
        s32 right = nodes_[node].right_;

        destroy(node);

        clearInternal(left);
        clearInternal(right);
    }

    template<class T, class Allocator, class Comparator>
    s32 AVLTree<T,Allocator,Comparator>::insertInternal(s32 node, value_type&& value)
    {
        if(node<0){
            return create(tree::move(value));
        }
        Step path[MaxLevels];

        s32 level = 0;
        s32 ni = node;
        for(;;){
            node_type& n = nodes_[ni];
            s32 cmp = comparator_(n.value_, value);

            if(0 == cmp){
                return node;

            }else if(0<cmp){
                TASSERT(level<MaxLevels);
                path[level].node_ = ni;
                path[level].which_ = AVLSub_Left;
                ++level;
                if(n.left_<0){
                    nodes_[ni].left_ = create(tree::move(value));
                    break;
                }
                ni = n.left_;

            }else{
                TASSERT(level<MaxLevels);
                path[level].node_ = ni;
                path[level].which_ = AVLSub_Right;
                ++level;
                if(n.right_<0){
                    nodes_[ni].right_ = create(tree::move(value));
                    break;
                }
                ni = n.right_;
            }
        }
        ++size_;
        return balanceInsert(node, path, level);
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator>
    s32 AVLTree<T,Allocator,Comparator>::balanceInsert(s32 node, Step* path, s32 numLevels)
    {
        s32 newNode = -1;
        while(0<numLevels){
            --numLevels;
            s32 ni = path[numLevels].node_;
            node_type& n = nodes_[ni];
            s32 which = path[numLevels].which_;
            if(AVLSub_Left == which){
                ++n.balance_;
            }else{
                --n.balance_;
            }
            s32 balance = n.balance_;
            if(0==balance){
                return node;
            }
            if(1<balance){
                if(nodes_[n.left_].balance_<0){
                    //LR
                    n.left_ = rotateLeft(n.left_);
                    newNode = rotateRight(ni);
                    updateBalance(newNode);
                }else{
                    //LL
                    newNode = rotateRight(ni);
                    nodes_[newNode].balance_ = 0;
                    n.balance_ = 0;
                }
                break;

            }else if(balance<-1){
                if(0<nodes_[n.right_].balance_){
                    //RL
                    n.right_ = rotateRight(n.right_);
                    newNode = rotateLeft(ni);
                    updateBalance(newNode);
                }else{
                    //RR
                    newNode = rotateLeft(ni);
                    nodes_[newNode].balance_ = 0;
                    n.balance_ = 0;
                }
                break;
            }
        }//while(0<numLevels)

        if(0<numLevels){
            node_type& n = nodes_[path[numLevels-1].node_];
            n.getSub(path[numLevels-1].which_) = newNode;

        }else if(0<=newNode){
            return newNode;
        }
        return node;
    }

    template<class T, class Allocator, class Comparator>
    s32 AVLTree<T,Allocator,Comparator>::findInternal(s32 node, Step* path, s32& level, const value_type& value)
    {
        while(0 <= node){
            s32 cmp = comparator_(nodes_[node].value_, value);

            if(0 == cmp){
                return node;

            }else if(0<cmp){
                TASSERT(level<MaxLevels);
                path[level].node_ = node;
                path[level].which_ = AVLSub_Left;
                ++level;
                node = nodes_[node].left_;
            }else{
                TASSERT(level<MaxLevels);
                path[level].node_ = node;
                path[level].which_ = AVLSub_Right;
                ++level;
                node = nodes_[node].right_;
            }
        }
        return node;
    }

    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::balanceRemove(Step* path, s32 numLevels)
    {
        while(0<--numLevels){
            s32 newNode = -1;
            s32 ni = path[numLevels].node_;
            node_type& n = nodes_[ni];
            s32 which = path[numLevels].which_;
            if(AVLSub_Left == which){
                --n.balance_;
            }else{
                ++n.balance_;
            }
            s32 balance = n.balance_;
            if(1<balance){
                if(nodes_[n.left_].balance_<0){
                    //LR
                    n.left_ = rotateLeft(n.left_);
                    newNode = rotateRight(ni);
                    updateBalance(newNode);
                    nodes_[path[numLevels-1].node_].getSub( path[numLevels-1].which_ ) = newNode;
                }else{
                    //LL
                    newNode = rotateRight(ni);
                    nodes_[path[numLevels-1].node_].getSub( path[numLevels-1].which_ ) = newNode;
                    if(0==nodes_[newNode].balance_){
                        nodes_[newNode].balance_ = -1;
                        n.balance_ = 1;
                        break;
                    }else{
                        nodes_[newNode].balance_ = 0;
                        n.balance_ = 0;
                    }
                }

            }else if(balance<-1){
                if(0<nodes_[n.right_].balance_){
                    //RL
                    n.right_ = rotateRight(n.right_);
                    newNode = rotateLeft(ni);
                    updateBalance(newNode);
                    nodes_[path[numLevels-1].node_].getSub( path[numLevels-1].which_ ) = newNode;
                }else{
                    //RR
                    newNode = rotateLeft(ni);
                    nodes_[path[numLevels-1].node_].getSub( path[numLevels-1].which_ ) = newNode;
                    if(0 == nodes_[newNode].balance_){
                        nodes_[newNode].balance_ = 1;
                        n.balance_ = -1;
                        break;
                    }else{
                        nodes_[newNode].balance_ = 0;
                        n.balance_ = 0;
                    }
                }

            }else if(0 != balance){
                break;
            }
        }//while(0<numLevels)
    }

    //---------------------------------------------------------------
    // 右回転
    template<class T, class Allocator, class Comparator>
    s32 AVLTree<T,Allocator,Comparator>::rotateRight(s32 node)
    {
        TASSERT(0<=node);
        s32 left = nodes_[node].left_;
        TASSERT(0<=left); //Left subree should exist

        nodes_[node].left_ = nodes_[left].right_;
        nodes_[left].right_ = node;

        return left;
    }


    //---------------------------------------------------------------
    // 左回転
    template<class T, class Allocator, class Comparator>
    s32 AVLTree<T,Allocator,Comparator>::rotateLeft(s32 node)
    {
        TASSERT(0<=node);
        s32 right = nodes_[node].right_;
        TASSERT(0<=right); //Right subree should exist

        nodes_[node].right_ = nodes_[right].left_;
        nodes_[right].left_ = node;

        return right;
    }


    template<class T, class Allocator, class Comparator>
    s32 AVLTree<T, Allocator, Comparator>::create(value_type&& value)
    {
        if(empty_<0) {
            s32 capacity = nodes_.capacity_ + 16;
            node_type* nodes = allocator_.malloc<node_type>(sizeof(node_type)*capacity);

            //Copy old nodes to new nodes
            for(s32 i=0; i<nodes_.capacity_; ++i){
                TPLACEMENT_NEW(&nodes[i].value_) value_type(tree::move(nodes_[i].value_));
                nodes[i].balance_ = nodes_[i].balance_;
                nodes[i].left_ = nodes_[i].left_;
                nodes[i].right_ = nodes_[i].right_;
            }

            //Initialize new nodes with default constructor
            for(s32 i=nodes_.capacity_; i<capacity; ++i){
                TPLACEMENT_NEW(&nodes[i].value_) value_type();
                nodes[i].balance_ = i+1;
            }
            nodes[capacity-1].balance_ = empty_;
            empty_ = nodes_.capacity_;
            allocator_.free(nodes_.items_);
            nodes_.capacity_ = capacity;
            nodes_.items_ = nodes;
        }
        s32 result = empty_;
        empty_ = nodes_[result].balance_;
        nodes_[result].balance_ = 0;
        nodes_[result].left_ = -1;
        nodes_[result].right_ = -1;
        nodes_[result].value_ = tree::move(value);
        return result;
    }

    template<class T, class Allocator, class Comparator>
    void AVLTree<T, Allocator, Comparator>::destroy(s32 node)
    {
        nodes_[node].value_.~T();
        nodes_[node].balance_ = empty_;
        nodes_[node].left_ = -1;
        nodes_[node].right_ = -1;
        empty_ = node;
    }

#ifdef TREE_AVLTREE_ENABLE_DEBUGPRINT
    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::print()
    {
        printInternal(root_, 0);
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::printInternal(s32 node, s32 level) const
    {
        if(node<0){
            return;
        }
        printInternal(nodes_[node].left_, level+1);
        for(s32 i=0; i<level; ++i){
            std::cout << ' ';
        }
        std::cout << nodes_[node].value_ << std::endl;
        printInternal(nodes_[node].right_, level+1);
    }
#endif
}
#endif //INC_TREE_AVLTREE_H_
