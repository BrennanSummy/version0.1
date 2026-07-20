#pragma once

#include <QMainWindow>
#include "ParameterSlider.h"
#include "RenderWindow.h"

class MainWindow: public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
private:
   void setupWindow(); 
};