#pragma once

#include <QMainWindow>
#include <QSplitter>
#include <QTimer>
#include <QCheckBox>
#include <QLineEdit>
#include <unordered_map>
#include "ParameterSlider.h"
#include "PushButton.h"
#include "TextBox.h"
#include "UpdateTimer.h"
#include "RenderWindow.h"

class MainWindow: public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    double default_kT                       = 1;
    double default_tension                  = 0.00;
    double default_intraLayerFirstNeighbor  = 1;
    double default_intraLayerSecondNeighbor = 0;
    double default_crossLayerComplement     = 0;
    double default_crossLayerNeighbor       = 0;
    double default_crossLayerBoundary       = 0;
    int default_framerate                   = 60;

    // Recording? as opposed to free manipulation of sliders
    bool m_recording;

    int m_stepIndex = 0;
    int m_fileIndex = -1;
    std::string m_currentFileName;

public slots:
    void receiveParameterList(ParameterList paramList);
signals:
    void startStopFrameTimer();
    void startRecording();
private:
    void setupWindow(); 
    void toggleRecording();
    void startFileRecord(ParameterDict paramDict);
    void stepFileForward();
    void initiateNextFile();
    void parameterListsToParameterDictMap();
    void startStop();
    //std::vector<ParameterList> parameterLists;
    std::unordered_map<std::string,ParameterList> parameterLists;
    ParameterSlider* kTSlider                    ;
    ParameterSlider* nearestNeighborSlider       ;
    ParameterSlider* secondNearestNeighborSlider ;
    ParameterSlider* crossLayerComplementSlider  ;
    ParameterSlider* crossLayerNeighborSlider    ;
    ParameterSlider* crossLayerSecondNeighborSlider    ;
    ParameterSlider* crossLayerBoundarySlider    ;
    ParameterSlider* framerateSlider             ;
    UpdateTimer*     updateTimer;
    QCheckBox*       recordingCheckBox;
    TextBox*         paramInputBox;
    RenderWindow*    m_renderArea;
    FileWriter       m_fileWriter;

    std::unordered_map<int,ParameterDict> m_fileIndexToParamDict;
};