#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core> // Use the version you need
#include <QOpenGLShaderProgram>

#include <GL/gl.h>

class RenderWindow : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public slots:
    void sliderUpdateLVert(float sliderVal);
    void sliderUpdateTVert(float sliderVal);
    void sliderUpdateRVert(float sliderVal);

protected:
    void updateVerts(float* newVerts, int sizeInBytes);
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
private:
    QOpenGLShaderProgram *m_program = nullptr;
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
};