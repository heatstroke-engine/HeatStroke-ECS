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
    std::uint32_t x {0UL};
    std::uint32_t y {0UL};
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

    compileTimeAssertion(std::is_same_v<CMPs::at<2>,CRender >);

    GameEngine GE;

    Entity& e = GE.createEntity();
    CPhysics p{33,57};
    CRender r{};
    GE.addComponents(e, p,r);
    
    GE.removeComponents<CPhysics,CRender>(e);
    GE.addComponents(e, p,r);
    GE.addTag<TPlayer>(e);
        
    const CPhysics& ph = GE.getComponent<CPhysics>(e);

    

    GE.removeEntity(1);

    return 0;
}
