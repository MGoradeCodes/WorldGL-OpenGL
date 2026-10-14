#include "headers/Chunk.h"
#include <cmath>

namespace
{
    constexpr int   ATLAS_COLS = 4;           
    constexpr int   ATLAS_ROWS = 1;
    constexpr float TILE_W = 1.0f / ATLAS_COLS;
    constexpr float TILE_H = 1.0f / ATLAS_ROWS;
}
int GetTerrainHeight();

Chunk::Chunk(int ChunkX, int ChunkZ) : ChunkX(ChunkX) , ChunkZ(ChunkZ), blocks(SIZE * SIZE * SIZE, BlockType::air){
    terrainNoise.SetSeed(2000);
    terrainNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    terrainNoise.SetFrequency(0.01f);
    terrainNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);

    for (int x = 0; x < SIZE; ++x)
    {   
        for (int z = 0; z < SIZE; ++z)
        {   
            int WorldX = ChunkX*SIZE+x;
            int WorldZ = ChunkZ*SIZE+z;
            int height = GetTerrainHeight(WorldX, WorldZ);
            for (int y = 0; y<=height; y++){

                if(y == height){
                    blocks[GetIndex(x, y, z)] = BlockType::grass;
                }else{
                blocks[GetIndex(x, y, z)] = BlockType::dirt;
                }    
            }
        }
    }

}

Chunk::~Chunk()
{
    if (mesh != nullptr)
    {
        mesh->Delete();
        delete mesh;
    }
}

int Chunk::GetIndex(int x, int y, int z) const
{
    return x + SIZE * (y + SIZE * z);
}

BlockType Chunk::GetBlock(int x, int y, int z) const
{
    if (x < 0 || x >= SIZE ||
        y < 0 || y >= SIZE ||
        z < 0 || z >= SIZE)
    {
        return BlockType::air;
    }

    return blocks[GetIndex(x, y, z)];
}

void Chunk::Draw()
{
    if (mesh != nullptr)
        mesh->Draw();
}


void Chunk::GenerateMesh()
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const std::array<Faces, 6> faces = {
        Faces::front, Faces::back,
        Faces::left, Faces::right,
        Faces::top, Faces::bottom
    };

    const std::array<std::array<int, 3>, 6> directions = {{
        { 0,  0,  1},
        { 0,  0, -1},
        {-1,  0,  0},
        { 1,  0,  0},
        { 0,  1,  0},
        { 0, -1,  0} 
    }};

    const std::array<std::array<float, 2>, 4> uv = {{
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f}
    }};

    for (int x = 0; x < SIZE; ++x)
    {
        for (int y = 0; y < SIZE; ++y)
        {
            for (int z = 0; z < SIZE; ++z)
            {
                const BlockType type = GetBlock(x, y, z);
                if (type == BlockType::air)
                    continue;
                for (std::size_t f = 0; f < faces.size(); ++f)
                {
                    const auto& d = directions[f];

                    if (GetBlockForMeshing(
                            x + d[0],
                            y + d[1],
                            z + d[2]) != BlockType::air)
                    {
                        continue;
                    }

                    const vector<array<float, 3>>& face = Block::FaceData.at(faces[f]);

                    const unsigned int base =
                        static_cast<unsigned int>(vertices.size() / 5);
                
                    const int tile = Block::GetTileIndex(type, faces[f]);
                    const float u0   = (tile % ATLAS_COLS) * TILE_W;
                    const float v0   = (tile / ATLAS_COLS) * TILE_H;

                    for (std::size_t v = 0; v < 4; ++v)
                    {
                        vertices.push_back(face[v][0] + x);
                        vertices.push_back(face[v][1] + y);
                        vertices.push_back(face[v][2] + z);

                        vertices.push_back(u0 + uv[v][0] * TILE_W);
                        vertices.push_back(v0 + uv[v][1] * TILE_H); 
                    }

                    indices.insert(indices.end(), {
                        base, base + 1, base + 2,
                        base, base + 2, base + 3
                    });
                }
            }
        }
    }

    if (mesh != nullptr)
    {
        mesh->Delete();
        delete mesh;
        mesh = nullptr;
    }

    if (!indices.empty())
        mesh = new Mesh(vertices, indices);
}


int Chunk::GetTerrainHeight(int x, int z) const{
    float noiseVal = (terrainNoise.GetNoise(static_cast<float>(x), static_cast<float>(z)) + 1.0f) * 0.5f;
    float mixedShape = std::pow(noiseVal, 3.5f); 
    float basePlains = SIZE * 0.2f;
    float maxMountainHeight = SIZE * 0.75f;

    return static_cast<int>(std::round(basePlains + (mixedShape * maxMountainHeight)));
};

void Chunk::SetNeighbors(Chunk* left,Chunk* right,Chunk* front,Chunk* back){
    leftNeighbor = left;
    rightNeighbor = right;
    frontNeighbor = front;
    backNeighbor = back;
    GenerateMesh();
    
}



BlockType Chunk::GetBlockForMeshing(int x, int y, int z) const
{
    if(y<0 || y>=SIZE){
        return BlockType::air;
    }

    if (x >= SIZE)
        return rightNeighbor
            ? rightNeighbor->GetBlock(0, y, z)
            : BlockType::air;

    if (x < 0)
        return leftNeighbor
            ? leftNeighbor->GetBlock(SIZE - 1, y, z)
            : BlockType::air;

    if (z >= SIZE)
        return frontNeighbor
            ? frontNeighbor->GetBlock(x, y, 0)
            : BlockType::air;

    if (z < 0)
        return backNeighbor
            ? backNeighbor->GetBlock(x, y, SIZE - 1)
            : BlockType::air;

    return GetBlock(x, y, z);
}