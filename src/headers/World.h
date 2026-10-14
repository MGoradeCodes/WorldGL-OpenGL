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
    private:
        static constexpr int RenderDistance = 3;
        std::map<std::pair<int, int>, std::unique_ptr<Chunk>> chunks;
        void LoadChunk(int chunkX, int chunkZ);
        void UnloadDistantChunks(int centerX, int centerZ);
        void ReconnectNeighbors(int chunkX, int chunkZ);

};