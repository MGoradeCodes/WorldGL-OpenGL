#pragma once
#include "Chunk.h"
#include "map"
#include "utility"
#include "memory"
#include "ShaderClass.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include <iostream>

class World{
    public:
        void Update(float playerX, float playerZ);
        void Draw(ShaderProgram& shader);
        bool SetBlock(
            int worldX,
            int worldY,
            int worldZ,
            BlockType type
        );
        void LoadChunk(int chunkX, int chunkZ);
        glm::ivec3 ConvertToChunkCoords(int x, int y, int z);
        int CenterX , CenterZ;
        std::optional<RaycastHit> Raycast(glm::vec3 origin,glm::vec3 direction,float maxDistance);

    private:
        static constexpr int RenderDistance = 3;
        std::map<std::pair<int, int>, std::unique_ptr<Chunk>> chunks;
        void UnloadDistantChunks(int centerX, int centerZ);
        void ReconnectNeighbors(int chunkX, int chunkZ);
        BlockType GetBlock(int worldX, int worldY, int worldZ);
        glm::ivec3 ConvertToLocalCoords(int x, int y, int z);

};