// we dont use Heap / new keyword!!!

// void* raw = operator new(sizeof(Order)) --> firstly it sets up the memory space which is need to be allocated!
// then we turn the momory into an actual order Object ... by running a constructor

// new(raw) Order{}; --> most expensive job is to find and allocate the memory
// the free memomry chunks are store in the form of LinkedList!

// Sys CALL(to execute some kind of previdged info/permissions) ---> {your code ---> CPU switching into kernal Mode ---> OS runs --->CPU switches back to the user mode ---> program continues.}

// Virtual Memory: Virtual Address Space (VAS) is a contiguous range of virtual memory locations made available to a process by the operating system, allowing it to execute instructions and store data independently of physical hardware constraints

// Page Fault: A page fault is a hardware exception raised by the Memory Management Unit (MMU) when a process attempts to access a memory page that is not currently loaded into physical RAM.
// - OS tries to find the main Addreess and fills everything with 0000. So that whenever Page fault occurs, the private and confential info arent out, in the search of the right page!

// TLB: A Translation Lookaside Buffer (TLB) is a high-speed hardware cache located within the CPU's Memory Management Unit (MMU) that stores recent translations of virtual memory addresses to physical memory addresses.
// It serves to reduce the latency of memory access by avoiding the need to consult the page table in main memory for every memory operation.

// Thread A --->16Bytes ---> LOCK; : whenever the new Keyword is been called!!! look for the free slots int the RAM
// If RAM has the space it provides, otherwise the pointer directs to the OS to allocate the required space!!!

// "malloc" is the memory operator! (operator new calls the malloc)
// malloc is a standard C library function defined in <stdlib.h> used for dynamic memory allocation on the heap at runtime.  It reserves a contiguous block of memory of a specified size in bytes and returns a void
// pointer to the beginning of that block. If the allocation fails, it returns a NULL pointer

/*
    FlowChart

    1. New Order
            |
            |
            |
    2. "Operator new" ---> calls "Malloc"
            |
            |
            |
    3. This finds the free chunk walk size-clas free list, Hope something fits (we keep through specific LinkedList)
            |
            |
            |
    4. Take a lock Heap is shared across threads Contention if arenas run dry ( this is the done in order to avoid the locks, by giving them the slices or chunck of Heap Memory)
            |
            |
            |
    5. Free Block Available?
        /       \
       /         \
      /           \
    Yes            No
    |               |
    |               |
    |               |
Return pointer      3. Ask OS for memory "mmap" syscall<br> User mode ---> kernal mode ---> back<br>~hundreds of ns+
~30-500 ns          |
                    |
                    |
                    4.First write triggers Page Fault OS maps virtual to physical RAM Zero-fills page ~ tens of us(micro sec).
                    |
                    |
                    |
                    5. TLB miss on first touch ---> CPU walks page tables ~ tens of cycles!(page walk: in first entry, CPU needs to traverse in memory info)
                    |
                    |
                    |
                    6. Pointer ready: worst case is micro sec not nano sec. (performance degrades!!!)


* Fragmentation: Memory fragmentation in C++ occurs when the heap contains scattered free blocks interspersed with allocated memory, preventing the allocation of
large contiguous blocks even if total free memory is sufficient.  This typically results from repeated dynamic allocation and deallocation of objects with varying sizes,
creating "holes" that are too small to satisfy future requests.

* Cache misses: Cache misses in C++ occur when the CPU cannot find required data in its fast, small cache and must fetch it from slower main memory (RAM), significantly degrading performance.
This is often caused by poor spatial locality (e.g., traversing linked lists or using std::map nodes scattered across the heap) or pointer chasing, which prevents the CPU’s hardware prefetcher
from predicting and loading data efficiently.
*/

#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <memory_resource>

namespace exchange::core
{
    template <typename T, std::size_t N>
    class MemoryPool
    {
        static_assert(N > 0, "MemoryPool capacity must be positive.");
        static_assert(sizeof(T) >= sizeof(void *),
                      "Object storage must fit an intrusive free-list pointer.");

        struct alignas((alignof(T) > 64U) ? alignof(T) : 64U) Slot
        {
            std::array<std::byte, sizeof(T)> storage{};
        };

        struct FreeNode
        {
            FreeNode *next{nullptr};
        };

    public:
        MemoryPool() : slots_(std::make_unique<Slot[]>(N))
        {
            initialise_free_list();
        }

        MemoryPool(const MemoryPool &) = delete;
        MemoryPool &operator=(const MemoryPool &) = delete;
        MemoryPool(MemoryPool &&) = delete;
        MemoryPool &operator=(MemoryPool &&) = delete;

        T *allocate() noexcept
        {
            if (free_list_head_ == nullptr) [[unlikely]]
            {
                return nullptr;
            }

            FreeNode *node = free_list_head_;
            free_list_head_ = free_list_head_->next;
            ++allocated_count_;
            return reinterpret_cast<T *>(node);
        }

        void deallocate(T *object) noexcept
        {
            if (object == nullptr) [[unlikely]]
            {
                return;
            }

            std::destroy_at(object);
            auto *node = reinterpret_cast<FreeNode *>(object);
            node->next = free_list_head_;
            free_list_head_ = node;
            --allocated_count_;
        }

    private:
        void initialise_free_list() noexcept
        {
            free_list_head_ = nullptr;
            for (std::size_t index = 0; index < N; ++index)
            {
                auto *node = reinterpret_cast<FreeNode *>(slots_[index].storage.data());
                node->next = free_list_head_;
                free_list_head_ = node;
            }
        }

        std::unique_ptr<Slot[]> slots_;
        FreeNode *free_list_head_{nullptr};
        std::size_t allocated_count_{0};
    };
}