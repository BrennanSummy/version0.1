#include "RenderWindow.h"

RenderWindow::RenderWindow(QWidget* parent)
        : QOpenGLWidget(parent), m_shader1(nullptr), m_mesh1(nullptr), m_indexCount(0){}

RenderWindow::~RenderWindow()
{
    makeCurrent();
    delete m_shader1;
    delete m_mesh1;
    doneCurrent();
}

void RenderWindow::initializeGL() {
    //// Crucial: Initialize the OpenGL function pointers for this context
    initializeOpenGLFunctions();


    // Set background color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Dark teal background

    // 1. Initialize the Shader object and tell it which shaders to use
    QString mesh1VertShaderPath = "src/shaders/vertexShaderA.vert";
    QString mesh1FragShaderPath = "src/shaders/fragShaderA.frag";

    m_shader1 = new Shader();
    m_shader1->loadShaders(mesh1VertShaderPath,mesh1FragShaderPath);


    // 2. Set up vertex data (XYZ coordinates for 3 points)
    std::vector<float> vertices = {
        -0.5f, -0.5f, 0.0f, // Left
         0.5f, -0.5f, 0.0f, // Right
         0.0f,  0.5f, 0.0f  // Top
    };

    std::vector<unsigned int> indices = {0,1,2};
    m_indexCount = static_cast<int>(indices.size());

    // 3. Generate and bind VAO and VBO
    // 4. Tell OpenGL how to interpret the vertex data
    // Unbind to prevent accidental modifications
    std::cout << "trying to init mesh" << std::endl;
    m_mesh1 = new Mesh();
    m_mesh1->init(vertices,indices,m_shader1->getShaderProgram());
}

void RenderWindow::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void RenderWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw
    m_shader1->bind();

    //updateVertices();

    m_mesh1->draw(GL_TRIANGLES, m_indexCount);

    m_shader1->release();
}

void RenderWindow::sliderUpdateLVert(float sliderVal)
{
    std::vector<float> newVertices = {
        -0.5f, sliderVal, 0.0f, // Left
         0.5f, -0.5f, 0.0f, // Right
         0.0f,  0.5f, 0.0f  // Top
    };
    updateVertices(newVertices);
    update();
}

void RenderWindow::updateVertices(std::vector<float> newVertices)
{
    m_mesh1->updateVertData(newVertices);
}