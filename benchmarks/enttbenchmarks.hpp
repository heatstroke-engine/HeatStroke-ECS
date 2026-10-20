#pragma once
#include <entt/entt.hpp>

/*
    Dummy CMPs. They must be the same as in benchmarks.hpp, so both ECS move the same data.
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
    Dummy TAgs. In EnTT a tag is an empty component.
*/
struct TPlayer{};
struct TEnemy{};
struct TBullet{};

/*
    Systems.
    In HeatStroke a registered system keeps a cache with the entities that match its signature, updated on
    every structural change. The EnTT equivalent is an owning group: it keeps the matching entities packed
    and is updated on every structural change too. So registering a system = creating its group, and
    update() iterates the group. The *_VIEW versions iterate a view instead (no cache, nothing to keep updated).

    Group ownership: a component can only be owned by one group, so HealthSystem owns CHealth and only
    observes CTransform (owned by PhysicsSystem).
*/
struct PhysicsSystem
{
    static void registerIn(entt::registry& reg)
    {
        static_cast<void>(reg.group<CPhysics, CTransform>());
    }

    void update(entt::registry& reg) const noexcept
    {
        reg.group<CPhysics, CTransform>().each([](const entt::entity, const CPhysics& phy, CTransform& tra){
            tra.x += phy.vx;
            tra.y += phy.vy;
        });
    }

    void updateView(entt::registry& reg) const noexcept
    {
        reg.view<const CPhysics, CTransform>().each([](const entt::entity, const CPhysics& phy, CTransform& tra){
            tra.x += phy.vx;
            tra.y += phy.vy;
        });
    }
};

struct RenderSystem
{
    static void registerIn(entt::registry& reg)
    {
        static_cast<void>(reg.group<CRender>());
    }

    void update(entt::registry& reg) const noexcept
    {
        reg.group<CRender>().each([](const entt::entity, CRender& ren){
            ren.sprite = 'x';
        });
    }

    void updateView(entt::registry& reg) const noexcept
    {
        reg.view<CRender>().each([](const entt::entity, CRender& ren){
            ren.sprite = 'x';
        });
    }
};

struct HealthSystem
{
    static void registerIn(entt::registry& reg)
    {
        static_cast<void>(reg.group<CHealth>(entt::get<const CTransform>));
    }

    void update(entt::registry& reg) const noexcept
    {
        reg.group<CHealth>(entt::get<const CTransform>).each([](const entt::entity, CHealth& hea, const CTransform&){
            --hea.hp;
        });
    }

    void updateView(entt::registry& reg) const noexcept
    {
        reg.view<CHealth, const CTransform>().each([](const entt::entity, CHealth& hea, const CTransform&){
            --hea.hp;
        });
    }
};
