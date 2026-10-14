#pragma once
#include <vector>
#include "Mesh.h"
#include "BlockData.h"
#include "array"
#include <cmath>
#include "FastNoiseLite.h"

using std::vector;
using std::array;
using std::sin;
using std::cos;

class Chunk{
    public:
        static constexpr int SIZE = 16;
        int ChunkX;
        int ChunkZ;
        Chunk(int ChunkX, int ChunkZ);
        ~Chunk();

        void GenerateMesh();
        void Draw();
        void SetNeighbors(
            Chunk* left,
            Chunk* right,
            Chunk* front,
            Chunk* back
        );
        BlockType GetBlock(int x, int y, int z) const;

        BlockType GetBlockForMeshing(
            int x, int y, int z
        ) const;
        FastNoiseLite terrainNoise;
        int GetTerrainHeight(int x, int z) const;

    private:
        vector<BlockType> blocks;
        Mesh* mesh = nullptr;

        int GetIndex(int x, int y, int z) const;
        Chunk* leftNeighbor  = nullptr;
        Chunk* rightNeighbor = nullptr;
        Chunk* backNeighbor  = nullptr;
        Chunk* frontNeighbor = nullptr;

};