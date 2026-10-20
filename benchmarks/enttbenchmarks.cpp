#include <benchmark/benchmark.h>
#include <entt/entt.hpp>
#include "enttbenchmarks.hpp"

#include <optional>
#include <vector>

/*
    Same tests as benchmarks.cpp, with the same names, implemented with EnTT.
    Where HeatStroke registers systems, here their groups are created (see enttbenchmarks.hpp).
*/

/*
    Create N empty entities in an empty registry.
    The registry is rebuilt every iteration (paused); its construction is not measured.
*/
static void BM_CREATEENTITES(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::optional<entt::registry> reg;
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        reg.emplace();
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            benchmark::DoNotOptimize(reg->create());
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
    std::optional<entt::registry> reg;
    std::vector<entt::entity> keys;
    keys.resize(n);
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        reg.emplace();
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = reg->create();
        }
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            reg->emplace<CPhysics>(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_CREATE1CMP)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Add 3 components (CPhysics, CRender, CTransform) to N empty entities.
    Adding changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_CREATE3CMPS(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::optional<entt::registry> reg;
    std::vector<entt::entity> keys;
    keys.resize(n);
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        reg.emplace();
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = reg->create();
        }
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            reg->emplace<CPhysics>(keys[i]);
            reg->emplace<CRender>(keys[i]);
            reg->emplace<CTransform>(keys[i]);
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
    std::optional<entt::registry> reg;
    std::vector<entt::entity> keys;
    keys.resize(n);
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        reg.emplace();
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = reg->create();
        }
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            reg->emplace<CPhysics>(keys[i]);
            reg->emplace<CRender>(keys[i]);
            reg->emplace<CTransform>(keys[i]);
            reg->emplace<CGeneric<0>>(keys[i]);
            reg->emplace<CGeneric<1>>(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_CREATE5CMPS)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Destroy N entities with 1 component (CPhysics). No groups, so nothing to keep updated.
    Destroying changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_ERASEENTITYW1CMP(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::vector<entt::entity> keys;
    keys.resize(n);
    std::optional<entt::registry> reg;
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        reg.emplace();
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = reg->create();
            reg->emplace<CPhysics>(keys[i]);
        }
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            reg->destroy(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ERASEENTITYW1CMP)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Destroy N entities with 3 components (CPhysics, CRender, CTransform). No groups.
    Destroying changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_ERASEENTITYW3CMPS(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::vector<entt::entity> keys;
    keys.resize(n);
    std::optional<entt::registry> reg;
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        reg.emplace();
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = reg->create();
            reg->emplace<CPhysics>(keys[i]);
            reg->emplace<CRender>(keys[i]);
            reg->emplace<CTransform>(keys[i]);
        }
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            reg->destroy(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ERASEENTITYW3CMPS)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Destroy N entities with 5 components (CPhysics, CRender, CTransform, CGeneric<0>, CGeneric<1>).
    No groups. Destroying changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_ERASEENTITYW5CMPS(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::vector<entt::entity> keys;
    keys.resize(n);
    std::optional<entt::registry> reg;
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        reg.emplace();
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = reg->create();
            reg->emplace<CPhysics>(keys[i]);
            reg->emplace<CRender>(keys[i]);
            reg->emplace<CTransform>(keys[i]);
            reg->emplace<CGeneric<0>>(keys[i]);
            reg->emplace<CGeneric<1>>(keys[i]);
        }
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            reg->destroy(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ERASEENTITYW5CMPS)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Remove 1 component (CPhysics) from N entities. No groups.
    erase (not remove) because, like HeatStroke, it expects the entity to have the component.
    Removing changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_ERASECMP(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::vector<entt::entity> keys;
    keys.resize(n);
    std::optional<entt::registry> reg;
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        reg.emplace();
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = reg->create();
            reg->emplace<CPhysics>(keys[i]);
        }
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            reg->erase<CPhysics>(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ERASECMP)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Remove 2 components (CPhysics, CTransform) from N entities. No groups.
    Removing changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_ERASECMPS(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::vector<entt::entity> keys;
    keys.resize(n);
    std::optional<entt::registry> reg;
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        reg.emplace();
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = reg->create();
            reg->emplace<CPhysics>(keys[i]);
            reg->emplace<CTransform>(keys[i]);
        }
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            reg->erase<CPhysics, CTransform>(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ERASECMPS)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Destroy N empty entities. No groups.
    Destroying changes the world, so it is rebuilt every iteration (paused).
*/
static void BM_DESTROYENTITIES(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::vector<entt::entity> keys;
    keys.resize(n);
    std::optional<entt::registry> reg;
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        reg.emplace();
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = reg->create();
        }
        state.ResumeTiming();
        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            reg->destroy(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_DESTROYENTITIES)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Access 1 component (CPhysics) of N entities with get, in creation order, and write it.
    Accessing does not change the structure of the world, so it is built only once.
