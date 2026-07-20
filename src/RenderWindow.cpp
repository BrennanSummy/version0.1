#include "RenderWindow.h"

static const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

static const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n" // Orange color
    "}\0";


void RenderWindow::initializeGL() {
    // Crucial: Initialize the OpenGL function pointers for this context
    initializeOpenGLFunctions();

    // Set background color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Dark teal background

    // 1. Compile and link shaders using Qt's helpful wrapper
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();

    // 2. Set up vertex data (XYZ coordinates for 3 points)
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // Left
         0.5f, -0.5f, 0.0f, // Right
         0.0f,  0.5f, 0.0f  // Top
    };

    // 3. Generate and bind VAO and VBO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 4. Tell OpenGL how to interpret the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind to prevent accidental modifications
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void RenderWindow::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void RenderWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Draw your geometry here
    m_program->bind();
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    m_program->release();
}

void RenderWindow::sliderUpdateLVert(float sliderVal)
{
    float newVertices[] = {
        -0.5f, sliderVal, 0.0f, // Left
         0.5f, -0.5, 0.0f, // Right
         0.0f,  0.5, 0.0f  // Top
    };
    int sizeInBytes = sizeof(newVertices);
    updateVerts(newVertices, sizeInBytes);
}
void RenderWindow::sliderUpdateTVert(float sliderVal)
{
    float newVertices[] = {
        -0.5f, -0.5, 0.0f, // Left
         0.5f, sliderVal, 0.0f, // Right
         0.0f,  0.5, 0.0f  // Top
    };
    int sizeInBytes = sizeof(newVertices);
    updateVerts(newVertices, sizeInBytes);
}
void RenderWindow::sliderUpdateRVert(float sliderVal)
{
    float newVertices[] = {
        -0.5f, -0.5, 0.0f, // Left
         0.5f, -0.5, 0.0f, // Right
         0.0f,  sliderVal, 0.0f  // Top
    };
    int sizeInBytes = sizeof(newVertices);
    updateVerts(newVertices, sizeInBytes);
}

void RenderWindow::updateVerts(float* newVerts, int sizeInBytes)
{
    // Summon the OpenGL context
    makeCurrent();

    // Bind the previously made Vertex Buffer Object
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // Overwrite with 0 offset. Size in bytes is needed for this
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeInBytes, newVerts);

    // Clean up by binding to 0?
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Tell Qt to do a paintGL call
    update();
}