#include <iostream>

#include <swelter.hpp>
#include <log.hpp>
#include <format>
#include <array>
#include <algorithm>
#include <cstdint>


void seetype(auto)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

struct TPlayer {};
struct NonMove
{
    NonMove(NonMove&& other) = delete;
};
struct CTransform
{
    float x;
    float y;
};

int main() {

    logInit();

    // LIFO reuse: a returned chunk is the next one handed out
    Swelter::Chunk* first = Swelter::ChunkPool::allocate();
    Swelter::ChunkPool::deallocate(first);
    Swelter::Chunk* reused = Swelter::ChunkPool::allocate();
    debugAssertion(first == reused);
    debugAssertion(Swelter::ChunkPool::ownedCount() == 1);
    debugAssertion(Swelter::ChunkPool::freeCount() == 0);
    Swelter::ChunkPool::deallocate(reused);

    // Alignment and no overlap between any pair of chunks
    std::array<Swelter::Chunk*, 5> chunks;
    for(auto& chunk : chunks)
    {
        chunk = Swelter::ChunkPool::allocate();
    }

    for(std::size_t i{}; i < chunks.size(); ++i)
    {
        const auto a = reinterpret_cast<std::uintptr_t>(chunks[i]);
        debugAssertion(a % Swelter::COLUMN_ALIGN == 0);

        for(std::size_t j = i + 1; j < chunks.size(); ++j)
        {
            const auto b = reinterpret_cast<std::uintptr_t>(chunks[j]);
            debugAssertion(std::max(a, b) - std::min(a, b) >= Swelter::CHUNK_SIZE);
        }
    }

    // Counters: the first chunk was reused, so only 5 were ever allocated
    debugAssertion(Swelter::ChunkPool::ownedCount() == chunks.size());
    debugAssertion(Swelter::ChunkPool::freeCount() == 0);

    for(std::size_t i{}; i < 4; ++i)
    {
        Swelter::ChunkPool::deallocate(chunks[i]);
    }
    debugAssertion(Swelter::ChunkPool::ownedCount() == 5);
    debugAssertion(Swelter::ChunkPool::freeCount() == 4);

    // Allocating again must reuse a free chunk, not allocate a new one
    chunks[0] = Swelter::ChunkPool::allocate();
    debugAssertion(Swelter::ChunkPool::ownedCount() == 5);
    debugAssertion(Swelter::ChunkPool::freeCount() == 3);

    return 0;
}
