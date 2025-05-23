#include "RayTracingInAWeekWithUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RayTracingInAWeekWithUI window;

    window.resize(640, 480);

    window.setWindowTitle("Simple Qt Application");
    window.show();

    return a.exec();
}

