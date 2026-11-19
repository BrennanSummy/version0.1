#include <QApplication>
#include <QOpenGLWidget>

#include "MainWindow.h"
#include "ParameterSlider.h"
#include "RenderWindow.h"

#include <iostream>

int main(int argc, char *argv[]) {
    //setenv("SYCL_UR_TRACE","1",1);
    //setenv("UR_LOG_LOADER","level:debug;flush:debug;output:stdout",1);
    QApplication app(argc, argv);

    MainWindow window;
    window.show();
    
    return app.exec();
}

