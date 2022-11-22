#include "block.h"
#include <iostream>
ChunkBlocks::ChunkBlocks(OpenGLContext* context)
    :InterleavedDrawable(context),m_tempIdx(nullptr),m_tempData(nullptr)
{}
void ChunkBlocks::createVBOdata(){
    createInterleavedVBOdata();
}

void ChunkBlocks::passChunkData(const std::vector<GLuint>* idx,const std::vector<float>* data){
    m_tempData = data;
    m_tempIdx = idx;
}

void ChunkBlocks::createInterleavedVBOdata(){
    if(m_tempIdx==nullptr||m_tempData==nullptr){
        std::cout << "need to call passChunkData before create!"<<std::endl;
        return;
    }
    m_count = m_tempIdx->size();

    generateIdx();
    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_tempIdx->size() * sizeof(GLuint), m_tempIdx->data(), GL_STATIC_DRAW);

    generateInterleaved();
    bindInterleaved();
    mp_context->glBufferData(GL_ARRAY_BUFFER, m_tempData->size() * sizeof(float), m_tempData->data(), GL_STATIC_DRAW);

    m_tempIdx = nullptr;
    m_tempData = nullptr;
}

glm::vec2 getBlockUV(BlockType t,const Direction d){
      glm::vec2 res(0,0);
      if(t != EMPTY) {
          switch(t) {
          case GRASS:
              if(d==YPOS){
                  res = glm::vec2(8,13);
              }else if(d==YNEG){
                  res = glm::vec2(2,15);
              }else{
                  res = glm::vec2(3,15);
              }
              break;
          case DIRT:
              res = glm::vec2(2,15);
              break;
          case STONE:
              res = glm::vec2(1,15);
              break;
          case WATER:
              res = glm::vec2(13,3);
              break;
          case LAVA:
              res = glm::vec2(13,1);
              break;
          case BEDROCK:
              res = glm::vec2(1,14);
              break;
          case SNOW:
              res = glm::vec2(2,11);
              //res = glm::vec2(1,12);
              break;
          default:
              // Other block types are not yet handled, so we default to debug purple
              res = glm::vec2(7,1);
              break;
          }
     }
     return res*BLK_UV;
}

bool isBreakable(BlockType type){
    switch(type){
    case EMPTY:
    case LAVA:
    case BEDROCK:
    case WATER:
        return false;
    default:
        return true;
    }
}

bool isOpaque(BlockType type){
    switch(type){
    case EMPTY:
    case WATER:
        return false;
    default:
        return true;
    }
}

bool shouldDraw(BlockType curr, BlockType adjacent){
    if(curr==EMPTY)return false;
    if(isOpaque(curr)){
        switch(adjacent){
        case EMPTY:
        case WATER:
        case LAVA:
            return true;
        default:
            return false;
        }
    }else{
        return curr!=adjacent;
    }
}
