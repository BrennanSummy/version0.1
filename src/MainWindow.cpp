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

    RenderWindow*   renderArea      = new RenderWindow(this, default_kT,default_tension,default_intraLayerFirstNeighbor, default_crossLayerComplement,
                                                        default_crossLayerNeighbor,default_crossLayerBoundary);
    QWidget*        parameterArea   = new QWidget();

    // Configure parameter area
    QVBoxLayout*    parameterLayout = new QVBoxLayout(parameterArea);
    parameterLayout->setContentsMargins(0, 0, 0, 0);

    // Add parameter sliders
    //////////////////////////////////////////////////////                  Title               Min   Max  Default                          Steps
    kTSlider                    = new ParameterSlider("kT"               ,     0.01,    5, default_kT                      ,  100, parameterArea);
    nearestNeighborSlider       = new ParameterSlider("N-Neighbor factor",      0.1,    2, default_intraLayerFirstNeighbor ,  100, parameterArea);
    secondNearestNeighborSlider = new ParameterSlider("2nd-N-Neighbor factor",  0.1,    2, default_intraLayerSecondNeighbor,  100, parameterArea);
    crossLayerComplementSlider  = new ParameterSlider("CL Complement "   ,       -1,    1, default_tension                 ,  100, parameterArea);
    crossLayerNeighborSlider    = new ParameterSlider("CL N-Neighbor"    ,       -1,    1, default_crossLayerNeighbor      ,  100, parameterArea);
    crossLayerBoundarySlider    = new ParameterSlider("CL Boundary"      ,     -100,  100, default_crossLayerBoundary      ,  100, parameterArea);
    framerateSlider             = new ParameterSlider("Framerate"        ,        1,   60, default_framerate               ,   60, parameterArea);
    parameterLayout->setSpacing(30);
    // left, top, right, bottom margins
    parameterLayout->setContentsMargins(0,20,0,20);
    parameterLayout->addWidget(kTSlider);
    parameterLayout->addWidget(nearestNeighborSlider);
    parameterLayout->addWidget(secondNearestNeighborSlider);
    parameterLayout->addWidget(crossLayerComplementSlider);
    parameterLayout->addWidget(crossLayerNeighborSlider);
    parameterLayout->addWidget(crossLayerBoundarySlider);
    parameterLayout->addWidget(framerateSlider);
    //parameterLayout->addWidget(slider2);

    // Add textbox for input
    QLineEdit* paramInputBox = new QLineEdit(this);
    paramInputBox->setPlaceholderText("Enter comma separated values to sample");
    parameterLayout->addWidget(paramInputBox);

    // Add start/stop button
    PushButton* startStopButton = new PushButton("Start/Stop", parameterArea);
    parameterLayout->insertWidget(0,startStopButton);

    // Add check box for recording vs free param adjustment
    QCheckBox* recordingCheckBox = new QCheckBox(parameterArea);
    recordingCheckBox->setText("Record");
    recordingCheckBox->setChecked(false);
    parameterLayout->addWidget(recordingCheckBox);

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

    // Nearest neighbor slider
    connect(nearestNeighborSlider,&ParameterSlider::sliderHasChanged,renderArea,&RenderWindow::sliderUpdateNearestNeighbor);

    // Second nearest neighbor slider
    connect(secondNearestNeighborSlider,&ParameterSlider::sliderHasChanged,renderArea,&RenderWindow::sliderUpdateSecondNearestNeighbor);

    // Cross layer complement slider
    connect(crossLayerComplementSlider,&ParameterSlider::sliderHasChanged,renderArea,&RenderWindow::sliderUpdateCrossLayerComplement);

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
   // connect(startStopButton,&PushButton::buttonPressed   ,renderArea,&RenderWindow::oneKSteps);

    if(DEVICE)
    {
        // Laptop
        connect(recordingCheckBox,&QCheckBox::stateChanged, this, &MainWindow::toggleParamSliders);//TOPBOTTOMSELECT//LAPTOP
        connect(showTopCheckBox,&QCheckBox::stateChanged, renderArea, &RenderWindow::updateShowTop);//TOPBOTTOMSELECT//LAPTOP
        connect(showBotCheckBox,&QCheckBox::stateChanged, renderArea, &RenderWindow::updateShowBot);//TOPBOTTOMSELECT//LAPTOP
    } else
    {
        // Desktop
        connect(recordingCheckBox,&QCheckBox::checkStateChanged, this, &MainWindow::toggleParamSliders);//TOPBOTTOMSELECT//LAPTOP
        connect(showTopCheckBox,&QCheckBox::checkStateChanged, renderArea, &RenderWindow::updateShowTop);//TOPBOTTOMSELECT
        connect(showBotCheckBox,&QCheckBox::checkStateChanged, renderArea, &RenderWindow::updateShowBot);//TOPBOTTOMSELECT
    }

    // Connect the Simulation pause/play signal to the startStopButton
    connect(renderArea->m_sim,&Simulation::toggleStartStop, startStopButton, &PushButton::onPress);

    // Connect Simulation saveSnapshot to renderArea method
    connect(renderArea->m_sim,&Simulation::saveSnapshot,renderArea,&RenderWindow::printToImageFile);

    // Refresh all default values to make sure sliders match simulation, etc...
    //kTSlider->emitDefaultValue();
    //tensionSlider->emitDefaultValue();
    //neighborSlider->emitDefaultValue();
    //crossLayerNeighborSlider->emitDefaultValue();
    //crossLayerBoundarySlider->emitDefaultValue();
    //framerateSlider->emitDefaultValue();

    //std::cout << "End of Main Window Setup" << std::endl;
}

void MainWindow::toggleParamSliders()
{
    bool state = kTSlider->isEnabled();

    kTSlider->setDisabled(state);
    nearestNeighborSlider->setDisabled(state);
    secondNearestNeighborSlider->setDisabled(state);
    crossLayerComplementSlider->setDisabled(state);
    crossLayerNeighborSlider->setDisabled(state);
    crossLayerBoundarySlider->setDisabled(state);
    framerateSlider->setDisabled(state);

    std::cout<<"Sliders toggled"<<std::endl;
}