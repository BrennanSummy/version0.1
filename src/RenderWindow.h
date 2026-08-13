#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core> // Update version if necessary
#include <QOpenGLShaderProgram>
#include <QWheelEvent>
#include <QMouseEvent>

#include "Shader.h"
#include "Mesh.h"
#include "Simulation.h"

class RenderWindow : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    RenderWindow(QWidget* parent = nullptr);
    ~RenderWindow();
    void drawLastAndComputeNext();

public slots:
    void sliderUpdateTemp(float sliderVal);
    void sliderUpdateTension(float sliderVal);
    void sliderUpdateNearestNeighbor(float sliderVal);
    void updateShowTop();//TOPBOTTOMSELECT
    void updateShowBot();//TOPBOTTOMSELECT

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void wheelEvent(QWheelEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
private:
    void updateVertices(std::vector<float> newVertices);
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    Shader* m_shader1;
    Mesh* m_mesh1;
    Texture* m_textureA;
    Texture* m_textureB;
    Texture* m_textureC;
    Texture* m_boardTopTexture;
    Texture* m_boardBotTexture;
    int m_boardTopTexLoc;
    int m_boardBotTexLoc;
    int m_texALoc;
    int m_texBLoc;
    int m_texCLoc;
    int m_indexCount;
    int m_showTopLoc;//TOPBOTTOMSELECT
    int m_showBotLoc;//TOPBOTTOMSELECT
    bool m_showTop;
    bool m_showBot;
    Simulation* m_sim;
    //void multiThreadStep();

    //QPointF mouseInitPosition;
    Position mouseInitPosition;
    int m_w;
    int m_h;
};