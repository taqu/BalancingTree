#ifndef INC_TREE_COMMON_H__
#define INC_TREE_COMMON_H__
/**
@file common.h
@author t-sakai
@date 2017/05/14 create
*/
#include <cassert>
#include <cstring>
#include <utility>
#include <cstdint>
#include <malloc.h>

#ifndef NULL
    #ifdef __cplusplus
        #define NULL 0
    #else
        #define NULL ((void*)0)
    #endif
#endif

#define TASSERT(exp) assert(exp)

#define TNEW new
#define TPLACEMENT_NEW(ptr) new(ptr)
#define TDELETE(ptr) delete (ptr); (ptr)=NULL
#define TDELETE_RAW(ptr) delete (ptr)
#define TDELETE_ARRAY(ptr) delete[] (ptr); (ptr)=NULL

namespace tree
{
    //---------------------------------------------------------
#if defined(_MSC_VER)
    typedef char Char;
    typedef unsigned char UChar;

    typedef __int8 s8;
    typedef __int16 s16;
    typedef __int32 s32;
    typedef __int64 s64;

    typedef unsigned __int8 u8;
    typedef unsigned __int16 u16;
    typedef unsigned __int32 u32;
    typedef unsigned __int64 u64;

    typedef float f32;
    typedef double f64;

    typedef intptr_t  intptr_t;
    typedef uintptr_t  uintptr_t;
    typedef ptrdiff_t  ptrdiff_t;
    typedef size_t size_t;

#elif defined(__GNUC__)
    typedef char Char;
    typedef unsigned char UChar;

    typedef int8_t s8;
    typedef int16_t s16;
    typedef int32_t s32;
    typedef int64_t s64;

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef float f32;
    typedef double f64;

    typedef intptr_t  intptr_t;
    typedef uintptr_t  uintptr_t;
    typedef ptrdiff_t  ptrdiff_t;
    typedef size_t size_t;

#else
    typedef char Char;
    typedef unsigned char UChar;

    typedef char s8;
    typedef short s16;
    typedef long s32;
    typedef long long s64;

    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned long u32;
    typedef unsigned long long u64;

    typedef float f32;
    typedef double f64;

    typedef intptr_t  intptr_t;
    typedef uintptr_t  uintptr_t;
    typedef ptrdiff_t  ptrdiff_t;
    typedef size_t lsize_t;

#endif

    using std::move;

    template<class T>
    void swap(T& x0, T& x1)
    {
        T t(move(x0));
        x0 = move(x1);
        x1 = move(t);
    }

    //---------------------------------------------------------
    struct DefaultAllocator
    {
        static inline void* malloc(u32 size)
        {
            return ::malloc(size);
        }

        static inline void free(void* mem)
        {
            ::free(mem);
        }
    };

#define TALLOCATOR_MALLOC(allocator, size) allocator::malloc(size)

#define TALLOCATOR_FREE(allocator, ptr) allocator::free((ptr));(ptr)=NULL

    template<class T>
    struct DefaultComparator
    {
        /**
        v0<v1 : <0
        v0==v1 : 0
        v0>v1 : >0
        */
        s32 operator()(const T& v0, const T& v1) const
        {
            return (v0==v1)? 0 : ((v0<v1)? -1 : 1);
        }
    };

    template<class T>
    struct DefaultTraversal
    {
        void operator()(T& v0)
        {
        }
    };
}
#endif //INC_TREE_COMMON_H__
