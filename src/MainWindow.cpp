#include "MainWindow.h"


// Constructor
MainWindow::MainWindow(QWidget *parent):
QMainWindow(parent),m_recording(false),m_fileWriter(){
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

    m_renderArea      = new RenderWindow(this, default_kT,default_tension,default_intraLayerFirstNeighbor, default_crossLayerComplement,
                                                        default_crossLayerNeighbor,default_crossLayerBoundary);
    QWidget*        parameterArea   = new QWidget();

    // Configure parameter area
    QVBoxLayout*    parameterLayout = new QVBoxLayout(parameterArea);
    parameterLayout->setContentsMargins(0, 0, 0, 0);

    // Add parameter sliders
    //////////////////////////////////////////////////////                  Title               Min   Max  Default                          Steps
    kTSlider                        = new ParameterSlider("kT"               ,     0.01,    5, default_kT                      ,   10, parameterArea);
    nearestNeighborSlider           = new ParameterSlider("N-Neighbor factor",      0.1,    2, default_intraLayerFirstNeighbor ,   10, parameterArea);
    secondNearestNeighborSlider     = new ParameterSlider("2nd-N-Neighbor factor",  0.1,    2, default_intraLayerSecondNeighbor,   10, parameterArea);
    crossLayerComplementSlider      = new ParameterSlider("CL Complement "   ,       -1,    1, default_tension                 ,   10, parameterArea);
    crossLayerNeighborSlider        = new ParameterSlider("CL N-Neighbor"    ,       -1,    1, default_crossLayerNeighbor      ,   10, parameterArea);
    crossLayerSecondNeighborSlider  = new ParameterSlider("CL 2nd-N-Neighbor",       -1,    1, default_crossLayerNeighbor      ,   10, parameterArea);
    crossLayerBoundarySlider        = new ParameterSlider("CL Boundary"      ,     -100,  100, default_crossLayerBoundary      ,   10, parameterArea);
    framerateSlider                 = new ParameterSlider("Framerate"        ,        1,   60, default_framerate               ,   60, parameterArea);
    parameterLayout->setSpacing(20);
    // left, top, right, bottom margins
    parameterLayout->setContentsMargins(0,20,0,20);
    parameterLayout->addWidget(kTSlider);
    parameterLayout->addWidget(nearestNeighborSlider);
    parameterLayout->addWidget(secondNearestNeighborSlider);
    parameterLayout->addWidget(crossLayerComplementSlider);
    parameterLayout->addWidget(crossLayerNeighborSlider);
    parameterLayout->addWidget(crossLayerSecondNeighborSlider);
    parameterLayout->addWidget(crossLayerBoundarySlider);
    parameterLayout->addWidget(framerateSlider);
    //parameterLayout->addWidget(slider2);

    // Add textbox for input
    paramInputBox = new TextBox("Goofy box","Enter comma separated values to sample", this);
    parameterLayout->addWidget(paramInputBox);

    // Add start/stop button
    PushButton* startStopButton = new PushButton("Start/Stop", parameterArea);
    parameterLayout->insertWidget(0,startStopButton);

    // Add check box for recording vs free param adjustment
    recordingCheckBox = new QCheckBox(parameterArea);
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
    splitter->addWidget(m_renderArea);
    splitter->addWidget(parameterArea);

    // Set initial sizes
    splitter->setSizes({800, 100});

    // Set central widget
    setCentralWidget(splitter);

    // Make a custom timer object
    updateTimer = new UpdateTimer(this,1000/60);

    // Connect things
    // Temperature slider
    connect(kTSlider     ,&ParameterSlider::sliderHasChanged,m_renderArea,&RenderWindow::sliderUpdateTemp);

    // Nearest neighbor slider
    connect(nearestNeighborSlider,&ParameterSlider::sliderHasChanged,m_renderArea,&RenderWindow::sliderUpdateNearestNeighbor);

    // Second nearest neighbor slider
    connect(secondNearestNeighborSlider,&ParameterSlider::sliderHasChanged,m_renderArea,&RenderWindow::sliderUpdateSecondNearestNeighbor);

    // Cross layer complement slider
    connect(crossLayerComplementSlider,&ParameterSlider::sliderHasChanged,m_renderArea,&RenderWindow::sliderUpdateCrossLayerComplement);

    // Cross layer nearest neighbor slider
    connect(crossLayerNeighborSlider,&ParameterSlider::sliderHasChanged,m_renderArea,&RenderWindow::sliderUpdateCrossLayerNearestNeighbor);
    connect(crossLayerSecondNeighborSlider,&ParameterSlider::sliderHasChanged,m_renderArea->m_sim,&Simulation::updateCrossLayerSecondNearestNeighbor);

    // Cross layer boundary interference slider
    connect(crossLayerBoundarySlider,&ParameterSlider::sliderHasChanged,m_renderArea,&RenderWindow::sliderUpdateCrossLayerBoundary);

    // Framerate slider
    connect(framerateSlider,&ParameterSlider::sliderHasChanged,updateTimer,&UpdateTimer::changeInterval);

    // Connect the QTimer to the renderArea update step
    connect(updateTimer, &UpdateTimer::timeIsUp, m_renderArea, &RenderWindow::drawLastAndComputeNext);

    // Connect the start/stop button to the QTimer
    //connect(startStopButton,&PushButton::buttonPressed   ,updateTimer, &UpdateTimer::toggle);
    // Connect the start/stop button to the main window method which decides to start recording or free playing
    connect(startStopButton,&PushButton::buttonPressed   ,this, &MainWindow::startStop);

    // Connect the MainWindow start signal to the QTimer
    connect(this,&MainWindow::startStopFrameTimer   ,updateTimer, &UpdateTimer::toggle);

    // Connect the MainWindow record start signal to 
    //connect(this,&MainWindow::startRecording   ,updateTimer, &UpdateTimer::toggle);

   // connect(startStopButton,&PushButton::buttonPressed   ,renderArea,&RenderWindow::oneKSteps);

   // Connect the textbox to the mainwindow
   connect(paramInputBox, &TextBox::paramListSignal, this, &MainWindow::receiveParameterList);

    if(DEVICE)
    {
        // Laptop
        connect(recordingCheckBox,&QCheckBox::stateChanged, this, &MainWindow::toggleRecording);//TOPBOTTOMSELECT//LAPTOP
        connect(showTopCheckBox,&QCheckBox::stateChanged, m_renderArea, &RenderWindow::updateShowTop);//TOPBOTTOMSELECT//LAPTOP
        connect(showBotCheckBox,&QCheckBox::stateChanged, m_renderArea, &RenderWindow::updateShowBot);//TOPBOTTOMSELECT//LAPTOP
    } else
    {
        // Desktop
        connect(recordingCheckBox,&QCheckBox::checkStateChanged, this, &MainWindow::toggleRecording);//TOPBOTTOMSELECT//LAPTOP
        connect(showTopCheckBox,&QCheckBox::checkStateChanged, m_renderArea, &RenderWindow::updateShowTop);//TOPBOTTOMSELECT
        connect(showBotCheckBox,&QCheckBox::checkStateChanged, m_renderArea, &RenderWindow::updateShowBot);//TOPBOTTOMSELECT
    }

    // Connect the Simulation pause/play signal to the startStopButton
    connect(m_renderArea->m_sim,&Simulation::toggleStartStop, startStopButton, &PushButton::onPress);

    // Connect Simulation saveSnapshot to renderArea method
    connect(m_renderArea->m_sim,&Simulation::saveSnapshot,m_renderArea,&RenderWindow::printToImageFile);

    // Refresh all default values to make sure sliders match simulation, etc...
    //kTSlider->emitDefaultValue();
    //tensionSlider->emitDefaultValue();
    //neighborSlider->emitDefaultValue();
    //crossLayerNeighborSlider->emitDefaultValue();
    //crossLayerBoundarySlider->emitDefaultValue();
    //framerateSlider->emitDefaultValue();

    //std::cout << "End of Main Window Setup" << std::endl;
}

