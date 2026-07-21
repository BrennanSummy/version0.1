#pragma once

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_3_3_Core> // Update version if necessary
#include <QOpenGLShaderProgram>
#include <vector>

class Mesh: protected QOpenGLFunctions_3_3_Core
{
    public:
        Mesh();
        ~Mesh();
        void init(const std::vector<float>& vertData, const std::vector<unsigned int>& indices, QOpenGLShaderProgram* shader);
        // Update VBO
        void updateVertData(const std::vector<float>& newVerts);
        // Update EBO
        void updateIndexData(const std::vector<unsigned int>& newIndices);
        void draw(GLenum mode, int indexCount);
        void destroy();
    private:
        QOpenGLBuffer m_vbo;
        QOpenGLVertexArrayObject m_vao;
        QOpenGLBuffer m_ebo;
};