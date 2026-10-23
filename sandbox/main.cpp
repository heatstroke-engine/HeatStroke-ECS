#include <iostream>

#include <swelter.hpp>
#include <log.hpp>
#include <format>


void seetype(auto)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

struct TPlayer {};
struct NonMove 
{
    NonMove(NonMove&& other) = delete;
};
struct CTransform 
{
    float x;
    float y;
};

int main() {

    logInit();

    auto id1 = Swelter::CmpRegistry::id<int>();
    auto id2 = Swelter::CmpRegistry::id<float>();
    auto id3 = Swelter::CmpRegistry::id<double>();
    auto id4 = Swelter::CmpRegistry::id<const int>();
    auto id5 = Swelter::CmpRegistry::id<const std::string>();
    auto id6 = Swelter::CmpRegistry::id<TPlayer>();


    logPrint(std::format("ID1 {}", id1));
    logPrint(std::format("ID2 {}", id2));
    logPrint(std::format("ID3 {}", id3));
    logPrint(std::format("ID4 {}", id4));
    logPrint(std::format("ID5 {}", id5));
    logPrint(std::format("ID6 {}", id6));

    Swelter::ComponentInfo stringinfo = Swelter::CmpRegistry::info<std::string>();
    Swelter::ComponentInfo taginfo = Swelter::CmpRegistry::info<TPlayer>();

    debugAssertion(!stringinfo.trivially_copyable);
    debugAssertion(!stringinfo.is_empty);

    debugAssertion(taginfo.trivially_copyable);
    debugAssertion(taginfo.size == 0);
    debugAssertion(taginfo.is_empty);
    debugAssertion(Swelter::CmpRegistry::count() == 5);

    std::byte buff1[4096];
    void* ptr = &buff1[0];
    std::size_t espacio_restante = sizeof(buff1) - 0;
    void* place = std::align(stringinfo.align, stringinfo.size, ptr, espacio_restante );
    std::byte buff2[4096];
    void* ptr2 = &buff2[0];
    espacio_restante = sizeof(buff2) - 0;
    void* place2 = std::align(stringinfo.align, stringinfo.size, ptr2, espacio_restante );

    new (place) std::string("ODIOMIVIDAdfdsfdsfdsfwefwenkjfweshfuiwesjhifugwesnfui");

    stringinfo.move_construct(place2,place);
    stringinfo.destroy(place);

    logPrint(*static_cast<std::string*>(place2));

    stringinfo.destroy(place2);

    return 0;
}
