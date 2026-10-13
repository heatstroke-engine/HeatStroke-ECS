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
using Entity = HeatStroke::EntityManager<CMPs, TAGs>::Entity;

void seetype(auto)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

int main() {

    GameEngine GE;

    Entity& e = GE.createEntity();
    GE.addComponents<CPhysics,CRender>(e, CPhysics{}, CRender{});

    auto key = e.getComponentKey<CPhysics>();
    auto key2 = e.getComponentKey<CRender>();

    std::cout << key.gen << " " << key.id << "\n";
    std::cout << key2.gen << " " << key2.id << "\n";

    GE.deleteComponents<CRender,CPhysics>(e);

    GE.addComponents<CPhysics,CRender>(e, CPhysics{}, CRender{});

    std::cout << key.gen << " " << key.id << "\n";
    std::cout << key2.gen << " " << key2.id << "\n";

    GE.removeEntity(1);

    return 0;
}
