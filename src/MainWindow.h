#pragma once

#include <QMainWindow>
#include <QSplitter>
#include <QTimer>
#include <QCheckBox>
#include "ParameterSlider.h"
#include "PushButton.h"
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
    int default_framerate                   = 30;
private:
   void setupWindow(); 
};