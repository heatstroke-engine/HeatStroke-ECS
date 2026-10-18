#include <iostream>

#include <ecs.hpp>
#include <log.hpp>
#include <format>
/*
    Dummy CMPs
*/
struct CTransform
{
    float x {0UL};
    float y {0UL};
};
struct CPhysics
{
    float vx {0UL};
    float vy {0UL};
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

using GameEngine = HeatStroke::EntityManager<CMPs, TAGs,1>;
using Entity = GameEngine::Entity;

/*
    Dummy Systems
*/
struct PhysicsSystem
{
    using SCMPs = MP::Typelist<CPhysics, CTransform>;
    using STAGs = MP::Typelist<>;

    inline static std::size_t cache_index = 0;

    void update(GameEngine& GE) const noexcept
    {

    }
};

struct RenderSystem
{
    using SCMPs = MP::Typelist<CRender>;
    using STAGs = MP::Typelist<>;

    inline static std::size_t cache_index = 0;

    void update(GameEngine& GE) const noexcept
    {

    }
};

void seetype(auto)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

int main() {

    HeatStroke::Log::initialize();

    GameEngine GE;

    Entity& e = GE.createEntity();
    CPhysics p{33,57};
    CTransform t{0,0};
    CRender r{};
    GE.addComponents(e, p,t,r);

    Entity& e2 = GE.createEntity();

    GE.addComponent(e2, CPhysics{5,5});


    GE.registerSystem<PhysicsSystem>();
    GE.registerSystem<RenderSystem>();

    HeatStroke::Log::print("PHY System Cache id: " + std::to_string(PhysicsSystem::cache_index));
    HeatStroke::Log::print("RENDER System Cache id: " + std::to_string(RenderSystem::cache_index));

    return 0;
}
