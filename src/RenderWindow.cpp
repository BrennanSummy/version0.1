#include "RenderWindow.h"
#include "Simulation.h"

RenderWindow::RenderWindow(QWidget* parent)
        : QOpenGLWidget(parent), m_shader1(nullptr), m_mesh1(nullptr),
            m_indexCount(0)   {}

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

    char* simTopData = &m_sim->m_board[1][0][0];
    char* simBotData = &m_sim->m_board[0][0][0];

    m_shader1->bind();

    int bWidthUniformLoc = m_shader1->getShaderProgram()->uniformLocation("bWidth");
    int bHeightUniformLoc = m_shader1->getShaderProgram()->uniformLocation("bHeight");
    m_shader1->getShaderProgram()->setUniformValue(bWidthUniformLoc, m_sim->m_width);
    m_shader1->getShaderProgram()->setUniformValue(bHeightUniformLoc, m_sim->m_height);
    // Initialize with just top layer showing
    m_shader1->getShaderProgram()->setUniformValue("showTop", 1);             
    m_shader1->getShaderProgram()->setUniformValue("showBot", 1);             
    m_showTop = true;
    m_showBot = true;

    // Make texture objects
    std::cout << "trying to init textures" << std::endl;
    m_textureA = new Texture("textures/ATop.png");
    m_textureB = new Texture("textures/BTop.png");
    m_textureC = new Texture("textures/CTop.png");
    m_boardTopTexture = new Texture(m_sim->m_width,m_sim->m_height,simTopData);
    m_boardBotTexture = new Texture(m_sim->m_width,m_sim->m_height,simBotData);

    // Get the uniform location for textures ABC
    int m_texALoc = m_shader1->getShaderProgram()->uniformLocation("texA");
    int m_texBLoc = m_shader1->getShaderProgram()->uniformLocation("texB");
    int m_texCLoc = m_shader1->getShaderProgram()->uniformLocation("texC");
    // Get the uniform location for board texture
    int m_boardTopTexLoc = m_shader1->getShaderProgram()->uniformLocation("boardTopTexture");
    int m_boardBotTexLoc = m_shader1->getShaderProgram()->uniformLocation("boardBotTexture");

    m_shader1->getShaderProgram()->setUniformValue(m_boardTopTexLoc,0);
    m_shader1->getShaderProgram()->setUniformValue(m_texALoc,1);
    m_shader1->getShaderProgram()->setUniformValue(m_texBLoc,2);
    m_shader1->getShaderProgram()->setUniformValue(m_texCLoc,3);
    m_shader1->getShaderProgram()->setUniformValue(m_boardBotTexLoc,4);

    std::cout<<"boardTopTex " << m_boardTopTexLoc << std::endl;
    std::cout<<"texALoc " << m_texALoc << std::endl;
    std::cout<<"texBLoc " << m_texBLoc << std::endl;
    std::cout<<"texCLoc " << m_texCLoc << std::endl;
    std::cout<<"boardBotTex " << m_boardBotTexLoc << std::endl;

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
}

void RenderWindow::drawLastAndComputeNext()
{
    // Load current textures
    char* topTexData = &m_sim->m_board[1][0][0];
    char* botTexData = &m_sim->m_board[0][0][0];
    //char* texData = &m_sim->m_displayBoard[0][0];
    m_boardTopTexture->updateData(topTexData);
    m_boardBotTexture->updateData(botTexData);

    // Draw
    update();
    // Compute next simulation state
    m_sim->randomStep();
}

void RenderWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw

    // Bind the shader object
    m_shader1->bind();


    // Bind the textures
    m_boardTopTexture->m_texture->bind(0);
    m_textureA->m_texture->bind(1);
    m_textureB->bind(2);
    m_textureC->bind(3);
    m_boardBotTexture->m_texture->bind(4);

    m_mesh1->draw(GL_TRIANGLES, m_indexCount);

    m_boardBotTexture->m_texture->release(4);
    m_textureC->release(3);
    m_textureB->release(2);
    m_textureA->m_texture->release(1);
    m_boardTopTexture->m_texture->release(0);
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

void RenderWindow::updateShowTop()
{
    makeCurrent();
    // Bind the shader program and alert if unsuccessful
    if(!m_shader1->getShaderProgram()->bind()){std::cout << "shader not bound"<<std::endl;}
    // Toggle the appropriate boolean and update the uniform used in the fragment shader
    if(m_showTop){m_showTop=false;}
    else {m_showTop=true;}
    m_shader1->getShaderProgram()->setUniformValue("showTop",m_showTop);
    // Release and render the change
    m_shader1->release();
    paintGL();
    doneCurrent();
}
void RenderWindow::updateShowBot()
{
    makeCurrent();
    // Bind the shader program and alert if unsuccessful
    if(!m_shader1->getShaderProgram()->bind()){std::cout << "shader not bound"<<std::endl;}
    // Toggle the appropriate boolean and update the uniform used in the fragment shader
    if(m_showBot){m_showBot=false;}
    else { m_showBot=true;}
    m_shader1->getShaderProgram()->setUniformValue("showBot",m_showBot);
    // Release and render the change
    m_shader1->release();
    paintGL();
    doneCurrent();
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

