#include <iostream>
#include <random>

#include "AVLTree.h"

int main(int argc, char** argv)
{
    const int Samples = 16;
    int samples[Samples];
    {
        std::random_device device;
        std::mt19937 random(device());
        for(int i=0; i<Samples; ++i){
            samples[i] = i;
        }
        for(int i=1; i<Samples; ++i){
            std::uniform_int_distribution<> dist(0, i-1);
            int j = dist(random);
            int t = samples[i];
            samples[i] = samples[j];
            samples[j] = t;
        }
    }
    tree::AVLTree<int> avlTree;

    for(int i=0; i<Samples; ++i){
        avlTree.insert(samples[i]);
    }
    std::cout << "AVL Tree:" << std::endl;
    avlTree.print();

    for(int i=0; i<Samples; ++i){
        TASSERT(NULL != avlTree.find(samples[i]));
    }
    for(int i=0; i<(Samples/2); ++i){
        avlTree.remove(samples[i]);
        TASSERT(NULL == avlTree.find(samples[i]));
    }
    std::cout << "AVL Tree:" << std::endl;
    avlTree.print();
    return 0;
}
