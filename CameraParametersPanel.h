#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>

class CameraParametersPanel : public QWidget {
    QPalette* palette;
    QFont header_font;
    QFont regular_font;

public:
    CameraParametersPanel(QWidget* parent = nullptr, QPalette* palette_par = nullptr)
        : QWidget(parent), palette(palette_par) {

        setAutoFillBackground(true);
        setPalette(*palette);
        configureFonts();

        QVBoxLayout* main_layout = new QVBoxLayout(this);

        QLabel* label = new QLabel("Camera Parameters:");
        label->setStyleSheet("QLabel { color: #8EBBFF; }");
        label->setFont(header_font);
        main_layout->addWidget(label);

        label = new QLabel("Pos: {x, y, z}");
        label->setFont(regular_font);
        main_layout->addWidget(label);

        auto x_cam = new QDoubleSpinBox;
        auto y_cam = new QDoubleSpinBox;
        auto z_cam = new QDoubleSpinBox;

        for (auto& spin_box : { x_cam, y_cam, z_cam }) {
            spin_box->setRange(-100000.0, 100000.0);
            spin_box->setDecimals(4);
            spin_box->setSingleStep(0.01);
        }

        QHBoxLayout* position_layout = new QHBoxLayout;
        position_layout->addWidget(x_cam);
        position_layout->addWidget(y_cam);
        position_layout->addWidget(z_cam);
        main_layout->addLayout(position_layout);

        label = new QLabel("Rotation: {Pitch, Yaw, Roll}");
        label->setFont(regular_font);
        main_layout->addWidget(label);

        auto pitch_cam = new QDoubleSpinBox;
        auto roll_cam = new QDoubleSpinBox;
        auto yaw_cam = new QDoubleSpinBox;

        for (auto& spin_box : { pitch_cam, roll_cam, yaw_cam }) {
            spin_box->setRange(-360, 360);
            spin_box->setDecimals(4);
            spin_box->setSingleStep(0.01);
        }

        QHBoxLayout* rotation_layout = new QHBoxLayout;
        rotation_layout->addWidget(pitch_cam);
        rotation_layout->addWidget(roll_cam);
        rotation_layout->addWidget(yaw_cam);
        main_layout->addLayout(rotation_layout);

        QHBoxLayout* fov_samples_layout = new QHBoxLayout;

        label = new QLabel("Fov: ");
        label->setFont(regular_font);
        fov_samples_layout->addWidget(label);

        auto fov_cam = new QDoubleSpinBox;
        fov_cam->setRange(0, 360);
        fov_cam->setDecimals(4);
        fov_cam->setSingleStep(0.01);
        fov_samples_layout->addWidget(fov_cam);

        label = new QLabel("Number of samples:");
        label->setFont(regular_font);
        fov_samples_layout->addWidget(label);

        auto sample_count_cam = new QDoubleSpinBox;
        sample_count_cam->setRange(0, 5000);
        sample_count_cam->setDecimals(0);
        sample_count_cam->setSingleStep(1);
        fov_samples_layout->addWidget(sample_count_cam);

        main_layout->addLayout(fov_samples_layout);

        QHBoxLayout* bounces_layout = new QHBoxLayout;

        label = new QLabel("Max bounces:");
        label->setFont(regular_font);
        bounces_layout->addWidget(label);

        auto bounce_count_cam = new QDoubleSpinBox;
        bounce_count_cam->setRange(0, 5000);
        bounce_count_cam->setDecimals(0);
        bounce_count_cam->setSingleStep(1);
        bounces_layout->addWidget(bounce_count_cam);

        main_layout->addLayout(bounces_layout);

        main_layout->addWidget(new QPushButton("Render Image", this));
        main_layout->addStretch();
        setLayout(main_layout);
    }

private:
    void configureFonts() {
        header_font.setFamily("Corbel");
        header_font.setPointSize(20);
        header_font.setBold(true);
        header_font.setItalic(true);

        regular_font.setFamily("Corbel");
        regular_font.setPointSize(10);
        regular_font.setBold(true);
    }
};