void MainWindow::toggleRecording()
{
    bool state = recordingCheckBox->isChecked();

    kTSlider                    ->setDisabled(state);
    nearestNeighborSlider       ->setDisabled(state);
    secondNearestNeighborSlider ->setDisabled(state);
    crossLayerComplementSlider  ->setDisabled(state);
    crossLayerNeighborSlider    ->setDisabled(state);
    crossLayerBoundarySlider    ->setDisabled(state);
    framerateSlider             ->setDisabled(state);

    std::cout<<"Sliders toggled"<<std::endl;
}

void MainWindow::receiveParameterList(ParameterList paramList)
{
    std::string key = paramList.name;
    parameterLists.insert({key,paramList});
    //DEBUGstd::cout << "-----------------------------------------------------------------------------" << std::endl;
    //DEBUGfor (auto iter = parameterLists.begin(); iter != parameterLists.end(); iter++)
    //DEBUG{
    //DEBUG    std::cout << iter->first << std::endl;
    //DEBUG    iter->second.print();
    //DEBUG}
    //DEBUGstd::cout << "-----------------------------------------------------------------------------" << std::endl;
}

void MainWindow::startStop()
{
    //disconnect(updateTimer);
    if(recordingCheckBox->isChecked())
    {
        connect(updateTimer,&UpdateTimer::timeIsUp,this,&MainWindow::stepFileForward);
        parameterListsToParameterDictMap();
        initiateNextFile();
    }
    else
    {
        connect(updateTimer,&UpdateTimer::timeIsUp,m_renderArea,&RenderWindow::drawLastAndComputeNext);
    }
    emit startStopFrameTimer();
}

