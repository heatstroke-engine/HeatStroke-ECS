#pragma once

#include <cassert>

namespace MP
{
    template<typename T, T VAL>
    struct constant
    {
        static constexpr T value {VAL};
    };

    /*
        is_same will store a bool value depending of the given T and U types been the same.
    */
    template<typename T, typename U>
    struct is_same : constant<bool, false>
    {
    };

    template<typename T>
    struct is_same<T,T> : constant<bool, true>
    {
    };
    template<typename T, typename U>
    constexpr bool is_same_v = is_same<T,U>::value;

    template<typename T>
    struct type_id
    {
        using type = T;
    };

    /*
        nth_type will return the N type in the given type pack.
    */

    template<std::size_t N, typename... Ts>
    struct nth_type
    {
        static_assert(sizeof...(Ts) != 0, "Error: TypeList with 0 types.");
    };

    template<std::size_t N, typename... Ts>
    using nth_type_t = typename nth_type<N, Ts...>::type;

    template<typename T, typename... Ts>
    struct nth_type<0, T, Ts...> : type_id<T>
    {

    };

    template<std::size_t N, typename T, typename... Ts>
    struct nth_type<N, T, Ts...> : type_id < nth_type_t<N-1,Ts...> > 
    {

    };

    /*
        pos_type will return the position that type T has in the given pack
    */
    template<typename T, typename... Ts>
    struct pos_type
    {
        static_assert(sizeof...(Ts) != 0 );
    };

    template<typename T, typename... Ts>
    struct pos_type<T,T,Ts...> : constant<std::size_t,0>
    {

    };

    template<typename T, typename U,typename... Ts>
    struct pos_type<T,U,Ts...> : constant<std::size_t, pos_type<T,Ts...>::value + 1>
    {

    };

    template<typename T, typename... Ts>
    constexpr std::size_t pos_type_v = pos_type<T, Ts...>::value;

    /*
        Typelist will handle packs of types.
    */
    template<typename... Ts>
    struct Typelist
    {
        consteval static std::size_t size() noexcept
        {
            return sizeof...(Ts);
        }

        template<typename T>
        consteval static bool contains() noexcept
        {
            return (false || ... || is_same_v<T,Ts>);
        }

        template<typename T>
        consteval static std::size_t pos() noexcept
        {
            static_assert(contains<T>() && "T is not part of the typelist");
            return pos_type_v<T,Ts...>;
        }

    };

    /*
        type_traits will handle basic traits for typelists.
    */
    template<typename TL>
    struct type_traits
    {
        consteval static std::size_t size() noexcept 
        {
            return TL::size();
        }

        template<typename T>
        consteval static std::size_t id() noexcept
        {
            compileTimeAssertion(TL::template contains<T>());
            return TL::template pos<T>();
        }

        template<typename T>
        consteval static std::size_t mask() noexcept
        {
            return ( 1 << id<T>() );
        }
    };
    
}