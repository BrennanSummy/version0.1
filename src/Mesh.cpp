#include "Mesh.h"
#include <iostream>

Mesh::Mesh(): m_vbo(QOpenGLBuffer::VertexBuffer), m_ebo(QOpenGLBuffer::IndexBuffer){}

Mesh::~Mesh(){destroy();}

void Mesh::init(const std::vector<float>& vertData, const std::vector<unsigned int>& indices, QOpenGLShaderProgram* shader)
{
    initializeOpenGLFunctions();
    // Option to render lines instead of faces
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    loadVertData(vertData);

    // Vertex Array Object
    m_vao.create();
    m_vao.bind();

    // Vertex Buffer Object
    m_vbo.create();
    m_vbo.bind();
    // Set the GL usage of this VBO
    m_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    // Allocate memory on GPU
    m_vbo.allocate(vertData.data(),vertData.size()*sizeof(float));

    // Element Buffer Object
    m_ebo.create();
    m_ebo.bind();
    // Set the GL usage of this EBO
    m_ebo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    // Allocate memory on GPU
    m_ebo.allocate(indices.data(),indices.size()*sizeof(unsigned int));

    // Tell the shader how to parse the data
    // Vertex positions
    shader->enableAttributeArray(0);
    shader->setAttributeBuffer(0,GL_FLOAT,0,3,5*sizeof(float));

    // Texture
    shader->enableAttributeArray(1);
    shader->setAttributeBuffer(1,GL_FLOAT,3*sizeof(float),2,5*sizeof(float));

    // Unbind everything, starting with the VAO to make sure it contains the EBO
    m_vao.release();
    m_vbo.release();
    m_ebo.release();
    //std::cout << "mesh init complete" << std::endl;
}

void Mesh::updateVertData(const std::vector<float>& newVerts)
{
    m_vbo.bind();
    // Only allocate more space if needed
    if(newVerts.size() * sizeof(float) <= m_vbo.size())
    {
        m_vbo.write(0,newVerts.data(),newVerts.size()*sizeof(float));
    }
    else
    {
        m_vbo.allocate(newVerts.data(),newVerts.size()*sizeof(float));
    }

    m_vbo.release();
}

void Mesh::additiveUpdateVertData(Position delta)
{
    for(int i = 0; i<m_currentVertData.size(); i=i+5)
    {
        m_bufferVertData[i]    = m_currentVertData[i]  + delta.x;
        m_bufferVertData[i+1]  = m_currentVertData[i+1]+ delta.y;
    }
    updateVertData(m_bufferVertData);
}

void Mesh::updateIndexData(const std::vector<unsigned int>& newIndices)
{   
    m_ebo.bind();
    // Only allocate more space if needed
    if(newIndices.size() * sizeof(unsigned int) <= m_ebo.size())
    {
        m_ebo.write(0,newIndices.data(),newIndices.size()*sizeof(unsigned int));
    }
    else
    {
        m_ebo.allocate(newIndices.data(),newIndices.size()*sizeof(unsigned int));
    }

    m_ebo.release();

}

void Mesh::draw(GLenum mode, int indexCount)
{
    m_vao.bind();
    glDrawElements(mode,indexCount,GL_UNSIGNED_INT,nullptr);
    m_vao.release();
}

void Mesh::destroy()
{
    if (m_vbo.isCreated()) {m_vbo.destroy();}
    if (m_ebo.isCreated()) {m_ebo.destroy();}
    if (m_vao.isCreated()) {m_vao.destroy();}
}

void Mesh::loadVertData(const std::vector<float> inputVector)
{
    for(int i = 0; i<inputVector.size(); i++)
    {
        m_currentVertData.push_back(inputVector[i]);
        m_bufferVertData.push_back(inputVector[i]);
    }
}

void Mesh::loadBufferToVert()
{
    for(int i = 0; i<m_currentVertData.size(); i++)
    {
        m_currentVertData[i] = m_bufferVertData[i];
    }
}

std::vector<Position> Mesh::readCurrentVerts()
{
    std::vector<Position> verts;
    for(int i = 0; i<m_currentVertData.size(); i=i+5)
    {
        Position pos = {m_currentVertData[i],m_currentVertData[i+1]};
        verts.push_back(pos);
    }
    return verts;
}

void Mesh::writeCurrentVertsWithPositions(std::vector<Position> inVerts)
{
    int vecIndex = 0;
    for(int i = 0; i<m_currentVertData.size(); i=i+5)
    {
        m_currentVertData[i]    = inVerts[vecIndex].x;
        m_currentVertData[i+1]  = inVerts[vecIndex].y;
        vecIndex++;
    }
    updateVertData(m_currentVertData);
}