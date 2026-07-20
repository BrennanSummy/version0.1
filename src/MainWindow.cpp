#include "MainWindow.h"

#include <QSplitter>

// Constructor
MainWindow::MainWindow(QWidget *parent):
QMainWindow(parent){
    // Basic settings
    setWindowTitle("Brennan's Window");
    // Window size
    resize(1200,600);

    setupWindow();
}

// Graphical Window Setup
void MainWindow::setupWindow()
{
    // Create main splitter
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    //QOpenGLWidget* renderArea = new RenderWindow();
    RenderWindow* renderArea = new RenderWindow();
    QWidget* parameterArea = new QWidget();

    // Configure parameter area
    QVBoxLayout* parameterLayout = new QVBoxLayout(parameterArea);
    parameterLayout->setContentsMargins(0, 0, 0, 0);

    // Add parameter sliders
    ParameterSlider* slider0 = new ParameterSlider("Param0", parameterArea);
    ParameterSlider* slider1 = new ParameterSlider("Param1", parameterArea);
    ParameterSlider* slider2 = new ParameterSlider("Param2", parameterArea);
    parameterLayout->addWidget(slider0);
    parameterLayout->addWidget(slider1);
    parameterLayout->addWidget(slider2);

    // Add to splitter
    splitter->addWidget(renderArea);
    splitter->addWidget(parameterArea);

    // Set initial sizes
    splitter->setSizes({600, 100});

    // Set central widget
    setCentralWidget(splitter);

    // Connect things
    connect(slider0,&ParameterSlider::sliderHasChanged,renderArea,&RenderWindow::sliderUpdateLVert);
    connect(slider1,&ParameterSlider::sliderHasChanged,renderArea,&RenderWindow::sliderUpdateTVert);
    connect(slider2,&ParameterSlider::sliderHasChanged,renderArea,&RenderWindow::sliderUpdateRVert);
}