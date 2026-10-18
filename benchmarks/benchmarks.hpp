#pragma once
#include <ecs.hpp>

/*
    Dummy CMPs
*/
struct CTransform
{
    float x {0.F};
    float y {0.F};
};
struct CPhysics
{
    float vx {0.F};
    float vy {0.F};
};
struct CRender
{
    char sprite {'@'};
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
using Entity = GameEngine::Entity;