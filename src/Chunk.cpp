#include "headers/Chunk.h"
#include <cmath>


//Constants 
namespace
{
    constexpr int   ATLAS_COLS = 4;           
    constexpr int   ATLAS_ROWS = 1;
    constexpr float TILE_W = 1.0f / ATLAS_COLS;
    constexpr float TILE_H = 1.0f / ATLAS_ROWS;
}

//Chunk Constructor and initialise vars
Chunk::Chunk(int ChunkX, int ChunkZ) : ChunkX(ChunkX) , ChunkZ(ChunkZ), blocks(SIZE * SIZE * SIZE, BlockType::air){
    // Set Noise Func    
    terrainNoise.SetSeed(2000);
    terrainNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    terrainNoise.SetFrequency(0.01f);
    terrainNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);

    // Loop for loading a chunk
    for (int x = 0; x < SIZE; ++x) // Till SIZE(16)
    {   
        for (int z = 0; z < SIZE; ++z) // Til SIZE(16)
        {   
            // Get world coodiantes for a local chunk
            int WorldX = ChunkX*SIZE+x; // ChunkX (multiple of 16 for each chunk)
            int WorldZ = ChunkZ*SIZE+z; // SAME FOR Z
            int height = GetTerrainHeight(WorldX, WorldZ); // Get terrain configs for each chunk
            for (int y = 0; y<=height; y++){

                if(y == height){ // Setup block levels
                    blocks[GetIndex(x, y, z)] = BlockType::grass;
                }else{
                    blocks[GetIndex(x, y, z)] = BlockType::dirt;
                }    
            }
        }
    }

}

//Get Da World Position for each block outside of local chunk coords.
glm::ivec3 Chunk::GetBlockWorldPosition(int localX,int localY,int localZ) const{
    return glm::ivec3(ChunkX * SIZE + localX, localY, ChunkZ * SIZE + localZ);
}

// Chunk Destructor FREE MEM
Chunk::~Chunk()
{
    if (mesh != nullptr)
    {
        mesh->Delete();
        delete mesh;
    }
}

//Get Block Actual Postion of each block
int Chunk::GetIndex(int x, int y, int z) const
{
    return x + SIZE * (y + SIZE * z);
}

//Check if HasBlock?
bool Chunk::HasBlock(int x, int y, int z){
    if (x < 0 || x >= SIZE ||
        y < 0 || y >= SIZE ||
        z < 0 || z >= SIZE)
    {
        return false;
    }
    return blocks[x + SIZE * (y + SIZE * z)] != BlockType::air;
}

//Set Block in the arr
bool Chunk::SetBlock(int x, int y, int z, BlockType type)
{
    if (x < 0 || x >= SIZE ||
        y < 0 || y >= SIZE ||
        z < 0 || z >= SIZE)
    {
        return false;
    }

    blocks[GetIndex(x, y, z)] = type;

    return true;
}


//Getter for Block
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


//Actually Draw Chunks
void Chunk::Draw()
{
    if (mesh != nullptr)
        mesh->Draw();
}

//Generate Mesh combined for all chunks
void Chunk::GenerateMesh()
{
    //Vertices and Indices for mesh loading
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    //Faces arr for BlockData Strcuture
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
    //Loop Through blocks in side a chunk ---> 16 * 16 * 16 = > 16
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
                        // Calculate all vertices for the mesh drawal.
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