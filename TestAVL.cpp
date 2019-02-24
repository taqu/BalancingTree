#include "catch_wrap.hpp"
#include <iostream>
#include <random>

//#define TREE_AVLTREE_ENABLE_DEBUGPRINT
#include "AVLTree.h"

#ifdef TREE_AVLTREE_ENABLE_DEBUGPRINT
#define DEBUGPRINT(tree)\
    std::cout << "AVL Tree:" << std::endl;\
    (tree).print()
#else
#define DEBUGPRINT(tree)
#endif

TEST_CASE("TestAVL")
{
    std::random_device device;
    const int Samples = 512;
    int samples[Samples];
    tree::AVLTree<int> avlTree;
    for(int n = 0; n < 16; ++n) {
        {
            tree::u32 seed = device();
            std::cout << "seed:" << seed << std::endl;
            std::mt19937 random(seed);
            for(int i = 0; i < Samples; ++i) {
                samples[i] = i;
            }
            for(int i = 1; i < Samples; ++i) {
                std::uniform_int_distribution<> dist(0, i - 1);
                int j = dist(random);
                int t = samples[i];
                samples[i] = samples[j];
                samples[j] = t;
            }
        }

        for(int i = 0; i < Samples; ++i) {
            avlTree.insert(tree::move(samples[i]));
        }
        DEBUGPRINT(avlTree);

        for(int i = 0; i < Samples; ++i) {
            tree::s32 pos = avlTree.find(samples[i]);
            EXPECT_NE(avlTree.end(), pos);
            EXPECT_EQ(samples[i], avlTree.get(pos));
        }
        for(int i = 0; i < (Samples / 2); ++i) {
            avlTree.remove(samples[i]);
            tree::s32 pos = avlTree.find(samples[i]);
            EXPECT_EQ(avlTree.end(), pos);
        }
        for(int i = (Samples / 2); i < Samples; ++i) {
            tree::s32 pos = avlTree.find(samples[i]);
            EXPECT_NE(avlTree.end(), pos);
        }
        DEBUGPRINT(avlTree);

        for(int i = 0; i < (Samples / 2); ++i) {
            avlTree.insert(tree::move(samples[i]));
        }

        for(int i = 0; i < Samples; ++i) {
            tree::s32 pos = avlTree.find(samples[i]);
            EXPECT_NE(avlTree.end(), pos);
            EXPECT_EQ(samples[i], avlTree.get(pos));
        }
        DEBUGPRINT(avlTree);
        avlTree.clear();
    }
}
