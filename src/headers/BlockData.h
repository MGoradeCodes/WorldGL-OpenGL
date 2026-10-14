#pragma once
#include <vector>
#include <map>
#include <string>
#include <array>

using std::vector;
using std::map;
using std::string;
using std::array;


enum class BlockType{
    dirt, air, grass
};

enum class Faces{
    front, back, left, right, top, bottom
};

class Block{
public:
    static inline const map<Faces, vector<array<float, 3>>> FaceData = {
        {
            Faces::front, {
                {-0.5f, -0.5f,  0.5f},
                { 0.5f, -0.5f,  0.5f},
                { 0.5f,  0.5f,  0.5f},
                {-0.5f,  0.5f,  0.5f}
            }
        },
        {
            Faces::back, {
                { 0.5f, -0.5f, -0.5f},
                {-0.5f, -0.5f, -0.5f},
                {-0.5f,  0.5f, -0.5f},
                { 0.5f,  0.5f, -0.5f}
            }
        },
        {
            Faces::left, {
                {-0.5f, -0.5f, -0.5f},
                {-0.5f, -0.5f,  0.5f},
                {-0.5f,  0.5f,  0.5f},
                {-0.5f,  0.5f, -0.5f}
            }
        },
        {
            Faces::right, {
                { 0.5f, -0.5f,  0.5f},
                { 0.5f, -0.5f, -0.5f},
                { 0.5f,  0.5f, -0.5f},
                { 0.5f,  0.5f,  0.5f}
            }
        },
        {
            Faces::top, {
                {-0.5f,  0.5f,  0.5f},
                { 0.5f,  0.5f,  0.5f},
                { 0.5f,  0.5f, -0.5f},
                {-0.5f,  0.5f, -0.5f}
            }
        },
        {
            Faces::bottom, {
                {-0.5f, -0.5f, -0.5f},
                { 0.5f, -0.5f, -0.5f},
                { 0.5f, -0.5f,  0.5f},
                {-0.5f, -0.5f,  0.5f}
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


