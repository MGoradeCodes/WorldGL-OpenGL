#include "headers/World.h"
#include <cmath>
#include <limits>
#include <algorithm>


// Update Player Chunk Pos FUNC
void World::Update(float playerX, float playerZ){
    //Get player position based on the chunk! : 
    CenterX = static_cast<int> (std::floor(playerX/Chunk::SIZE));
    CenterZ = static_cast <int> (std::floor(playerZ/Chunk::SIZE));
    //Get the player position in the chunk itself
    for(int x = CenterX - RenderDistance; x<= CenterX+RenderDistance; ++x){
        for(int z = CenterZ - RenderDistance; z<= CenterZ+RenderDistance; ++z){
            //Load chunks from -x to x and -z to z
            LoadChunk(x, z);
        }
    }
    // keep unloading the left over distant chunks that can affect the performance
    UnloadDistantChunks(CenterX, CenterZ);
};


bool World::SetBlock(int worldX,int worldY,int worldZ,BlockType type){
    glm::ivec3 ChunkPos = ConvertToChunkCoords(worldX, worldY, worldZ);
    auto key = std::make_pair(ChunkPos.x , ChunkPos.z);
    if(!chunks.contains(key)){
        return false;
    };
    auto it = chunks.find(key);
    Chunk* chunk = it->second.get();
    glm::ivec3 LocalPos =
    ConvertToLocalCoords(worldX, worldY, worldZ);
    bool success = chunk->SetBlock(LocalPos.x, LocalPos.y, LocalPos.z, type);
    if (!success)
        return false;

    chunk->GenerateMesh();

    return true;

};


glm::ivec3 World::ConvertToChunkCoords(int x, int y, int z)
{
    return glm::ivec3(
        static_cast<int>(std::floor(
            static_cast<float>(x) / Chunk::SIZE)),
        y,
        static_cast<int>(std::floor(
            static_cast<float>(z) / Chunk::SIZE))
    );
}
glm::ivec3 World::ConvertToLocalCoords(int x, int y, int z)
{
    int localX = ((x % Chunk::SIZE) + Chunk::SIZE)
                 % Chunk::SIZE;
    int localZ = ((z % Chunk::SIZE) + Chunk::SIZE)
                 % Chunk::SIZE;

    return glm::ivec3(localX, y, localZ);
}
//Chunk Loading Func
void World::LoadChunk(int chunkX, int chunkZ)
{   
    //Get the chunk asked for in the params : 
    auto key = std::make_pair(chunkX, chunkZ);

    //Check if it already exists? (Duplicate prevention, no need to load, already loaded chunks again)
    if (chunks.contains(key))
        return;

    //Once a unique pair is found we have add append it in.
    chunks[key] = std::make_unique<Chunk>(chunkX, chunkZ);
    //Once we found a new chunk attatch it again in reconnect chunks
    ReconnectNeighbors(chunkX, chunkZ);
}


// UnloadDistantChunks FUNC
void World::UnloadDistantChunks(int CenterX, int CenterZ){
    bool removedAny = false;
    // Go through chunks
    for(auto it = chunks.begin(); it!=chunks.end();){
        int x = it->first.first;
        int z = it->first.second;
        

    //check for chunks based on render distance
        if (std::abs(x - CenterX) > RenderDistance ||
    std::abs(z - CenterZ) > RenderDistance){
            it = chunks.erase(it);
            removedAny = true;
        }else{
            ++it;
        }
    }
    //if removed any chunks, further disconnect them to save performance
    if (removedAny)
    {
        for (auto& [coords, chunk] : chunks)
        {
            //Once we have unloaded distant chunks we reconnect neighbours(in this case they go null(nullptr) so no new chunks)
            ReconnectNeighbors(coords.first, coords.second);
        }
    }
}

// RECONNECT NEIGHBOUR FUNC

void World::ReconnectNeighbors(int chunkX, int chunkZ)
{   //Lamba made for getting chunk
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
        //Look for the current chunk
        int x = chunkX + dx[i];
        int z = chunkZ + dz[i];

        //Check if my current chunk exists, and store it as a pointer (nullptr if not exists)
        Chunk* current = getChunk(x, z);
        //If not exist(nullptr) continue (skip)
        if (!current)
            continue;

        //Set Neighbours according to the new neighbours on the found curr chunk
        current->SetNeighbors(
            getChunk(x - 1, z),
            getChunk(x + 1, z),
            getChunk(x, z + 1),
            getChunk(x, z - 1) 
        );
    }
}


