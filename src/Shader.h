#pragma once

#include "Texture.h"
#include <QOpenGLShaderProgram>

// The Shader class makes shader programs given paths to the relevant vertex and fragment shader source files.
class Shader : protected QOpenGLFunctions_3_3_Core
{
    private:
        QOpenGLShaderProgram* m_shaderProg;
    public:
        Shader();
        ~Shader();
        void loadShaders(const QString vertexShaderPath, const QString fragShaderPath);
        void bind();
        void release();
        QOpenGLShaderProgram* getShaderProgram();
};