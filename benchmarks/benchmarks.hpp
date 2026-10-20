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
struct CHealth
{
    int hp {100};
};
template<std::size_t X>
struct CGeneric
{
    std::size_t x = X;
};
/*
    Dummy TAgs
*/
struct TPlayer{};
struct TEnemy{};
struct TBullet{};



using CMPs = MP::Typelist<CTransform, CPhysics, CRender,CGeneric<0>,CGeneric<1>,CHealth>;
using TAGs = MP::Typelist<TPlayer, TEnemy, TBullet >;

using GameEngine = HeatStroke::EntityManager<CMPs, TAGs>;
using Entity = GameEngine::Entity;

struct PhysicsSystem
{
    using SCMPs = MP::Typelist<CPhysics, CTransform>;
    using STAGs = MP::Typelist<>;

    inline static std::size_t cache_index = 0;

    void update(GameEngine& GE) const noexcept
    {
        auto&& f = ([](const HeatStroke::Key , const CPhysics& phy, CTransform& tra){
            tra.x += phy.vx;
            tra.y += phy.vy;
        });

        GE.foreach<PhysicsSystem>(f);
    }
};

struct RenderSystem
{
    using SCMPs = MP::Typelist<CRender>;
    using STAGs = MP::Typelist<>;

    inline static std::size_t cache_index = 0;

    void update(GameEngine& GE) const noexcept
    {
        auto&& f = ([](const HeatStroke::Key , CRender& ren){
            ren.sprite = 'x';
        });

        GE.foreach<RenderSystem>(f);
    }
};

struct HealthSystem
{
    using SCMPs = MP::Typelist<CHealth, CTransform>;
    using STAGs = MP::Typelist<>;

    inline static std::size_t cache_index = 0;

    void update(GameEngine& GE) const noexcept
    {
        auto&& f = ([](const HeatStroke::Key , CHealth& hea, const CTransform&){
            --hea.hp;
        });

        GE.foreach<HealthSystem>(f);
    }
};
