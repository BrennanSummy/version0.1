#include <QApplication>
#include <QOpenGLWidget>

#include "MainWindow.h"
#include "ParameterSlider.h"
#include "RenderWindow.h"

#include <iostream>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();
    
    return app.exec();
}

