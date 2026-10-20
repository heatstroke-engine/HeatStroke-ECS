#include "benchmarks.hpp"

#include <benchmark/benchmark.h>

/*
    Create N empty entities in an empty manager.
    The manager is rebuilt every iteration (paused); its construction is not measured.
*/
static void BM_CREATEENTITES(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::optional<GameEngine> GE;
    for (auto _ : state)
    {
        state.PauseTiming();
        GE.emplace();         
        state.ResumeTiming();
        for(int64_t i{}; i < n; ++i)
        {
            benchmark::DoNotOptimize(GE->createEntity());
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_CREATEENTITES)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Add 1 component (CPhysics) to N empty entities.
    Adding changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_CREATE1CMP(benchmark::State& state)
{
    
    const int64_t  n = state.range(0);
    std::optional<GameEngine> GE;
    std::vector<HeatStroke::Key> keys;
    keys.resize(n);
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        GE.emplace();     
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = GE->createEntity();
        }    
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            GE->addComponent<CPhysics>(keys[i],CPhysics{});
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_CREATE1CMP)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Add 3 components (CPhysics, CRender, CTransform) to N empty entities with addComponents.
    Adding changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_CREATE3CMPS(benchmark::State& state)
{
    
    const int64_t  n = state.range(0);
    std::optional<GameEngine> GE;
    std::vector<HeatStroke::Key> keys;
    keys.resize(n);
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        GE.emplace();     
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = GE->createEntity();
        }    
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            GE->addComponents<CPhysics,CRender,CTransform>(keys[i],CPhysics{},CRender{},CTransform{});
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_CREATE3CMPS)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Add 5 components (CPhysics, CRender, CTransform, CGeneric<0>, CGeneric<1>) to N empty entities.
    Adding changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_CREATE5CMPS(benchmark::State& state)
{
    
    const int64_t  n = state.range(0);
    std::optional<GameEngine> GE;
    std::vector<HeatStroke::Key> keys;
    keys.resize(n);
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        GE.emplace();     
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = GE->createEntity();
        }    
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            GE->addComponents<CPhysics,CRender,CTransform,CGeneric<0>,CGeneric<1> >(keys[i],
                                                                            CPhysics{},
                                                                            CRender{},
                                                                            CTransform{},
                                                                            CGeneric<0>{},
                                                                            CGeneric<1>{}
                                                                            );
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_CREATE5CMPS)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Destroy N entities with 1 component (CPhysics). No systems registered, so no caches to update.
    Destroying changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_ERASEENTITYW1CMP(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::vector<HeatStroke::Key> keys;
    keys.resize(n);
    std::optional<GameEngine> GE;
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        GE.emplace();         
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = GE->createEntity();
            GE->addComponent<CPhysics>(keys[i],CPhysics{});
        }
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            GE->removeEntity(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ERASEENTITYW1CMP)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Destroy N entities with 3 components (CPhysics, CRender, CTransform). No systems registered.
    Destroying changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_ERASEENTITYW3CMPS(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::vector<HeatStroke::Key> keys;
    keys.resize(n);
    std::optional<GameEngine> GE;
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        GE.emplace();         
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = GE->createEntity();
            GE->addComponents<CPhysics,CRender,CTransform>(keys[i],
                                                    CPhysics{},
                                                    CRender{},
                                                    CTransform{}
                                                );
        }
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            GE->removeEntity(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ERASEENTITYW3CMPS)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Destroy N entities with 5 components (CPhysics, CRender, CTransform, CGeneric<0>, CGeneric<1>).
    No systems registered. Destroying changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_ERASEENTITYW5CMPS(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::vector<HeatStroke::Key> keys;
    keys.resize(n);
    std::optional<GameEngine> GE;
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        GE.emplace();         
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = GE->createEntity();
            GE->addComponents<CPhysics,CRender,CTransform,CGeneric<0>,CGeneric<1> >(keys[i],
                                                                            CPhysics{},
                                                                            CRender{},
                                                                            CTransform{},
                                                                            CGeneric<0>{},
                                                                            CGeneric<1>{}
                                                                            );
        }
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            GE->removeEntity(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ERASEENTITYW5CMPS)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Remove 1 component (CPhysics) from N entities. No systems registered, so no caches to update.
    Removing changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_ERASECMP(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::vector<HeatStroke::Key> keys;
    keys.resize(n);
    std::optional<GameEngine> GE;
    for (auto _ : state)
    {
        state.PauseTiming();
        GE.emplace();         
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = GE->createEntity();
            GE->addComponent<CPhysics>(keys[i],CPhysics{});
        }
        state.ResumeTiming();
        for(int64_t i{}; i < n; ++i)
        {
            GE->removeComponent<CPhysics>(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ERASECMP)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Remove 2 components (CPhysics, CTransform) from N entities with removeComponents. No systems registered.
    Removing changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_ERASECMPS(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::vector<HeatStroke::Key> keys;
    keys.resize(n);
    std::optional<GameEngine> GE;
    for (auto _ : state)
    {
        state.PauseTiming();
        GE.emplace();         
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = GE->createEntity();
            GE->addComponents<CPhysics,CTransform>(keys[i],CPhysics{},CTransform{});
        }
        state.ResumeTiming();
        for(int64_t i{}; i < n; ++i)
        {
            GE->removeComponents<CPhysics,CTransform>(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ERASECMPS)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Destroy N empty entities. No systems registered.
    Destroying changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_DESTROYENTITIES(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::vector<HeatStroke::Key> keys;
    keys.resize(n);
    std::optional<GameEngine> GE;
    for (auto _ : state)
    {
        state.PauseTiming();
        GE.emplace();         
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = GE->createEntity();
        }
        state.ResumeTiming();
        for(int64_t i{}; i < n; ++i)
        {
            GE->removeEntity(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_DESTROYENTITIES)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Access 1 component (CPhysics) of N entities with getComponent, in creation order, and write it.
    Accessing does not change the structure of the world, so it is built only once.
*/
static void BM_GETCMP(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    std::vector<HeatStroke::Key> keys;
    keys.resize(n);
    GameEngine GE;
    for(int64_t i{}; i < n; ++i)
    {
        keys[i] = GE.createEntity();
        GE.addComponent<CPhysics>(keys[i],CPhysics{});
    }

    // Perform benchmark
    for (auto _ : state)
    {
        for(int64_t i{}; i < n; ++i)
        {
            GE.getComponent<CPhysics>(keys[i]).vx = 33.f;
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_GETCMP)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Update RenderSystem (1 component: CRender) over N entities that all match its signature.
    Updating does not change the structure of the world, so it is built only once.
*/
static void BM_ITERATESINGLECMP(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    GameEngine GE;
    RenderSystem rendersys{};
    for(int64_t i{}; i < n; ++i)
    {
        HeatStroke::Key key = GE.createEntity();
        GE.addComponents<CRender>(key,CRender{});
    }
    GE.registerSystem<RenderSystem>();

    // Perform benchmark
    for (auto _ : state)
    {
        rendersys.update(GE);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ITERATESINGLECMP)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Update PhysicsSystem (2 components: CPhysics, CTransform) over N entities that all match its signature.
    Updating does not change the structure of the world, so it is built only once.
*/
static void BM_ITERATETWOCMPS(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    GameEngine GE;
    PhysicsSystem physys{};
    for(int64_t i{}; i < n; ++i)
    {
        HeatStroke::Key key = GE.createEntity();
        GE.addComponents<CPhysics,CTransform>(key,CPhysics{},CTransform{});
    }
    GE.registerSystem<PhysicsSystem>();

    // Perform benchmark
    for (auto _ : state)
    {
        physys.update(GE);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ITERATETWOCMPS)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);


/*
    Update PhysicsSystem over N entities where only half (the odd ones) match its signature:
    all have CPhysics, only half have CTransform. Shows the cost of non contiguous iteration.
    Updating does not change the structure of the world, so it is built only once.
*/
static void BM_ITERATETWOCMPSHALF(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    GameEngine GE;
    PhysicsSystem physys{};
    for(int64_t i{}; i < n; ++i)
    {
        HeatStroke::Key key = GE.createEntity();
        GE.addComponent<CPhysics>(key,CPhysics{});
        if(i % 2)
        {
            GE.addComponent<CTransform>(key,CTransform{});
        }
    }
    GE.registerSystem<PhysicsSystem>();

    // Perform benchmark
    for (auto _ : state)
    {
        physys.update(GE);
    }
    // Only half of the entities match the system signature
    state.SetItemsProcessed(state.iterations() * (state.range(0) / 2));
}
BENCHMARK(BM_ITERATETWOCMPSHALF)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Add and remove a component to N entities. Adding and then removing leaves the world as it was,
    so it can be built only once. Each entity counts as 2 operations (add + remove).
*/

// CHealth is not in the signature of any registered system: structural change + checking the caches
static void BM_ADDREMOVECMP(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    std::vector<HeatStroke::Key> keys;
    keys.resize(n);
    GameEngine GE;
    for(int64_t i{}; i < n; ++i)
    {
        keys[i] = GE.createEntity();
        GE.addComponents<CPhysics,CTransform>(keys[i],CPhysics{},CTransform{});
    }
    GE.registerSystem<PhysicsSystem>();
    GE.registerSystem<RenderSystem>();

    // Perform benchmark
    for (auto _ : state)
    {
        for(int64_t i{}; i < n; ++i)
        {
            GE.addComponent<CHealth>(keys[i],CHealth{});
        }
        for(int64_t i{}; i < n; ++i)
        {
            GE.removeComponent<CHealth>(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0) * 2);
}
BENCHMARK(BM_ADDREMOVECMP)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

// CHealth completes the signature of HealthSystem: every add inserts the entity in its cache
// and every remove erases it from the cache
static void BM_ADDREMOVECMPCACHED(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    std::vector<HeatStroke::Key> keys;
    keys.resize(n);
    GameEngine GE;
    for(int64_t i{}; i < n; ++i)
    {
        keys[i] = GE.createEntity();
        GE.addComponents<CPhysics,CTransform>(keys[i],CPhysics{},CTransform{});
    }
    GE.registerSystem<PhysicsSystem>();
    GE.registerSystem<RenderSystem>();
    GE.registerSystem<HealthSystem>();

    // Perform benchmark
    for (auto _ : state)
    {
        for(int64_t i{}; i < n; ++i)
        {
            GE.addComponent<CHealth>(keys[i],CHealth{});
        }
        for(int64_t i{}; i < n; ++i)
        {
            GE.removeComponent<CHealth>(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0) * 2);
}
// No 1M: removing from the cache is a linear std::erase, so the test is O(n^2) and 1M takes minutes per iteration
BENCHMARK(BM_ADDREMOVECMPCACHED)->Arg(1000)->Arg(10000)->Arg(100000);

/*
    Register a system with N entities already created: cost of creating its cache and filling it.
    Registering the same signature again would reuse the cache, so the world is rebuilt every iteration.
*/
static void BM_REGISTERSYSTEM(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::optional<GameEngine> GE;
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        GE.emplace();
        for(int64_t i{}; i < n; ++i)
        {
            HeatStroke::Key key = GE->createEntity();
            GE->addComponents<CPhysics,CTransform>(key,CPhysics{},CTransform{});
        }
        state.ResumeTiming();

        // Perform benchmark
        GE->registerSystem<PhysicsSystem>();
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_REGISTERSYSTEM)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Destroy N entities that are in the caches of the registered systems. Every entity matches
    PhysicsSystem and HealthSystem, so removeEntity erases it from 2 caches.
*/
static void BM_DESTROYENTITIESCACHED(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::vector<HeatStroke::Key> keys;
    keys.resize(n);
    std::optional<GameEngine> GE;
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        GE.emplace();
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = GE->createEntity();
            GE->addComponents<CPhysics,CTransform,CHealth>(keys[i],CPhysics{},CTransform{},CHealth{});
        }
        GE->registerSystem<PhysicsSystem>();
        GE->registerSystem<RenderSystem>();
        GE->registerSystem<HealthSystem>();
        state.ResumeTiming();

        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            GE->removeEntity(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
// No 1M: removing from the caches is a linear std::erase, so the test is O(n^2) and 1M takes minutes per iteration
BENCHMARK(BM_DESTROYENTITIESCACHED)->Arg(1000)->Arg(10000)->Arg(100000);

/*
    Game scenario: one frame updating several systems over a world with heterogeneous entities.
        - 50% units:       CTransform, CPhysics, CRender, CHealth (half TPlayer, half TEnemy)
        - 30% props:       CTransform, CRender
        - 20% projectiles: CTransform, CPhysics (TBullet)
    Updating does not change the structure of the world, so it is built only once.
*/
static void BM_GAMESCENARIO(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    GameEngine GE;
    for(int64_t i{}; i < n; ++i)
    {
        HeatStroke::Key key = GE.createEntity();
        const int64_t type = i % 10;
        if(type < 5)
        {
            GE.addComponents<CTransform,CPhysics,CRender,CHealth>(key,CTransform{},CPhysics{},CRender{},CHealth{});
            if(i % 2)
            {
                GE.addTag<TPlayer>(key);
            }
            else
            {
                GE.addTag<TEnemy>(key);
            }
        }
        else if(type < 8)
        {
            GE.addComponents<CTransform,CRender>(key,CTransform{},CRender{});
        }
        else
        {
            GE.addComponents<CTransform,CPhysics>(key,CTransform{},CPhysics{});
            GE.addTag<TBullet>(key);
        }
    }
    GE.registerSystem<PhysicsSystem>();
    GE.registerSystem<RenderSystem>();
    GE.registerSystem<HealthSystem>();

    PhysicsSystem physys{};
    RenderSystem  rendersys{};
    HealthSystem  healthsys{};

    // Perform benchmark
    for (auto _ : state)
    {
        physys.update(GE);
        healthsys.update(GE);
        rendersys.update(GE);
    }
    // Entities in the world updated per second (one frame per iteration)
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_GAMESCENARIO)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

BENCHMARK_MAIN();
