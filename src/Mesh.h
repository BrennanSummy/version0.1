#pragma once

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_3_3_Core> // Update version if necessary
#include <QOpenGLShaderProgram>
#include <vector>
struct Position
{
    float x;
    float y;
    public:
    //Position subtract(Position v1, Position v2){Position pos = {{v1.x-v2.x},{v1.y-v2.y}}; return pos;}
    Position add(Position v2){Position pos = {{x+v2.x},{y+v2.y}}; return pos;}
    Position subtract(Position v2){Position pos = {{x-v2.x},{y-v2.y}}; return pos;}
    Position scale(float scalar){Position pos = {scalar * x, scalar * y}; return pos;}
};

class Mesh: protected QOpenGLFunctions_3_3_Core
{
    public:
        Mesh();
        ~Mesh();
        void init(const std::vector<float>& vertData, const std::vector<unsigned int>& indices, QOpenGLShaderProgram* shader);
        // Update VBO
        void updateVertData(const std::vector<float>& newVerts);
        void additiveUpdateVertData(float xToAdd, float yToAdd);
        void loadBufferToVert();
        std::vector<Position> readCurrentVerts();
        void writeCurrentVertsWithPositions(std::vector<Position> verts);
        // Update EBO
        void updateIndexData(const std::vector<unsigned int>& newIndices);
        void draw(GLenum mode, int indexCount);
        void destroy();
    private:
        QOpenGLBuffer m_vbo;
        QOpenGLVertexArrayObject m_vao;
        QOpenGLBuffer m_ebo;
        std::vector<float> m_currentVertData;
        std::vector<float> m_bufferVertData;
        std::vector<unsigned int> m_currentIndexData;
        void loadVertData(const std::vector<float> inputVector);
};