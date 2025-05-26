#include "RayTracingInAWeekWithUI.h"
#include <QtWidgets/QApplication>

#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RayTracingInAWeekWithUI window;

    window.resize(640, 480);

    window.setWindowTitle("Simple Qt Application");
    window.show();

    return a.exec();
}

