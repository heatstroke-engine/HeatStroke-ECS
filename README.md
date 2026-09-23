# HS-ECS

An Entity Component System written in C++ for the **HeatStroke Engine**, a 2D game engine built from scratch.

> **Status: work in progress.** This repository is the core of my Bachelor's thesis (TFG) at the Universidad de Alicante, started in September 2026 and due in May 2027. The `sw/ecs/basic/1.0` branch contains the first, basic ECS. Archetypes and a job system are the next milestones (see [Roadmap](#roadmap)).

## Why this project

HeatStroke Engine is the foundation for my game *Cry War*. The goal of this repository is to build its ECS step by step, starting from a simple and well-tested design and evolving it towards a data-oriented, cache-friendly and parallel one, measuring each step along the way.

## Design

The first version (`sw/ecs/basic/1.0`) is built around **slotmaps**:

- Entities and components are referenced through **generational handles**, so a stale handle is detected instead of silently pointing at reused storage.
- Storage is **dense**, which keeps iteration cache-friendly.
- Insertion, lookup and removal are **O(1)**.

Everything lives in the `HeatStroke` namespace.

## Roadmap

| Milestone | State |
|---|---|
| Basic ECS with slotmaps (`sw/ecs/basic/1.0`) | In progress |
| Archetype-based storage | Planned |
| Job system for parallel execution | Planned |
| Benchmarks: basic ECS vs. archetype ECS vs. [EnTT](https://github.com/skypjack/entt) | Planned |

## Repository layout

| Path | Content |
|---|---|
| `code/` | ECS sources |
| `sandbox/` | Sandbox application to exercise the ECS |
| `scripts/` | Helper scripts |
| `CMakeLists.txt`, `CMakePresets.json` | Build configuration |

## Building

The project uses **CMake** with presets (see `CMakePresets.json`).

## Usage

<!-- TODO (Rauw)

## Benchmarks

Once the archetype version exists, the plan is to compare three implementations under the same workloads:

1. The basic ECS (slotmaps).
2. The archetype ECS with the job system.
3. [EnTT](https://github.com/skypjack/entt), as an external reference.

<!-- TODO (Rauw): when I have results, add a table here.

## Related

- **HeatStroke Engine**: the 2D engine this ECS belongs to.
- **Cry War**: the game built on top of it.

## Author

Raúl Hidalgo Nicolás · Universidad de Alicante

## License

See [LICENSE](LICENSE).
