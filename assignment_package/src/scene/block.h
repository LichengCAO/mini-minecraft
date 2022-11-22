#pragma once
#include "drawable.h"
#include <glm_includes.h>
// The six cardinal directions in 3D space
#define BLK_UV 0.0625f

enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG
};

enum BlockType : unsigned char
{
    EMPTY, GRASS, DIRT, STONE, WATER, LAVA, SNOW, BEDROCK, SAND
};

struct VertexData{
    glm::vec4 pos;
    glm::vec2 uv;
};

struct BlockFace{
    Direction dir;
    std::array<VertexData,4> verts;

    BlockFace(Direction d,const VertexData &ll,const VertexData &lr,const VertexData &ur, const VertexData &ul)
        :dir(d),verts{ll,lr,ur,ul}
    {}
};

const std::array<BlockFace,6> adjacentFaces{
            BlockFace(XPOS, VertexData{glm::vec4(1,0,1,1),glm::vec2(0,0)},
                            VertexData{glm::vec4(1,0,0,1),glm::vec2(BLK_UV,0)},
                            VertexData{glm::vec4(1,1,0,1),glm::vec2(BLK_UV,BLK_UV)},
                            VertexData{glm::vec4(1,1,1,1),glm::vec2(0,BLK_UV)}
                      ),
            BlockFace(XNEG, VertexData{glm::vec4(0,0,0,1),glm::vec2(0,0)},
                            VertexData{glm::vec4(0,0,1,1),glm::vec2(BLK_UV,0)},
                            VertexData{glm::vec4(0,1,1,1),glm::vec2(BLK_UV,BLK_UV)},
                            VertexData{glm::vec4(0,1,0,1),glm::vec2(0,BLK_UV)}
                      ),
            BlockFace(YPOS, VertexData{glm::vec4(0,1,1,1),glm::vec2(0,0)},
                            VertexData{glm::vec4(1,1,1,1),glm::vec2(BLK_UV,0)},
                            VertexData{glm::vec4(1,1,0,1),glm::vec2(BLK_UV,BLK_UV)},
                            VertexData{glm::vec4(0,1,0,1),glm::vec2(0,BLK_UV)}
                      ),
            BlockFace(YNEG, VertexData{glm::vec4(0,0,0,1),glm::vec2(0,0)},
                            VertexData{glm::vec4(1,0,0,1),glm::vec2(BLK_UV,0)},
                            VertexData{glm::vec4(1,0,1,1),glm::vec2(BLK_UV,BLK_UV)},
                            VertexData{glm::vec4(0,0,1,1),glm::vec2(0,BLK_UV)}
                      ),
            BlockFace(ZPOS, VertexData{glm::vec4(0,0,1,1),glm::vec2(0,0)},
                            VertexData{glm::vec4(1,0,1,1),glm::vec2(BLK_UV,0)},
                            VertexData{glm::vec4(1,1,1,1),glm::vec2(BLK_UV,BLK_UV)},
                            VertexData{glm::vec4(0,1,1,1),glm::vec2(0,BLK_UV)}
                      ),
            BlockFace(ZNEG, VertexData{glm::vec4(1,0,0,1),glm::vec2(0,0)},
                            VertexData{glm::vec4(0,0,0,1),glm::vec2(BLK_UV,0)},
                            VertexData{glm::vec4(0,1,0,1),glm::vec2(BLK_UV,BLK_UV)},
                            VertexData{glm::vec4(1,1,0,1),glm::vec2(0,BLK_UV)}
                      )
};

class Blocks:public InterleavedDrawable{
private:
    const std::vector<GLuint>* m_tempIdx;    //use to hold data for createVBOdata temperally
    const std::vector<float>* m_tempData;//use to hold data for createVBOdata temperally
public:
    Blocks(OpenGLContext* mp_context);
    void passChunkData(const std::vector<GLuint>* idx,const std::vector<float>* data);
    virtual void createVBOdata()override;
    virtual void createInterleavedVBOdata()override;
};

glm::vec2 getBlockUV(BlockType t,const Direction d);
