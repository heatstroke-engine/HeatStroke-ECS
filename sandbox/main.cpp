#include <iostream>

#include <ecs.hpp>


/*
    Dummy CMPs
*/
struct CTransform
{

};
struct CPhysics
{

};
struct CRender
{

};
/*
    Dummy TAgs
*/
struct TPlayer{};
struct TEnemy{};
struct TBullet{};

using CMPs = MP::Typelist<CTransform, CPhysics, CRender>;
using TAGs = MP::Typelist<TPlayer, TEnemy, TBullet >;

using GameEngine = HeatStroke::EntityManager<CMPs, TAGs>;

void seetype(auto)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

int main() {

    GameEngine GE;

    HeatStroke::ComponentStorage<CMPs,TAGs> CS{};

    seetype(GameEngine::Entity::Key_Storage_t{});



    return 0;
}
