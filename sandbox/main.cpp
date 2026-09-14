#include <iostream>

#include <slotmap.hpp>


int main() {

    HeatStroke::SlotMap<int, 10> slotMap;   
    
    [[maybe_unused]] auto key = slotMap.push_back(3);
    [[maybe_unused]]auto key2 = slotMap.push_back(7);
    [[maybe_unused]]slotMap.push_back(34);
    [[maybe_unused]]slotMap.push_back(25);
    [[maybe_unused]]slotMap.push_back(456);

    slotMap.erase(key);
    slotMap.erase(key2);

    for (auto it = slotMap.cbegin(); it != slotMap.cend(); it++ )
    {
        std::cout << "Item: " << *it << "\n";
    }


    
    return 0;
}
