#include "MainWindow.h"


// Constructor
MainWindow::MainWindow(QWidget *parent):
QMainWindow(parent){
    // Basic settings
    setWindowTitle("Simulation Window");
    // Window size
    resize(900,800);

    setupWindow();
}

// Graphical Window Setup
void MainWindow::setupWindow()
{
    // Create main splitter
    QSplitter*      splitter        = new QSplitter(Qt::Horizontal);

    RenderWindow*   renderArea      = new RenderWindow(this, default_kT,default_tension,default_intraLayerNeighbor,
                                                        default_crossLayerNeighbor,default_crossLayerBoundary);
    QWidget*        parameterArea   = new QWidget();

    // Configure parameter area
    QVBoxLayout*    parameterLayout = new QVBoxLayout(parameterArea);
    parameterLayout->setContentsMargins(0, 0, 0, 0);

    // Add parameter sliders
    //////////////////////////////////////////////////////           Title               Min   Max  Default                    Steps
    ParameterSlider* kTSlider                 = new ParameterSlider("kT"               , 0.01,    5, default_kT                ,   50, parameterArea);
    ParameterSlider* tensionSlider            = new ParameterSlider("Tension factor"   ,    0,  0.1, default_tension           ,  100, parameterArea);
    ParameterSlider* neighborSlider           = new ParameterSlider("N-Neighbor factor",    0,   10, default_intraLayerNeighbor,  100, parameterArea);
    ParameterSlider* crossLayerNeighborSlider = new ParameterSlider("CL N-Neighbor"    ,   -1,    1, default_crossLayerNeighbor,   20, parameterArea);
    ParameterSlider* crossLayerBoundarySlider = new ParameterSlider("CL Boundary"      ,   -1,    1, default_crossLayerBoundary,   20, parameterArea);
    ParameterSlider* framerateSlider          = new ParameterSlider("Framerate"        ,  0.5,   60, default_framerate         ,  120, parameterArea);
    parameterLayout->setSpacing(30);
    // left, top, right, bottom margins
    parameterLayout->setContentsMargins(0,20,0,20);
    parameterLayout->addWidget(kTSlider);
    parameterLayout->addWidget(neighborSlider);
    parameterLayout->addWidget(tensionSlider);
    parameterLayout->addWidget(framerateSlider);
    parameterLayout->addWidget(crossLayerNeighborSlider);
    parameterLayout->addWidget(crossLayerBoundarySlider);
    //parameterLayout->addWidget(slider2);

    // Add start/stop button
    PushButton* startStopButton = new PushButton("Start/Stop", parameterArea);
    parameterLayout->insertWidget(0,startStopButton);

    // Add check boxes for layer showing
    QCheckBox* showTopCheckBox = new QCheckBox(parameterArea);
    QCheckBox* showBotCheckBox = new QCheckBox(parameterArea);
    showTopCheckBox->setText("Show Top");
    showTopCheckBox->setChecked(true);
    showBotCheckBox->setText("Show Bottom");
    showBotCheckBox->setChecked(true);
    parameterLayout->addWidget(showTopCheckBox);
    parameterLayout->addWidget(showBotCheckBox);

    // Add a stretch item to push other widgets up
    parameterLayout->addStretch(1);

    // Add areas to splitter
    splitter->addWidget(renderArea);
    splitter->addWidget(parameterArea);

    // Set initial sizes
    splitter->setSizes({800, 100});

    // Set central widget
    setCentralWidget(splitter);

    // Make a custom timer object
    UpdateTimer* updateTimer = new UpdateTimer(this,1000/30);

    // Connect things
    // Temperature slider
    connect(kTSlider     ,&ParameterSlider::sliderHasChanged,renderArea,&RenderWindow::sliderUpdateTemp);

    // Tension slider
    connect(tensionSlider,&ParameterSlider::sliderHasChanged,renderArea,&RenderWindow::sliderUpdateTension);

    // Nearest neighbor slider
    connect(neighborSlider,&ParameterSlider::sliderHasChanged,renderArea,&RenderWindow::sliderUpdateNearestNeighbor);

    // Cross layer nearest neighbor slider
    connect(crossLayerNeighborSlider,&ParameterSlider::sliderHasChanged,renderArea,&RenderWindow::sliderUpdateCrossLayerNearestNeighbor);

    // Cross layer boundary interference slider
    connect(crossLayerBoundarySlider,&ParameterSlider::sliderHasChanged,renderArea,&RenderWindow::sliderUpdateCrossLayerBoundary);

    // Framerate slider
    connect(framerateSlider,&ParameterSlider::sliderHasChanged,updateTimer,&UpdateTimer::changeInterval);

    // Connect the QTimer to the renderArea update step
    connect(updateTimer, &UpdateTimer::timeIsUp, renderArea, &RenderWindow::drawLastAndComputeNext);

    // Connect the start/stop button to the QTimer
    connect(startStopButton,&PushButton::buttonPressed   ,updateTimer, &UpdateTimer::toggle);

    connect(showTopCheckBox,&QCheckBox::checkStateChanged, renderArea, &RenderWindow::updateShowTop);//TOPBOTTOMSELECT
                                                                                                     
    connect(showBotCheckBox,&QCheckBox::checkStateChanged, renderArea, &RenderWindow::updateShowBot);//TOPBOTTOMSELECT

    // Refresh all default values to make sure sliders match simulation, etc...
    //kTSlider->emitDefaultValue();
    //tensionSlider->emitDefaultValue();
    //neighborSlider->emitDefaultValue();
    //crossLayerNeighborSlider->emitDefaultValue();
    //crossLayerBoundarySlider->emitDefaultValue();
    //framerateSlider->emitDefaultValue();

    //std::cout << "End of Main Window Setup" << std::endl;
}