void MainWindow::startFileRecord(ParameterDict paramDict)
{
    // set all of the params
    m_renderArea->m_sim->updateNearestNeighbor          (paramDict.dict.at("iNN" ));
    m_renderArea->m_sim->updateSecondNearestNeighbor    (paramDict.dict.at("i2NN"));
    m_renderArea->m_sim->updateCrossLayerNearestNeighbor(paramDict.dict.at("cNN" ));
    m_renderArea->m_sim->updateCrossLayerSecondNearestNeighbor(paramDict.dict.at("c2NN"));
    m_renderArea->m_sim->updateCrossLayerComplement     (paramDict.dict.at("cC"  ));

    p("entered method");

    // Construct a name that communicates params
    std::ostringstream osStream;
    osStream << paramDict.toString() << "_dat";
    std::string newName = osStream.str();
    p("made new name");
    // Create a new file
    m_fileWriter.setNewFileName(newName);
    p("set new name");
    m_fileWriter.createFile();

}
void MainWindow::stepFileForward()
{
    // ramp down temp
    if(m_stepIndex < kT_rampLength + 1)
    {
        double kT = start_kT + (end_kT - start_kT) * (((float)m_stepIndex) / kT_rampLength);
        // send to simulation
        m_renderArea->m_sim->updateTemp(kT);
        m_renderArea->drawLastAndComputeNext();
    }
    else if(kT_rampLength < m_stepIndex && m_stepIndex < readsPerFile)
    {
        m_renderArea->drawLastAndComputeNext();

        if(m_stepIndex%scanBoundsPeriodInSteps == 0)
        {
            // SCAN BOUNDARY LENGTHS
            std::vector<int> bLengths = m_renderArea->m_sim->m_counter.boundaryScan(0);
            std::vector<int> tLengths = m_renderArea->m_sim->m_counter.boundaryScan(1);
            // RECORD TO FILE
            m_fileWriter.vectorToLine("b_"+std::to_string(m_stepIndex)+":",bLengths);
            m_fileWriter.vectorToLine("t_"+std::to_string(m_stepIndex)+":",tLengths);
        }
        if(m_stepIndex%grabScreenPeriodInSteps == 0)
        {
            std::ostringstream osStream;
            osStream << "logs/bulk/pics/" << m_fileWriter.getName() << "_" << m_stepIndex;
            std::string name = osStream.str(); 
            m_renderArea-> printToImageFile(name);
        }
    }
    else
    {
        // START NEXT FILE
        p("file end");
        initiateNextFile();
    }
    m_stepIndex ++;

}

void MainWindow::initiateNextFile()
{
    m_fileIndex ++;
    m_stepIndex = 0;
    // Get the appropriate parameter dictionary
    ParameterDict paramDict = m_fileIndexToParamDict.at(m_fileIndex);
    p("retrieved param dict for next file");
    // Set params and make file
    startFileRecord(paramDict);
    p("startFileRecord finish");
    // Reset the simulation boards
    m_renderArea->m_sim->reset();
}

void MainWindow::parameterListsToParameterDictMap()
{
    // Loop through each list
    std::unordered_map<std::string,int> sizes;
    int productOfSizes = 1;
    for (auto iter = parameterLists.begin(); iter != parameterLists.end(); iter++)
    {
        int size = iter->second.parameters.size();
        sizes.insert({iter->first,size});
        productOfSizes = productOfSizes * size;
    }

    int runningProduct;
    for(int i = 0; i < productOfSizes; i++)
    {
        // Modulus and floor division to loop through
        // each thing divided by the product of those before, mod the size of itself.
        std::unordered_map<std::string,int> indexDict;
        std::unordered_map<std::string,float> paramDictProto;
        runningProduct = 1;
        for (auto iter = parameterLists.begin(); iter != parameterLists.end(); iter++)
        {
            int size = sizes.at(iter->first);
            int index = (i / runningProduct) % size;
            runningProduct = runningProduct * size;
            // BUNCH O STUFF
            paramDictProto.insert({iter->first,iter->second.parameters[index]});

        }
        ParameterDict paramDict(paramDictProto);
        m_fileIndexToParamDict.insert({i, paramDict});
        //DEBUGp(paramDict.toString());
    }
}