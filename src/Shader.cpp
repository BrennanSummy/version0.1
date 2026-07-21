#include "Shader.h"

Shader::Shader()
{
    initializeOpenGLFunctions();
    m_shaderProg = new QOpenGLShaderProgram();
}
Shader::~Shader()
{
    delete m_shaderProg;
}

void Shader::loadShaders(const QString vertexShaderPath, const QString fragShaderPath)
{
    m_shaderProg->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderPath);
    m_shaderProg->addShaderFromSourceFile(QOpenGLShader::Fragment, fragShaderPath);
    m_shaderProg->link();
    m_shaderProg->bind();
}

QOpenGLShaderProgram* Shader::getShaderProgram()
{return m_shaderProg;}

void Shader::bind(){m_shaderProg->bind();}
void Shader::release(){m_shaderProg->release();}
    