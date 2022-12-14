#pragma once
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include <array>
#include <unordered_map>
#include <cstddef>
#include "block.h"

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.
// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

// One Chunk is a 16 x 256 x 16 section of the world,
// containing all the Minecraft blocks in that area.
// We divide the world into Chunks in order to make
// recomputing its VBO data faster by not having to
// render all the world at once, while also not having
// to render the world block by block.

class Chunk {
private:  
    // All of the blocks contained within this Chunk
    std::array<BlockType, 65536> m_blocks;
    // This Chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;
    const glm::vec2 m_offsetXZ;
    ChunkBlocks m_opaquePart;
    ChunkBlocks m_transparentPart;
    //helper functions
    void generateFace(BlockFace f,int x,int y,int z,
                      std::vector<GLuint>& id,std::vector<float>& data);
public:
    const ChunkBlocks& opaquePart;
    const ChunkBlocks& transparentPart;

    Chunk(OpenGLContext* context, int x, int z);
    BlockType getBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getBlockAt(int x, int y, int z) const;
    BlockType getBlockAt(glm::vec3)const;
    void setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);
    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);
    void createBlocksVBOdata();
};
