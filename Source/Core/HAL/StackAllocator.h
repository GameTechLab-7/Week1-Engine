﻿#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include <cstdlib>   // malloc, free
#include <cstddef>   // size_t
#include <new>       // placement new

class StackAllocator
{
private:
    struct CMemChunk
    {
        CMemChunk* pNext = nullptr; 

        unsigned char* data() {
            return reinterpret_cast<unsigned char*>(this + 1);
        }
    };

    CMemChunk* m_pChunk;          // 현재(가장 최근에 할당된) 청크
    unsigned char* m_pFirstByte;  // 현재 청크 내에서 아직 할당되지 않은 첫 번째 바이트의 주소
    size_t m_freeBytes;           
    void* m_pFreeSlots;           

    static const size_t cbChunkAlloc = 0x10000;
    static const size_t cbChunkPayload = cbChunkAlloc - sizeof(CMemChunk);

    void* Allocate(size_t sz);
    void allocateBlock();

public:
    StackAllocator();
    ~StackAllocator();

    template<typename E>
    E* newNode() {
        void* ptr = Allocate(sizeof(E));
        if (ptr == nullptr)
            return nullptr;
        return ::new(ptr) E();
    }

    // 템플릿 함수: 특정 노드를 삭제합니다.
    template<typename E>
    void deleteNode(E* node) {
        if (node == nullptr)
            return;
        node->~E();
        *reinterpret_cast<void**>(node) = m_pFreeSlots;
        m_pFreeSlots = node;
    }
};

#endif // STACK_ALLOCATOR_H