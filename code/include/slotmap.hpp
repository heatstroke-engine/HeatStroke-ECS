#pragma once

#include <warning.hpp>
#include <array>
namespace HeatStroke
{   
    /*
     * SlotMap is a data structure that allows for efficient storage and retrieval of elements using a unique key.
     * It provides constant time complexity for insertion, deletion, and access operations.
     */
    template<typename Data_t, std::size_t Capacity>
    class SlotMap
    {
        public:
            using value_type            = Data_t;
            using index_type            = std::uint32_t;
            using gen_type              = index_type;
            using key_type              = struct { index_type id; gen_type gen; };
            using iterator              = value_type*;
            using const_iterator        = value_type const*;

            constexpr explicit SlotMap();

            [[nodiscard]] inline constexpr std::size_t size() const noexcept
            {
                 return size_; 
            }

            [[nodiscard]] inline constexpr std::size_t capacity() const noexcept
            {
                return Capacity; 
            }

            [[nodiscard]] constexpr key_type push_back(value_type&& newVal);

            [[nodiscard]] constexpr key_type push_back(const value_type& newVal)
            {
                return push_back(value_type{newVal});
            }

            constexpr void clear() noexcept
            {
                freelist_init();
                generation_ = 0ULL;
            } 

            [[nodiscard]] inline constexpr Data_t& operator[](const key_type key) noexcept
            {
                runTimeAssertion(is_valid(key));

                return data_[key.id];
            }

            constexpr bool erase(const key_type key) noexcept;

            [[nodiscard]] constexpr iterator begin() noexcept
            {
                return data_.begin();
            }

            [[nodiscard]] constexpr iterator end() noexcept
            {
                return &data_[size_];
            }

            [[nodiscard]] constexpr const_iterator cbegin() const noexcept
            {
                return data_.cbegin();
            }

            [[nodiscard]] constexpr const_iterator cend() const noexcept
            {
                return &data_[size_];
            }

            [[nodiscard]] constexpr bool is_valid(const key_type key) const noexcept
            {
                return key.id < Capacity && indices_[key.id].gen == key.gen;
            }

            //Deleted functions
            SlotMap(const SlotMap& rhs) = delete;
            SlotMap& operator=(const SlotMap& rhs) = delete;

        private:
            [[nodiscard]] constexpr index_type allocate();
            constexpr void free(const key_type) noexcept;

            constexpr void freelist_init() noexcept
            {
                index_type i = 0;
                for(key_type& index : indices_)
                {
                    index.id = ++i;
                }
                freelist_ = 0ULL;
            }

            index_type                       size_      {0ULL};
            index_type                       freelist_  {0ULL};
            gen_type                         generation_{0ULL};
            std::array<  key_type, Capacity> indices_   {};
            std::array<value_type, Capacity> data_      {};
            std::array<index_type, Capacity> erase_     {};

    };


    template<typename Data_t, std::size_t Capacity>
    constexpr SlotMap<Data_t,Capacity>::SlotMap()
    {
        clear();
    }


    template<typename Data_t, std::size_t Capacity>
    HeatStroke::SlotMap<Data_t, Capacity>::index_type
    constexpr SlotMap<Data_t, Capacity>::allocate()
    {

        runTimeAssertion(size_ < Capacity);
        
        // Reserve
        index_type slotId = freelist_;
        freelist_ = indices_[slotId].id; // Freelist -> first free
        // Init slot
        key_type& slot = indices_[slotId];
        slot.id = size_;
        slot.gen = generation_;
        
        //Update
        ++size_;
        ++generation_;
        
        return slotId;
    }


    template<typename Data_t, std::size_t Capacity>
    SlotMap<Data_t, Capacity>::key_type 
    constexpr SlotMap<Data_t, Capacity>::push_back(value_type&& newVal)
    {
        index_type reserveId = allocate();
        key_type& slot = indices_[reserveId];

        // Move data
        data_[slot.id] = std::move(newVal);
        erase_[slot.id] = reserveId;
        // Key for the user
        key_type userKey {slot};
        userKey.id = reserveId;

        return userKey;

         
    }


    template<typename Data_t, std::size_t Capacity>
    constexpr void SlotMap<Data_t,Capacity>::free(const key_type key) noexcept
    {
        runTimeAssertion(is_valid(key)); // This cant happen.

        key_type& slot = indices_[key.id];
        
        if (slot.id != (size_ - 1))
        {
            // Copy last here
            data_[slot.id] = data_[size_ - 1];
            erase_[slot.id] = erase_[size_ - 1];
            indices_[erase_[slot.id]].id = slot.id;
                          
        }
        // Update freelist
        slot.id     = freelist_;
        slot.gen     = generation_++;
        freelist_   = key.id;
        // Update size
        --size_;
    }


    template<typename Data_t, std::size_t Capacity>
    bool
    constexpr SlotMap<Data_t, Capacity>::erase(const key_type key) noexcept
    {
        if (!is_valid(key))
        {
            return false; // Early return, invalid key.
        }
        free(key);
        return true;
    }

}