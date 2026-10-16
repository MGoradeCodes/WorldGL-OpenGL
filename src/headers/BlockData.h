#pragma once
#include <vector>
#include <map>
#include <string>
#include <array>

using std::vector;
using std::map;
using std::string;
using std::array;
#include "glm/glm.hpp"
#include <optional>


enum class BlockType{
    dirt, air, grass
};

struct RaycastHit
{
    glm::ivec3 blockPosition;    
    glm::ivec3 previousPosition; 
    glm::ivec3 faceNormal;       
};

enum class Faces{
    front, back, left, right, top, bottom
};

class Block{
public:
    static inline const map<Faces, vector<array<float, 3>>> FaceData = {
        {
            Faces::front, {
                {0.0f, 0.0f, 1.0f},
                {1.0f, 0.0f, 1.0f},
                {1.0f, 1.0f, 1.0f},
                {0.0f, 1.0f, 1.0f}
            }
        },
        {
            Faces::back, {
                {1.0f, 0.0f, 0.0f},
                {0.0f, 0.0f, 0.0f},
                {0.0f, 1.0f, 0.0f},
                {1.0f, 1.0f, 0.0f}
            }
        },
        {
            Faces::left, {
                {0.0f, 0.0f, 0.0f},
                {0.0f, 0.0f, 1.0f},
                {0.0f, 1.0f, 1.0f},
                {0.0f, 1.0f, 0.0f}
            }
        },
        {
            Faces::right, {
                {1.0f, 0.0f, 1.0f},
                {1.0f, 0.0f, 0.0f},
                {1.0f, 1.0f, 0.0f},
                {1.0f, 1.0f, 1.0f}
            }
        },
        {
            Faces::top, {
                {0.0f, 1.0f, 1.0f},
                {1.0f, 1.0f, 1.0f},
                {1.0f, 1.0f, 0.0f},
                {0.0f, 1.0f, 0.0f}
            }
        },
        {
            Faces::bottom, {
                {0.0f, 0.0f, 0.0f},
                {1.0f, 0.0f, 0.0f},
                {1.0f, 0.0f, 1.0f},
                {0.0f, 0.0f, 1.0f}
            }
        }
    };

    static int GetTileIndex(BlockType type, Faces face)
    {
        switch (type)
        {
        case BlockType::dirt:
            return 0;
        case BlockType::grass:
            if (face == Faces::top)    return 1;
            if (face == Faces::bottom) return 0;
            return 2;
        default:
            return 0;
        }
    };


};


