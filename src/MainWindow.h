#pragma once

#include <QMainWindow>
#include <QSplitter>
#include <QTimer>
#include "ParameterSlider.h"
#include "PushButton.h"
#include "UpdateTimer.h"
#include "RenderWindow.h"

class MainWindow: public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
private:
   void setupWindow(); 
};