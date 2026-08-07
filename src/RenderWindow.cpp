#include "RenderWindow.h"
#include "Simulation.h"

RenderWindow::RenderWindow(QWidget* parent)
        : QOpenGLWidget(parent), m_shader1(nullptr), m_mesh1(nullptr),
            m_texture1(nullptr),m_texture2(nullptr), m_indexCount(0)   {}

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
    //QString mesh1VertShaderPath = "src/shaders/vertexShaderA.vert";
    std::string vPath = std::string(SHADER_PATH) + std::string("vertexShaderA.vert");
    std::string fPath = std::string(SHADER_PATH) + std::string("fragShaderDomainMapping.frag");
    QString mesh1VertShaderPath = vPath.c_str();
    QString mesh1FragShaderPath = fPath.c_str();
    //QString mesh1FragShaderPath = "src/shaders/fragShaderA.frag";
    //QString mesh1FragShaderPath = "src/shaders/fragShaderDomainMapping.frag";





    m_shader1 = new Shader();
    m_shader1->loadShaders(mesh1VertShaderPath,mesh1FragShaderPath);

    // Ensure simulation object is initialized
    m_sim = new Simulation();
    m_sim->printBoard();

    char* simData = &m_sim->m_board[0][0];
    //char* simData = &m_sim->m_displayBoard[0][0];

    // Make a texture object
    std::cout << "trying to init texture" << std::endl;
    m_texture1 = new Texture("t1.png");
    // FOR DISPLAYBOARD int displayWidth = 2*m_sim->m_width + m_sim->m_height-1;
    // FOR DISPLAYBOARD int displayHeight = m_sim->m_height;
    // FOR DISPLAYBOARD m_texture2 = new Texture(displayWidth,displayHeight,simData);
    m_texture2 = new Texture(m_sim->m_width,m_sim->m_height,simData);

    // FOR DISPLAYBOARD // Do a bit of math to figure out where to put the vertices
    // FOR DISPLAYBOARD float w = (float) m_sim->m_width;
    // FOR DISPLAYBOARD float h = (float) m_sim->m_height;

    // FOR DISPLAYBOARD float vertexWidth = 2/(1 + (h/(2*w)));
    // FOR DISPLAYBOARD float vertexHeight = (sqrt(3) * h) / (w + (h/2));

    // FOR DISPLAYBOARD float topY = -1 + vertexHeight;
    // FOR DISPLAYBOARD float bottomRightX = -1 + vertexWidth;
    // FOR DISPLAYBOARD float topLeftX = 1 - vertexWidth;
    m_shader1->getShaderProgram()->setUniformValue("texWidth", m_sim->m_width);
    m_shader1->getShaderProgram()->setUniformValue("texHeight", m_sim->m_height);


    // Set up vertex data with positions followed by texture coords
    //std::vector<float> coolestVertices = {
    //    -1.0f           , -1.0f, 0.0f,          0.0f, 0.0f, // BLeft
    //     bottomRightX   , -1.0f, 0.0f,          1.0f, 0.0f, // BRight
    //     topLeftX       ,  topY, 0.0f,          0.0f, 1.0f, // TLeft
    //     1.0f           ,  topY, 0.0f,          1.0f, 1.0f  // TRight
    //};

    // TRY MAPPING X Texture coordinate off to the right to allow parallelogram shape 
    //float texOffset = 1.0/(4.0);
    float texOffset = (1.0 / (float) m_sim->m_width) * (floor(m_sim->m_height/2));
    //float texOffset = 0;
    std::vector<float> coolestVertices = {
        -1.0f           , -1.0f, 0.0f,              0.0f, 0.0f, // BLeft
         1.0f           , -1.0f, 0.0f,  1.0f + texOffset, 0.0f, // BRight
        -1.0f           ,  1.0f, 0.0f,              0.0f, 1.0f, // TLeft
         1.0f           ,  1.0f, 0.0f,  1.0f + texOffset, 1.0f  // TRight
    };

    std::vector<unsigned int> indices =
    {
        0,1,2, // First Triangle
        1,2,3  // Second Triangle
    };
    m_indexCount = static_cast<int>(indices.size());
    //std::cout << "index count is " << m_indexCount << std::endl;

    // Make a mesh object
    //std::cout << "trying to init mesh" << std::endl;
    m_mesh1 = new Mesh();
    m_mesh1->init(coolestVertices,indices,m_shader1->getShaderProgram());

    // Get the uniform location for texture 1
    int m_tex1Loc = m_shader1->getShaderProgram()->uniformLocation("t1");
    // Get the uniform location for texture 2
    int m_tex2Loc = m_shader1->getShaderProgram()->uniformLocation("boardTexture");

    std::cout<<"tex1Loc " << m_tex1Loc << std::endl;
    std::cout<<"tex2Loc " << m_tex2Loc << std::endl;
}

