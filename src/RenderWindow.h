#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core> // Update version if necessary
#include <QOpenGLShaderProgram>

#include "Shader.h"
#include "Mesh.h"

class RenderWindow : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    RenderWindow(QWidget* parent = nullptr);
    ~RenderWindow();

public slots:
    void sliderUpdateLVert(float sliderVal);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
private:
    void updateVertices(std::vector<float> newVertices);
    Shader* m_shader1;
    Mesh* m_mesh1;
    Texture* m_texture1;
    Texture* m_texture2;
    int m_indexCount;
};