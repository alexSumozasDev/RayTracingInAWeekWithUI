#include "RayTracingInAWeekWithUI.h"
#include <QPainter>
#include "CameraParametersPanel.h"
#include "SceneElementsPanel.h"

RayTracingInAWeekWithUI::RayTracingInAWeekWithUI(QWidget *parent) : QMainWindow(parent)
{

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    setupPalette();

    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    QWidget* topRow = new QWidget;
    QHBoxLayout* topLayout = new QHBoxLayout(topRow);

    CameraParametersPanel* menu = new CameraParametersPanel(this, cam_panel_pal);


    QWidget* rightWidget = new QWidget;
    rightWidget->setStyleSheet("background-color: lightgray");

    topLayout->addWidget(menu, 25);
    topLayout->addWidget(rightWidget, 70);
    topLayout->addStretch(5);

    QWidget* bottomRow = new QWidget;
    bottomRow->setStyleSheet("background-color: darkgray");

    QHBoxLayout* bottomLayout = new QHBoxLayout(bottomRow);
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(new SceneElementsPanel(this));
    bottomLayout->addStretch(1);


    mainLayout->addWidget(topRow, 6);   
    mainLayout->addWidget(bottomRow, 3); 
    mainLayout->addStretch(1);       

}

RayTracingInAWeekWithUI::~RayTracingInAWeekWithUI()
{}

void RayTracingInAWeekWithUI::setupPalette() {

    pal = new QPalette();
    cam_panel_pal = new QPalette();

    QColor textColor = QColor(244, 245, 252);

    pal->setColor(QPalette::Window, QColor(36, 41, 62));
    pal->setColor(QPalette::WindowText, textColor);

    uint cont_off = 30;
    cam_panel_pal->setColor(QPalette::Window, QColor(36 + cont_off, 41 + cont_off, 62 + cont_off));
    cam_panel_pal->setColor(QPalette::WindowText, textColor);

    cam_panel_pal->setColor(QPalette::Button, QColor(142, 187, 255));



    this->setAutoFillBackground(true);
    this->setPalette(*pal);
}


