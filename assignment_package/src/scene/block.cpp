#include "block.h"
#include <iostream>
Blocks::Blocks(OpenGLContext* context)
    :InterleavedDrawable(context),m_tempIdx(nullptr),m_tempData(nullptr)
{}
void Blocks::createVBOdata(){
    createInterleavedVBOdata();
}

void Blocks::passChunkData(const std::vector<GLuint>* idx,const std::vector<float>* data){
    m_tempData = data;
    m_tempIdx = idx;
}

void Blocks::createInterleavedVBOdata(){
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
