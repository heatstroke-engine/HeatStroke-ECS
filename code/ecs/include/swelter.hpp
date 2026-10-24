#pragma once

#include <warning.hpp>
#include <type_traits>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>
#include <cstddef> 
#include <cstring>
#include <algorithm>

namespace Swelter 
{
    constexpr std::size_t bytes_in_kb = 1024;   // Number of bytes in a KB
    constexpr std::size_t number_of_kb = 16;    // Number of desire KB
    constexpr std::size_t CHUNK_SIZE = number_of_kb * bytes_in_kb; // Chunk size = KB * 1024
    
    constexpr std::size_t COLUMN_ALIGN = 64; // Align column
    
    
    struct ComponentInfo
    {
        // Component data
        std::uint32_t size          {0}; 
        std::uint32_t align         {0};
        bool trivially_copyable     {false};
        bool is_empty               {false};
        // Component type-erase functions
        void (*destroy)(void*) = nullptr;   
        void (*move_construct)(void*, void*) = nullptr;
    };

    template<typename T>
    ComponentInfo makeCMPInfo()
    {
        return ComponentInfo{
            .size  =  std::is_empty_v<T> ?  std::uint32_t{0} : static_cast<std::uint32_t>(sizeof(T)),
            .align = alignof(T),
            .trivially_copyable = std::is_trivially_copyable_v<T>,
            .is_empty = std::is_empty_v<T>,
            .destroy = ([](void* ptr) {
                std::destroy_at(static_cast<T*>(ptr));
            }),
            .move_construct = ([](void* to, void* from) {
                std::construct_at(static_cast<T*>(to), std::move(*static_cast<T*>(from)));
            }) 
        };
    }

    template<typename T>
    concept Component = (!std::is_array_v<T> && 
                        std::is_object_v<T> && 
                        (alignof(T) <= COLUMN_ALIGN) && 
                        std::is_nothrow_move_constructible_v<T> && 
                        std::is_nothrow_destructible_v<T>
                    );

    struct CmpRegistry
    {

        template<typename T>
        static std::uint32_t id() requires Component<std::remove_cvref_t<T>>
        {           
            return typeId<std::remove_cvref_t<T>>();
        }

        template<typename... Ts>
        static void registerComponents()
        {            
            (id<Ts>(), ...);
        }

        static ComponentInfo info(std::uint32_t id) 
        {
            debugAssertion(id < cmpinfo.size(), "Out of bounds id");

            return cmpinfo[id];
        }

        template<typename T>
        static ComponentInfo info() 
        {
            return info(id<T>());
        }

        static std::size_t count() noexcept
        {
            return cmpinfo.size();
        }

        private:
            template<Component T>
            static std::uint32_t typeId() noexcept
            {
                static const std::uint32_t id = enlistType(makeCMPInfo<T>());
                
                return id;
            }

            static std::uint32_t enlistType(ComponentInfo cmpinfo0)
            {
                cmpinfo.emplace_back(cmpinfo0);

                return static_cast<std::uint32_t>(cmpinfo.size() - 1UL);
            }

            inline constinit static std::vector<ComponentInfo> cmpinfo; // TODO: Use mutex.
    };

    struct alignas(COLUMN_ALIGN) Chunk
    {
        compileTimeAssertion(CHUNK_SIZE % COLUMN_ALIGN == 0);

        Chunk() = default;

        // Deleted functions
        Chunk(const Chunk&) = delete;
        Chunk(Chunk&&) = delete;
        Chunk& operator=(const Chunk&) = delete;
        Chunk& operator=(Chunk&&) = delete;
        // Chunk data
        std::byte data[CHUNK_SIZE];
    };
    compileTimeAssertion(sizeof(Chunk) == CHUNK_SIZE);


    struct ChunkPool
    {
        static Chunk* allocate()
        {
            Chunk* ret;
            if(frees.empty())
            {
                frees.reserve(owned.size() + 1);
                owned.emplace_back(std::make_unique_for_overwrite<Chunk>());
                ret = owned.back().get();
            }
            else
            {
                ret = frees.back();
                frees.pop_back();
            }
            #ifdef HS_DEBUG_MODE
                std::memset(ret->data,0xCD, CHUNK_SIZE);
            #endif

            return ret;
        }

        static void deallocate(Chunk* chunk) noexcept
        {
            debugAssertion(chunk != nullptr);
            debugAssertion(std::ranges::find(frees, chunk) == frees.end());
            debugAssertion(std::ranges::find(owned, chunk, &std::unique_ptr<Swelter::Chunk>::get) != owned.end());

            #ifdef HS_DEBUG_MODE
                std::memset(chunk->data,0xDD, CHUNK_SIZE);
            #endif

            frees.emplace_back(chunk);
        }

        static std::size_t ownedCount() noexcept
        {
            return owned.size();
        }

        static std::size_t freeCount() noexcept
        {
            return frees.size();
        }

        private:
            // TODO: Use mutex
            inline static constinit std::vector<Chunk*> frees;
            inline static constinit std::vector<std::unique_ptr<Chunk>> owned;
    };

} // Swelter namespace

