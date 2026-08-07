#include "RenderWindow.h"
#include "Simulation.h"

RenderWindow::RenderWindow(QWidget* parent)
        : QOpenGLWidget(parent), m_shader1(nullptr), m_mesh1(nullptr),
            m_boardTexture(nullptr), m_textureA(nullptr)
            , m_indexCount(0)   {}

RenderWindow::~RenderWindow()
{
    makeCurrent();
    delete m_shader1;
    delete m_mesh1;
    doneCurrent();
}

void RenderWindow::initializeGL() {
    // Initialize the OpenGL function pointers for this context
    initializeOpenGLFunctions();
    // Enable Transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


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

    int bWidthUniformLoc = m_shader1->getShaderProgram()->uniformLocation("bWidth");
    int bHeightUniformLoc = m_shader1->getShaderProgram()->uniformLocation("bHeight");
    std::cout<<"bWidthUniformLoc " << bWidthUniformLoc << std::endl;
    std::cout<<"bHeightUniformLoc " << bHeightUniformLoc << std::endl;
    m_shader1->getShaderProgram()->setUniformValue(bWidthUniformLoc, m_sim->m_width);
    m_shader1->getShaderProgram()->setUniformValue(bHeightUniformLoc, m_sim->m_height);

    m_shader1->bind();
    // Make texture objects
    std::cout << "trying to init textures" << std::endl;
    m_textureA = new Texture("t1.png");
    m_textureB = new Texture("LUNorm1.png");
    m_textureC = new Texture("RUNorm1.png");
    m_boardTexture = new Texture(m_sim->m_width,m_sim->m_height,simData);

    // Get the uniform location for textures ABC
    int m_texALoc = m_shader1->getShaderProgram()->uniformLocation("texA");
    // Get the uniform location for board texture
    int m_boardTexLoc = m_shader1->getShaderProgram()->uniformLocation("boardTexture");
    int m_texBLoc = m_shader1->getShaderProgram()->uniformLocation("texB");
    int m_texCLoc = m_shader1->getShaderProgram()->uniformLocation("texC");
    m_shader1->getShaderProgram()->setUniformValue(m_boardTexLoc,0);
    m_shader1->getShaderProgram()->setUniformValue(m_texALoc,1);
    m_shader1->getShaderProgram()->setUniformValue(m_texBLoc,2);
    m_shader1->getShaderProgram()->setUniformValue(m_texCLoc,3);

    std::cout<<"boardTex " << m_boardTexLoc << std::endl;
    std::cout<<"texALoc " << m_texALoc << std::endl;
    std::cout<<"texBLoc " << m_texBLoc << std::endl;
    std::cout<<"texCLoc " << m_texCLoc << std::endl;

    m_shader1->release();
    // Set up vertex data with positions followed by texture coords
    //std::vector<float> coolestVertices = {
    //    -1.0f           , -1.0f, 0.0f,          0.0f, 0.0f, // BLeft
    //     bottomRightX   , -1.0f, 0.0f,          1.0f, 0.0f, // BRight
    //     topLeftX       ,  topY, 0.0f,          0.0f, 1.0f, // TLeft
    //     1.0f           ,  topY, 0.0f,          1.0f, 1.0f  // TRight
    //};

    // TRY MAPPING X Texture coordinate off to the right to allow parallelogram shape 
    float texOffset = (1.0 / (float) m_sim->m_width) * (floor(m_sim->m_height/2));
    //float texOffset = 0;
    std::vector<float> coolestVertices = {
        -1.0f           , -1.0f, 0.0f,              0.0f, 0.0f, // BLeft
         1.0f           , -1.0f, 0.0f,  1.0f + texOffset, 0.0f, // BRight
        -1.0f           ,  1.0f, 0.0f,              0.0f, 1.0f, // TLeft
         1.0f           ,  1.0f, 0.0f,  1.0f + texOffset, 1.0f  // TRight
    };

    // Set up the indices for the two triangles (will be passed to the EBO)
    std::vector<unsigned int> indices =
    {
        0,1,2, // First Triangle
        1,2,3  // Second Triangle
    };
    m_indexCount = static_cast<int>(indices.size());

    // Make a mesh object
    m_mesh1 = new Mesh();
    m_mesh1->init(coolestVertices,indices,m_shader1->getShaderProgram());

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
    m_boardTexture->updateData(texData);

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


    // Set the uniform values before binding textures? OLD
    //m_boardTexture->bind(0);
    m_boardTexture->m_texture->bind(0);
    m_textureA->m_texture->bind(1);

    // order of binding doesn't matter if it's before uniform setting?
    // Bind the texture objects
    m_textureB->bind(2);
    m_textureC->bind(3);

    m_mesh1->draw(GL_TRIANGLES, m_indexCount);

    m_textureC->release(3);
    m_textureB->release(2);
    m_textureA->m_texture->release(1);
    m_boardTexture->m_texture->release(0);
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