//Draw func
void World::Draw(ShaderProgram& shader)
{
    for (auto& [coords, chunk] : chunks)
    {
        glm::mat4 model(1.0f);
        //Model Matrix for the chunks (local coordinates to world coordinates)
        model = glm::translate(
            model,
            glm::vec3(
                coords.first * Chunk::SIZE,
                0.0f,
                coords.second * Chunk::SIZE
            )
        );
        // Matrix Configs
        glUniformMatrix4fv(
            glGetUniformLocation(shader.ID, "model" ),
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );
        // Tell Pointer to call the func
        chunk->Draw();
    }
}

BlockType World::GetBlock(int worldX, int worldY, int worldZ)
{
    if (worldY < 0 || worldY >= Chunk::SIZE)
        return BlockType::air;

    glm::ivec3 chunkPos =
        ConvertToChunkCoords(worldX, worldY, worldZ);

    auto key = std::make_pair(chunkPos.x, chunkPos.z);

    auto it = chunks.find(key);

    if (it == chunks.end())
        return BlockType::air;

    glm::ivec3 localPos =
        ConvertToLocalCoords(worldX, worldY, worldZ);

    return it->second->GetBlock(
        localPos.x,
        localPos.y,
        localPos.z
    );
}

std::optional<RaycastHit> World::Raycast(
    glm::vec3 origin,
    glm::vec3 direction,
    float maxDistance)
{
    if (glm::dot(direction, direction) == 0.0f ||
        maxDistance < 0.0f)
    {
        return std::nullopt;
    }

    direction = glm::normalize(direction);

    // Voxel containing the ray's starting position.
    glm::ivec3 voxel(
        static_cast<int>(std::floor(origin.x)),
        static_cast<int>(std::floor(origin.y)),
        static_cast<int>(std::floor(origin.z))
    );

    // Direction in which each voxel coordinate changes.
    glm::ivec3 step(
        direction.x > 0.0f ? 1 : direction.x < 0.0f ? -1 : 0,
        direction.y > 0.0f ? 1 : direction.y < 0.0f ? -1 : 0,
        direction.z > 0.0f ? 1 : direction.z < 0.0f ? -1 : 0
    );

    const float infinity = std::numeric_limits<float>::infinity();

    // Distance along the ray between successive voxel boundaries.
    glm::vec3 delta(
        step.x != 0 ? std::abs(1.0f / direction.x) : infinity,
        step.y != 0 ? std::abs(1.0f / direction.y) : infinity,
        step.z != 0 ? std::abs(1.0f / direction.z) : infinity
    );

    // Distance along the ray to the first boundary on each axis.
    glm::vec3 side;

    side.x = step.x > 0
        ? (voxel.x + 1.0f - origin.x) * delta.x
        : step.x < 0
            ? (origin.x - voxel.x) * delta.x
            : infinity;

    side.y = step.y > 0
        ? (voxel.y + 1.0f - origin.y) * delta.y
        : step.y < 0
            ? (origin.y - voxel.y) * delta.y
            : infinity;

    side.z = step.z > 0
        ? (voxel.z + 1.0f - origin.z) * delta.z
        : step.z < 0
            ? (origin.z - voxel.z) * delta.z
            : infinity;

    // Previous empty voxel and the face entered.
    glm::ivec3 previous = voxel;
    glm::ivec3 normal(0);

    float distance = 0.0f;

    while (distance <= maxDistance)
    {
        BlockType block = GetBlock(voxel.x, voxel.y, voxel.z);

        if (block != BlockType::air)
        {
            return RaycastHit{
                voxel,
                previous,
                normal
            };
        }

        previous = voxel;

        // Move into whichever voxel boundary comes next.
        if (side.x <= side.y && side.x <= side.z)
        {
            distance = side.x;
            side.x += delta.x;
            voxel.x += step.x;
            normal = glm::ivec3(-step.x, 0, 0);
        }
        else if (side.y <= side.z)
        {
            distance = side.y;
            side.y += delta.y;
            voxel.y += step.y;
            normal = glm::ivec3(0, -step.y, 0);
        }
        else
        {
            distance = side.z;
            side.z += delta.z;
            voxel.z += step.z;
            normal = glm::ivec3(0, 0, -step.z);
        }
    }

    return std::nullopt;
}