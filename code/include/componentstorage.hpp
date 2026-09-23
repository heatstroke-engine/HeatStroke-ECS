#pragma once

#include <slotmap.hpp>
#include <tuple>
#include <typetraits.hpp>
//#include <sourcelocation>  <<<<<<<<------------

namespace HeatStroke
{
    template<typename CMPLIST>
    struct cmp_traits : MP::type_traits<CMPLIST>
    {

    };

    template<typename TAGLIST>
    struct tag_traits : MP::type_traits<TAGLIST>
    {

    };

    template<typename CMPSLIST, typename TAGSLIST, std::size_t Capacity = 1000>
    class ComponentStorage
    {
        public:
            using cmpinfo = cmp_traits<CMPSLIST>;
            using taginfo = tag_traits<TAGSLIST>;
            template<typename T>
            using cmp_to_slotmap = HeatStroke::SlotMap<T,Capacity>;
            using Cmps_slotmaps = MP::for_all_insert_template_t<cmp_to_slotmap, CMPSLIST>;
            using Storage_t = MP::replace_t<std::tuple, Cmps_slotmaps>;

            ComponentStorage() = default;

            template<typename CMP>
            constexpr static std::size_t getId()
            {
                return cmpinfo::template id<CMP>();
            }

            template<typename... CMPs>
            constexpr static std::size_t getMask() noexcept
            {
                return cmpinfo::template mask<CMPs...>();
            }

            template<typename CMP>
            constexpr HeatStroke::SlotMap<CMP,Capacity>& getStorage() noexcept
            {
                return std::get<getId<CMP>()>(cmps_);
            }

            // Deleted functions
            ComponentStorage(const ComponentStorage& rhs) = delete;
            ComponentStorage& operator=(const ComponentStorage& rhs) = delete;
        private:
            Storage_t cmps_ {};

    };

};