void RenderWindow::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    m_w = w;
    m_h = h;
    //std::cout << "width is now " <<m_w<<" and height " << m_h<< std::endl;
}

void RenderWindow::drawLastAndComputeNext()
{
    // Load current texture
    char* texData = &m_sim->m_board[0][0];
    //char* texData = &m_sim->m_displayBoard[0][0];
    m_texture2->updateData(texData);

    //std::cout << "texture updated to current board" << std::endl;
    // Draw
    update();
    // Compute next simulation state
    m_sim->step();
    //std::cout << "simulation step completed" << std::endl;
}

void RenderWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw

    // Bind the shader object
    m_shader1->bind();

    // Bind the texture object
    glActiveTexture(GL_TEXTURE0);
    m_texture2->bind(0);
    m_shader1->getShaderProgram()->setUniformValue(m_tex2Loc,0);

    // Set the static textures for each domain
    glActiveTexture(GL_TEXTURE1);
    m_texture1->bind(1);
    m_shader1->getShaderProgram()->setUniformValue(m_tex1Loc,1);

    //std::cout << "is texture 1 bound? " << m_texture1->getTexture()->isBound() << std::endl;

    m_mesh1->draw(GL_TRIANGLES, m_indexCount);

    m_texture2->release(0);
    m_texture1->release(1);
    m_shader1->release();
}


void RenderWindow::sliderUpdateTemp(float sliderVal)
{
    m_sim->updateTemp(sliderVal);
}

void RenderWindow::sliderUpdateTension(float sliderVal)
{
    m_sim->updateTension(sliderVal);
}

void RenderWindow::sliderUpdateNearestNeighbor(float sliderVal)
{
    m_sim->updateNearestNeighbor(sliderVal);
}

void RenderWindow::updateVertices(std::vector<float> newVertices)
{
    m_mesh1->updateVertData(newVertices);
}

void RenderWindow::wheelEvent(QWheelEvent* event) 
{
    float zoomScale;
    int wheelMove = event->angleDelta().y();
    if(wheelMove>0){zoomScale = 1.1;}
    else{zoomScale = 0.9;}
    float normX = (event->position().x())/(((float)m_w)/2) - 1;
    float normY = -((event->position().y())/(((float)m_h)/2) - 1);

    Position mousePos = {normX,normY};

    // Read current quad vertices
    std::vector<Position> globalVerts = m_mesh1->readCurrentVerts();
    // Transform those vertices into coordinates around the mouse
        // Scale those coordinates to zoom
        // Then Transform them out by adding mousePos back
    std::vector<Position> globalScaledVerts;
    for(int i = 0; i < globalVerts.size(); i++)
    {
        globalScaledVerts.push_back(  ((globalVerts[i].subtract(mousePos)).scale(zoomScale)).add(mousePos)  );
    }
    m_mesh1->writeCurrentVertsWithPositions(globalScaledVerts);
    update();
}

void RenderWindow::mousePressEvent(QMouseEvent* event)
{
    //mouseInitPosition = event->position();
    mouseInitPosition = {(float) event->position().x(),(float) -event->position().y()};
}

void RenderWindow::mouseReleaseEvent(QMouseEvent* event)
{
    m_mesh1->loadBufferToVert();
}

void RenderWindow::mouseMoveEvent(QMouseEvent* event) 
{
    // For some reason the vertical axis is flipped, so take the negative
    Position mousePos = {(float) event->position().x(),(float) -event->position().y()};
    
    Position delta = mousePos.subtract(mouseInitPosition);

    delta = {delta.x * (2/(float)m_w), delta.y * (2/(float)m_h)};
    // move the quad vertices by that delta scaled by window resolution
    m_mesh1->additiveUpdateVertData(delta);
    update();
}