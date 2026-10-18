#pragma once

#include <vector>
#include <cstdint>
#include <warning.hpp>

namespace HeatStroke
{   

    struct Key 
    { 
        std::uint32_t id; 
        std::uint32_t gen; 
        bool operator==(const Key&) const = default;
    };
    
    /*
     * SlotMap is a data structure that allows for efficient storage and retrieval of elements using a unique key.
     * It provides constant time complexity for insertion, deletion, and access operations.
     */
    template<typename Data_t, std::size_t Capacity = 1000>
    class SlotMap
    {
        public:
            using value_type            = Data_t;
            using index_type            = std::uint32_t;
            using gen_type              = index_type;
            using key_type              = Key;
            using iterator              = value_type*;
            using const_iterator        = value_type const*;

            constexpr explicit SlotMap();

            inline constexpr void resize();

            [[nodiscard]] inline constexpr std::size_t size() const noexcept
            {
                 return size_; 
            }

            [[nodiscard]] inline constexpr std::size_t capacity() const noexcept
            {
                return capacity_; 
            }

            [[nodiscard]] constexpr key_type push_back(value_type&& newVal);

            [[nodiscard]] constexpr key_type push_back(const value_type& newVal)
            {
                return push_back(value_type{newVal});
            }

            constexpr void clear() noexcept
            {
                freelist_init();
            } 

            [[nodiscard]] inline constexpr Data_t& operator[](const key_type key) noexcept
            {
                runTimeAssertion(is_valid(key), "Invalid key passed.");

                return data_[indices_[key.id].id];
            }

            [[nodiscard]] inline constexpr const Data_t& operator[](const key_type key) const noexcept
            {
                runTimeAssertion(is_valid(key), "Invalid key passed.");

                return data_[indices_[key.id].id];
            }

            constexpr bool erase(const key_type key) noexcept;

            [[nodiscard]] constexpr iterator begin() noexcept
            {
                return data_.data();
            }

            [[nodiscard]] constexpr iterator end() noexcept
            {
                return data_.data() + size_;
            }

            [[nodiscard]] constexpr const_iterator cbegin() const noexcept
            {
                return data_.data();
            }

            [[nodiscard]] constexpr const_iterator cend() const noexcept
            {
                return data_.data() + size_;
            }

            [[nodiscard]] constexpr bool is_valid(const key_type key) const noexcept
            {
                return key.id < capacity_ && indices_[key.id].gen == key.gen;
            }

            //Deleted functions
            SlotMap(const SlotMap& rhs) = delete;
            SlotMap& operator=(const SlotMap& rhs) = delete;

        private:
            [[nodiscard]] constexpr index_type allocate();
            constexpr void free(const key_type) noexcept;

            constexpr void freelist_init() noexcept
            {
                indices_.resize(capacity_);
                data_.resize(capacity_);
                erase_.resize(capacity_);

                index_type i = 0;
                for(key_type& index : indices_)
                {
                    index.id = ++i;
                    index.gen = generation_++; // Invalidates every key handed out before
                }
                freelist_ = 0ULL;
                size_ = 0ULL;
            }

            index_type                       size_      {0ULL};
            index_type                       capacity_  {Capacity};
            index_type                       freelist_  {0ULL};
            gen_type                         generation_{1ULL}; // Starts at 1 so a default key {0,0} is never valid
            std::vector<  key_type> indices_   {};
            std::vector<value_type> data_      {};
            std::vector<index_type> erase_     {};

    };


    template<typename Data_t, std::size_t Capacity>
    constexpr SlotMap<Data_t,Capacity>::SlotMap()
    {
        compileTimeAssertion(Capacity > 0);
        clear();
    }

    template<typename Data_t, std::size_t Capacity>
    inline void constexpr SlotMap<Data_t,Capacity>::resize()
    {
        warning("Resize of Slotmap");

        index_type idx = indices_.size();

        capacity_*=2;
        indices_.resize(capacity_);
        data_.resize(capacity_);
        erase_.resize(capacity_);


        for(index_type i{idx}; i < indices_.size(); ++i)
        {   
            indices_[i].id = i + 1;
        }

    }


    template<typename Data_t, std::size_t Capacity>
    HeatStroke::SlotMap<Data_t, Capacity>::index_type
    constexpr SlotMap<Data_t, Capacity>::allocate()
    {
        if(size_ >= capacity_)
        {
            resize();
        }
        
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
        runTimeAssertion(is_valid(key), "Call it with an invalid key"); // This cant happen.

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
            warning("Deleting in Slotmap with an invalid key");
            return false; // Early return, invalid key.
        }
        free(key);
        return true;
    }

}