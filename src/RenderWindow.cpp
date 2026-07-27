#include "RenderWindow.h"
#include "Simulation.h"

RenderWindow::RenderWindow(QWidget* parent)
        : QOpenGLWidget(parent), m_shader1(nullptr), m_mesh1(nullptr),
            m_texture1(nullptr), m_indexCount(0)   {}

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
    //QString mesh1FragShaderPath = "src/shaders/fragShaderA.frag";
    QString mesh1FragShaderPath = "src/shaders/fragShaderDomainMapping.frag";

    m_shader1 = new Shader();
    m_shader1->loadShaders(mesh1VertShaderPath,mesh1FragShaderPath);


    // 2. Set up vertex data with positions followed by texture coords
    std::vector<float> coolestVertices = {
        -0.5f, -0.5f, 0.0f,          0.0f, 0.0f, // BLeft
         0.5f, -0.5f, 0.0f,          1.0f, 0.0f, // BRight
        -0.5f,  0.5f, 0.0f,          0.0f, 1.0f, // TLeft
         1.0f,  1.0f, 0.0f,          1.0f, 1.0f  // TRight
    };

    std::vector<unsigned int> indices =
    {
        0,1,2, // First Triangle
        1,2,3  // Second Triangle
    };
    m_indexCount = static_cast<int>(indices.size());
    std::cout << "index count is " << m_indexCount << std::endl;

    // Make a mesh object
    std::cout << "trying to init mesh" << std::endl;
    m_mesh1 = new Mesh();
    m_mesh1->init(coolestVertices,indices,m_shader1->getShaderProgram());

    // Ensure simulation object is initialized
    //Simulation mySim = Simulation();
    m_sim = new Simulation();
    m_sim->printBoard();

    char* simData = &m_sim->m_board[0][0];

    // Make a texture object
    std::cout << "trying to init texture" << std::endl;
    //m_texture1 = new Texture("t2.png");
    m_texture2 = new Texture(m_sim->m_width,m_sim->m_height,simData);

}

void RenderWindow::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void RenderWindow::drawLastAndComputeNext()
{
    // Load current texture
    char* texData = &m_sim->m_board[0][0];
    m_texture2->updateData(texData);
    // Draw
    update();
    // Compute next simulation state
    m_sim->step();
}

void RenderWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw

    // Bind the shader object
    m_shader1->bind();

    // Bind the texture object
    m_texture2->bind(0);
    m_shader1->getShaderProgram()->setUniformValue("texture",0);

    m_mesh1->draw(GL_TRIANGLES, m_indexCount);

    m_shader1->release();
}

void RenderWindow::sliderUpdateLVert(float sliderVal)
{

    //std::vector<float> newVertices = {
    //        -1.0f, sliderVal, 0.0f, -1.0f,-1.0f, // BLeft
    //         1.0f, -1.0f, 0.0f,      1.0f,-1.0f, // BRight
    //        -1.0f,  1.0f, 0.0f,     -1.0f, 1.0f, // TLeft
    //         1.0f,  1.0f, 0.0f,      1.0f, 1.0f  // TRight
    //    };
    std::vector<float> newVertices = {
            -1.0f, sliderVal, 0.0f,  0.0f, 0.0f, // BLeft
             0.0f, -1.0f, 0.0f,      1.0f, 0.0f, // BRight
             0.0f,  1.0f, 0.0f,      0.0f, 1.0f, // TLeft
             1.0f,  1.0f, 0.0f,      1.0f, 1.0f  // TRight
        };

   // std::vector<float> newVertices = {
   //     -0.5f, sliderVal, 0.0f,     // BLeft
   //      0.5f, -0.5f,     0.0f,     // BRight
   //     -0.5f,  0.5f,     0.0f,     // TLeft
   //      0.5f,  0.5f,     0.0f      // TRight
   // };
    updateVertices(newVertices);
    update();
}

void RenderWindow::updateVertices(std::vector<float> newVertices)
{
    m_mesh1->updateVertData(newVertices);
}