*/
static void BM_GETCMP(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    std::vector<entt::entity> keys;
    keys.resize(n);
    entt::registry reg;
    for(int64_t i{}; i < n; ++i)
    {
        keys[i] = reg.create();
        reg.emplace<CPhysics>(keys[i]);
    }

    // Perform benchmark
    for (auto _ : state)
    {
        for(int64_t i{}; i < n; ++i)
        {
            reg.get<CPhysics>(keys[i]).vx = 33.f;
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_GETCMP)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Update RenderSystem (1 component: CRender, group) over N entities that all match its signature.
    Updating does not change the structure of the world, so it is built only once.
*/
static void BM_ITERATESINGLECMP(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    entt::registry reg;
    RenderSystem rendersys{};
    for(int64_t i{}; i < n; ++i)
    {
        const entt::entity key = reg.create();
        reg.emplace<CRender>(key);
    }
    RenderSystem::registerIn(reg);

    // Perform benchmark
    for (auto _ : state)
    {
        rendersys.update(reg);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ITERATESINGLECMP)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Update PhysicsSystem (2 components: CPhysics, CTransform, group) over N entities that all match its signature.
    Updating does not change the structure of the world, so it is built only once.
*/
static void BM_ITERATETWOCMPS(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    entt::registry reg;
    PhysicsSystem physys{};
    for(int64_t i{}; i < n; ++i)
    {
        const entt::entity key = reg.create();
        reg.emplace<CPhysics>(key);
        reg.emplace<CTransform>(key);
    }
    PhysicsSystem::registerIn(reg);

    // Perform benchmark
    for (auto _ : state)
    {
        physys.update(reg);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ITERATETWOCMPS)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Update PhysicsSystem (group) over N entities where only half (the odd ones) match its signature:
    all have CPhysics, only half have CTransform.
    Updating does not change the structure of the world, so it is built only once.
*/
static void BM_ITERATETWOCMPSHALF(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    entt::registry reg;
    PhysicsSystem physys{};
    for(int64_t i{}; i < n; ++i)
    {
        const entt::entity key = reg.create();
        reg.emplace<CPhysics>(key);
        if(i % 2)
        {
            reg.emplace<CTransform>(key);
        }
    }
    PhysicsSystem::registerIn(reg);

    // Perform benchmark
    for (auto _ : state)
    {
        physys.update(reg);
    }
    // Only half of the entities match the system signature
    state.SetItemsProcessed(state.iterations() * (state.range(0) / 2));
}
BENCHMARK(BM_ITERATETWOCMPSHALF)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Add and remove a component to N entities. Adding and then removing leaves the world as it was,
    so it can be built only once. Each entity counts as 2 operations (add + remove).
*/

// CHealth is not in any group: structural change only
static void BM_ADDREMOVECMP(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    std::vector<entt::entity> keys;
    keys.resize(n);
    entt::registry reg;
    for(int64_t i{}; i < n; ++i)
    {
        keys[i] = reg.create();
        reg.emplace<CPhysics>(keys[i]);
        reg.emplace<CTransform>(keys[i]);
    }
    PhysicsSystem::registerIn(reg);
    RenderSystem::registerIn(reg);

    // Perform benchmark
    for (auto _ : state)
    {
        for(int64_t i{}; i < n; ++i)
        {
            reg.emplace<CHealth>(keys[i]);
        }
        for(int64_t i{}; i < n; ++i)
        {
            reg.erase<CHealth>(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0) * 2);
}
BENCHMARK(BM_ADDREMOVECMP)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

// CHealth completes the signature of HealthSystem: every add inserts the entity in its group
// and every remove takes it out of the group
static void BM_ADDREMOVECMPCACHED(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    std::vector<entt::entity> keys;
    keys.resize(n);
    entt::registry reg;
    for(int64_t i{}; i < n; ++i)
    {
        keys[i] = reg.create();
        reg.emplace<CPhysics>(keys[i]);
        reg.emplace<CTransform>(keys[i]);
    }
    PhysicsSystem::registerIn(reg);
    RenderSystem::registerIn(reg);
    HealthSystem::registerIn(reg);

    // Perform benchmark
    for (auto _ : state)
    {
        for(int64_t i{}; i < n; ++i)
        {
            reg.emplace<CHealth>(keys[i]);
        }
        for(int64_t i{}; i < n; ++i)
        {
            reg.erase<CHealth>(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0) * 2);
}
// Same sizes as HeatStroke (no 1M) so both can be compared
BENCHMARK(BM_ADDREMOVECMPCACHED)->Arg(1000)->Arg(10000)->Arg(100000);

/*
    Register a system with N entities already created: create its group, which packs the matching entities.
    The group already exists after the first iteration, so the world is rebuilt every iteration.
*/
static void BM_REGISTERSYSTEM(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::optional<entt::registry> reg;
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        reg.emplace();
        for(int64_t i{}; i < n; ++i)
        {
            const entt::entity key = reg->create();
            reg->emplace<CPhysics>(key);
            reg->emplace<CTransform>(key);
        }
        state.ResumeTiming();

        // Perform benchmark
        PhysicsSystem::registerIn(*reg);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_REGISTERSYSTEM)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    Destroy N entities that are in the groups of two systems (PhysicsSystem and HealthSystem).
*/
static void BM_DESTROYENTITIESCACHED(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::vector<entt::entity> keys;
    keys.resize(n);
    std::optional<entt::registry> reg;
    for (auto _ : state)
    {
        // Set World
        state.PauseTiming();
        reg.emplace();
        for(int64_t i{}; i < n; ++i)
        {
            keys[i] = reg->create();
            reg->emplace<CPhysics>(keys[i]);
            reg->emplace<CTransform>(keys[i]);
            reg->emplace<CHealth>(keys[i]);
        }
        PhysicsSystem::registerIn(*reg);
        RenderSystem::registerIn(*reg);
        HealthSystem::registerIn(*reg);
        state.ResumeTiming();

        // Perform benchmark
        for(int64_t i{}; i < n; ++i)
        {
            reg->destroy(keys[i]);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
// Same sizes as HeatStroke (no 1M) so both can be compared
BENCHMARK(BM_DESTROYENTITIESCACHED)->Arg(1000)->Arg(10000)->Arg(100000);

/*
    Game scenario: one frame updating several systems (groups) over a world with heterogeneous entities.
        - 50% units:       CTransform, CPhysics, CRender, CHealth (half TPlayer, half TEnemy)
        - 30% props:       CTransform, CRender
        - 20% projectiles: CTransform, CPhysics (TBullet)
    Updating does not change the structure of the world, so it is built only once.
*/
static void buildGameScenario(entt::registry& reg, const int64_t n)
{
    for(int64_t i{}; i < n; ++i)
    {
        const entt::entity key = reg.create();
        const int64_t type = i % 10;
        if(type < 5)
        {
            reg.emplace<CTransform>(key);
            reg.emplace<CPhysics>(key);
            reg.emplace<CRender>(key);
            reg.emplace<CHealth>(key);
            if(i % 2)
            {
                reg.emplace<TPlayer>(key);
            }
            else
            {
                reg.emplace<TEnemy>(key);
            }
        }
        else if(type < 8)
        {
            reg.emplace<CTransform>(key);
            reg.emplace<CRender>(key);
        }
        else
        {
            reg.emplace<CTransform>(key);
            reg.emplace<CPhysics>(key);
            reg.emplace<TBullet>(key);
        }
    }
}

static void BM_GAMESCENARIO(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    entt::registry reg;
    buildGameScenario(reg, n);
    PhysicsSystem::registerIn(reg);
    RenderSystem::registerIn(reg);
    HealthSystem::registerIn(reg);

    PhysicsSystem physys{};
    RenderSystem  rendersys{};
    HealthSystem  healthsys{};

    // Perform benchmark
    for (auto _ : state)
    {
        physys.update(reg);
        healthsys.update(reg);
        rendersys.update(reg);
    }
    // Entities in the world updated per second (one frame per iteration)
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_GAMESCENARIO)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

/*
    EnTT only: the iteration tests with views instead of groups. A view keeps no cache (nothing to update
    on structural changes) and is how EnTT is usually used, so it is the other half of the comparison.
*/
static void BM_ITERATESINGLECMP_VIEW(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    entt::registry reg;
    RenderSystem rendersys{};
    for(int64_t i{}; i < n; ++i)
    {
        const entt::entity key = reg.create();
        reg.emplace<CRender>(key);
    }

    // Perform benchmark
    for (auto _ : state)
    {
        rendersys.updateView(reg);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ITERATESINGLECMP_VIEW)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

static void BM_ITERATETWOCMPS_VIEW(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    entt::registry reg;
    PhysicsSystem physys{};
    for(int64_t i{}; i < n; ++i)
    {
        const entt::entity key = reg.create();
        reg.emplace<CPhysics>(key);
        reg.emplace<CTransform>(key);
    }

    // Perform benchmark
    for (auto _ : state)
    {
        physys.updateView(reg);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_ITERATETWOCMPS_VIEW)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

static void BM_ITERATETWOCMPSHALF_VIEW(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    entt::registry reg;
    PhysicsSystem physys{};
    for(int64_t i{}; i < n; ++i)
    {
        const entt::entity key = reg.create();
        reg.emplace<CPhysics>(key);
        if(i % 2)
        {
            reg.emplace<CTransform>(key);
        }
    }

    // Perform benchmark
    for (auto _ : state)
    {
        physys.updateView(reg);
    }
    // Only half of the entities match the system signature
    state.SetItemsProcessed(state.iterations() * (state.range(0) / 2));
}
BENCHMARK(BM_ITERATETWOCMPSHALF_VIEW)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

static void BM_GAMESCENARIO_VIEW(benchmark::State& state)
{
    // Set World
    const int64_t  n = state.range(0);
    entt::registry reg;
    buildGameScenario(reg, n);

    PhysicsSystem physys{};
    RenderSystem  rendersys{};
    HealthSystem  healthsys{};

    // Perform benchmark
    for (auto _ : state)
    {
        physys.updateView(reg);
        healthsys.updateView(reg);
        rendersys.updateView(reg);
    }
    // Entities in the world updated per second (one frame per iteration)
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_GAMESCENARIO_VIEW)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

BENCHMARK_MAIN();
