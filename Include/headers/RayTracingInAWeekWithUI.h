#pragma once
#include <qpalette.h>
#include <QPainter>
#include <QResizeEvent>
#include <QtWidgets/QMainWindow>
#include "ui_RayTracingInAWeekWithUI.h"

class RayTracingInAWeekWithUI : public QMainWindow
{
    Q_OBJECT
    QPalette* pal;
    QPalette* cam_panel_pal;
public:
    RayTracingInAWeekWithUI(QWidget *parent = nullptr);
    ~RayTracingInAWeekWithUI();
    void setupPalette();

private:
    Ui::RayTracingInAWeekWithUIClass ui;
};
