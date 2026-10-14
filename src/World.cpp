#include "headers/World.h"


#include <cmath>

void World::Update(float playerX, float playerZ){
    int CenterX = static_cast<int> (std::floor(playerX/Chunk::SIZE));
    int CenterZ = static_cast <int> (std::floor(playerZ/Chunk::SIZE));

    for(int x = CenterX - RenderDistance; x<= CenterX+RenderDistance; ++x){
        for(int z = CenterZ - RenderDistance; z<= CenterZ+RenderDistance; ++z){
            LoadChunk(x, z);
        }
    }

    UnloadDistantChunks(CenterX, CenterZ);
};

void World::LoadChunk(int chunkX, int chunkZ)
{
    auto key = std::make_pair(chunkX, chunkZ);

    if (chunks.contains(key))
        return;

    std::cout << "Loading chunk: "
              << chunkX << ", " << chunkZ << '\n';

    chunks[key] = std::make_unique<Chunk>(chunkX, chunkZ);

    std::cout << "Chunks after insertion: "
              << chunks.size() << '\n';

    ReconnectNeighbors(chunkX, chunkZ);
}

void World::UnloadDistantChunks(int CenterX, int CenterZ){
    bool removedAny = false;
    for(auto it = chunks.begin(); it!=chunks.end();){
        int x = it->first.first;
        int z = it->first.second;

        if (std::abs(x - CenterX) > RenderDistance ||
    std::abs(z - CenterZ) > RenderDistance){
            it = chunks.erase(it);
            removedAny = true;
        }else{
            ++it;
        }
    }
    if (removedAny)
    {
        for (auto& [coords, chunk] : chunks)
        {
            ReconnectNeighbors(coords.first, coords.second);
        }
    }
}

void World::ReconnectNeighbors(int chunkX, int chunkZ)
{
    auto getChunk = [&](int x, int z) -> Chunk*
    {
        auto it = chunks.find({x, z});

        if (it == chunks.end())
            return nullptr;

        return it->second.get();
    };

    // Reconnect the new chunk and its four neighbors.
    const int dx[] = { 0, -1, 1, 0, 0 };
    const int dz[] = { 0,  0, 0, 1, -1 };

    for (int i = 0; i < 5; ++i)
    {
        int x = chunkX + dx[i];
        int z = chunkZ + dz[i];

        Chunk* current = getChunk(x, z);

        if (!current)
            continue;

        current->SetNeighbors(
            getChunk(x - 1, z),
            getChunk(x + 1, z),
            getChunk(x, z + 1),
            getChunk(x, z - 1) 
        );
    }
}

void World::Draw(ShaderProgram& shader)
{
    for (auto& [coords, chunk] : chunks)
    {
        glm::mat4 model(1.0f);

        model = glm::translate(
            model,
            glm::vec3(
                coords.first * Chunk::SIZE,
                0.0f,
                coords.second * Chunk::SIZE
            )
        );

        glUniformMatrix4fv(
            glGetUniformLocation(shader.ID, "model" ),
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        chunk->Draw();
    }
}