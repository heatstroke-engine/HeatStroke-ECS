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
using TAGs = MP::Typelist<TPlayer, TEnemy, TBullet>;

using GameEngine = HeatStroke::EntityManager<CMPs, TAGs>;
int main() {

    GameEngine GE;

    compileTimeAssertion(TAGs::pos<TEnemy>() == 1);

    return 0;
}
