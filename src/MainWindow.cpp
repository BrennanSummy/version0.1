#include "MainWindow.h"


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

    // Add start/stop button
    PushButton* startStopButton = new PushButton("Start/Stop", parameterArea);
    parameterLayout->addWidget(startStopButton);

    // Add areas to splitter
    splitter->addWidget(renderArea);
    splitter->addWidget(parameterArea);

    // Set initial sizes
    splitter->setSizes({600, 100});

    // Set central widget
    setCentralWidget(splitter);

    // Make a custom timer object
    UpdateTimer* updateTimer = new UpdateTimer(this,1000/60);

    // Connect things
    connect(slider0,&ParameterSlider::sliderHasChanged,renderArea,&RenderWindow::sliderUpdateLVert);

    // Connect the QTimer to the renderArea update step
    connect(updateTimer, &UpdateTimer::timeIsUp, renderArea, &RenderWindow::drawLastAndComputeNext);

    // debug: connect button to update
    //connect(updateTimer, &UpdateTimer::timeIsUp, this,[this](){std::cout<<"o"<<std::endl;});
    //connect(startStopButton, &PushButton::buttonPressed, renderArea, &RenderWindow::drawLastAndComputeNext);

    // Connect the start/stop button to the QTimer
    connect(startStopButton, &PushButton::buttonPressed, updateTimer, &UpdateTimer::toggle);

    std::cout << "End of Main Window Setup" << std::endl;
}