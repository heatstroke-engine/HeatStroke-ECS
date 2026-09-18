#pragma once

#include <cassert>
#include <cstdint>
#include <tuple>

namespace MP
{   
    // Typelist FW
    template<typename... Ts>
    struct Typelist;


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

    /*
        IFT will return a type depending of the given condition
    */
    template<bool C, typename T, typename F>
    struct IFT : type_id<F>
    {

    };

    template<typename T, typename F>
    struct IFT<true,T,F> : type_id<T>
    {

    };

    template<bool C, typename T, typename U>
    using IFT_t = IFT<C,T,U>::type;

    template<typename T, typename... Ts>
    constexpr std::size_t pos_type_v = pos_type<T, Ts...>::value;

    /*
        replace will allow to create a type from the given template New of the List.
    */
    template<template <typename...> class New, typename List>
    struct replace
    {

    };

    template<template <typename...> class New, typename... Ts>
    struct replace<New, Typelist<Ts...>> : type_id<New<Ts...>>
    {

    };
    
    template<template <typename...> class New, typename List>
    using replace_t =  replace<New, List>::type;

    template<template <typename, std::size_t> class Container,std::size_t Size, typename List>
    struct fill_container
    {

    };

    template<template <typename, std::size_t> class Container,std::size_t Size,  typename... Ts>
    struct fill_container<Container, Size, Typelist<Ts...>> : type_id<Typelist<Container<Ts, Size>...>>
    {

    };

    template<template <typename, std::size_t> class Container,std::size_t Size, typename List>
    using fill_container_t = fill_container<Container, Size, List>::type;
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
        constexpr static uint8_t list_size = TL::size();
        using mask_type =   IFT_t< list_size <= 32 ,
                                    IFT_t< list_size <= 16, 
                                    IFT_t<list_size <= 8, uint8_t, 
                                    uint16_t>, 
                                    uint32_t>, 
                                    uint64_t>;

        consteval static uint8_t size() noexcept 
        {
            return TL::size();
        }

        template<typename T>
        consteval static uint8_t id() noexcept
        {
            compileTimeAssertion(TL::template contains<T>());
            return TL::template pos<T>();
        }

        template<typename T>
        consteval static mask_type mask() noexcept
        {
            return ( 1 << id<T>() );
        }
    };
    
}