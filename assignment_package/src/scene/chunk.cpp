#include "chunk.h"


Chunk::Chunk(OpenGLContext* context, int x , int z)
    : m_blocks(),
      m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}},
      m_offsetXZ(glm::vec2(x,z)),
      m_opaquePart(context),m_transparentPart(context),
      opaquePart(m_opaquePart),transparentPart(m_transparentPart)
{
    std::fill_n(m_blocks.begin(), 65536, EMPTY);
}

void Chunk::generateFace(BlockFace f, int x,int y,int z,
                         std::vector<GLuint>& id,std::vector<float>& data){
    glm::vec4 currOffset(m_offsetXZ.x+x,y,m_offsetXZ.y+z,0);
    BlockType currType = getBlockAt(x,y,z);
    int lenPerData = 6;//pos,uv
    int vcount = data.size()/lenPerData;
    for (auto& v: f.verts) {
        glm::vec4 pos = v.pos + currOffset;
        glm::vec2 uv = v.uv + getBlockUV(currType,f.dir);
        data.push_back(pos[0]);
        data.push_back(pos[1]);
        data.push_back(pos[2]);
        data.push_back(pos[3]);
        data.push_back(uv.x);
        data.push_back(uv.y);
    }

    id.push_back(vcount);
    id.push_back(vcount+1);
    id.push_back(vcount+2);
    id.push_back(vcount);
    id.push_back(vcount+2);
    id.push_back(vcount+3);
}

// Does bounds checking with at()
BlockType Chunk::getBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
    return m_blocks.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
BlockType Chunk::getBlockAt(int x, int y, int z) const {
    return getBlockAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(z));
}

BlockType Chunk::getBlockAt(glm::vec3 coord)const{
    int x = glm::floor(coord.x);
    int y = glm::floor(coord.y);
    int z = glm::floor(coord.z);
    return getBlockAt(x,y,z);
}
// Does bounds checking with at()
void Chunk::setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t) {
    m_blocks.at(x + 16 * y + 16 * 256 * z) = t;
}

const static std::unordered_map<Direction, Direction, EnumHash> oppositeDirection {
    {XPOS, XNEG},
    {XNEG, XPOS},
    {YPOS, YNEG},
    {YNEG, YPOS},
    {ZPOS, ZNEG},
    {ZNEG, ZPOS}
};

void Chunk::linkNeighbor(uPtr<Chunk> &neighbor, Direction dir) {
    if(neighbor != nullptr) {
        this->m_neighbors[dir] = neighbor.get();
        neighbor->m_neighbors[oppositeDirection.at(dir)] = this;
    }
}

void Chunk::createBlocksVBOdata(){
    std::vector<GLuint> opaqueId;
    std::vector<GLuint> transparentId;
    std::vector<float> opaqueData;
    std::vector<float> transparentData;

    std::vector<GLuint>* idx = nullptr;
    std::vector<float>* data = nullptr;

    for(int x=0;x<16;x++){
        for(int y=0;y<256;y++){
            for(int z=0;z<16;z++){
                BlockType curr = getBlockAt(x,y,z);
                //to push data to different chunkblocks
                idx = isOpaque(curr)?&opaqueId:&transparentId;
                data = isOpaque(curr)?&opaqueData:&transparentData;
                for(BlockFace f: adjacentFaces){
                    glm::vec3 coord = glm::vec3(x,y,z) + f.offset;
                    //to check if the face need draw
                    bool draw = false;
                    if(coord.x<0){
                        Chunk* neighbor = m_neighbors[XNEG];
                        if(neighbor==nullptr){
                            draw=false;
                        }else{
                            draw = shouldDraw(curr,neighbor->getBlockAt(15,y,z));
                        }
                    }else if(coord.x>=16){
                        Chunk* neighbor = m_neighbors[XPOS];
                        if(neighbor==nullptr){
                            draw=false;
                        }else{
                            draw = shouldDraw(curr,neighbor->getBlockAt(0,y,z));
                        }
                    }else if(coord.z<0){
                        Chunk* neighbor = m_neighbors[ZNEG];
                        if(neighbor==nullptr){
                            draw=false;
                        }else{
                            draw = shouldDraw(curr,neighbor->getBlockAt(x,y,15));
                        }
                    }else if(coord.z >= 16){
                        Chunk* neighbor = m_neighbors[ZPOS];
                        if(neighbor==nullptr){
                            draw=false;
                        }else{
                            draw = shouldDraw(curr,neighbor->getBlockAt(x,y,0));
                        }
                    }else if(coord.y<0){
                        draw = false;
                    }else if(coord.y>=256){
                        draw = false;
                    }else{
                        draw = shouldDraw(curr,getBlockAt(coord));
                    }
                    //draw if neccessary
                    if(draw){
                        generateFace(f,x,y,z,*idx,*data);
                    }
                }
            }
        }
    }

    m_opaquePart.passChunkData(&opaqueId,&opaqueData);
    m_transparentPart.passChunkData(&transparentId,&transparentData);
    m_opaquePart.createInterleavedVBOdata();
    m_transparentPart.createInterleavedVBOdata();
}
