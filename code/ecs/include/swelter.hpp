#pragma once

#include <warning.hpp>
#include <type_traits>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace Swelter 
{
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
                        (alignof(T) <= 64) && 
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
    

} // Swelter namespace

