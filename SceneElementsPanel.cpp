#include "SceneElementsPanel.h"

SceneElementsPanel::SceneElementsPanel(QWidget* parent)
    : QWidget(parent) {

    auto layout = new QVBoxLayout(this);

    object_list_ = new QTreeWidget;
    object_list_->setHeaderLabels({ "Name", "Type" });
    layout->addWidget(object_list_);

    connect(object_list_, &QTreeWidget::itemClicked,
        this, &SceneElementsPanel::onObjectSelected);

    editor_stack_ = new QStackedWidget;
    layout->addWidget(editor_stack_);

    editor_stack_->addWidget(createSphereEditor());  
    editor_stack_->addWidget(createCubeEditor());    

    add_object_button_ = new QPushButton("Add Object");
    layout->addWidget(add_object_button_);
    connect(add_object_button_, &QPushButton::clicked,
        this, &SceneElementsPanel::onAddObject);

    loadSceneObjects();
}

void SceneElementsPanel::loadSceneObjects() {
    // Mockup data
    auto sphere_item = new QTreeWidgetItem(object_list_);
    sphere_item->setText(0, "Sphere 1");
    sphere_item->setText(1, "Sphere");

    auto cube_item = new QTreeWidgetItem(object_list_);
    cube_item->setText(0, "Cube 1");
    cube_item->setText(1, "Cube");
}

void SceneElementsPanel::onObjectSelected(QTreeWidgetItem* item, int column) {
    QString type = item->text(1);
    if (type == "Sphere") {
        editor_stack_->setCurrentIndex(0);
    }
    else if (type == "Cube") {
        editor_stack_->setCurrentIndex(1);
    }
}

void SceneElementsPanel::onAddObject() {
    // Trigger new object creation (e.g. show dialog or add placeholder)
    emit objectAdded();
}

QWidget* SceneElementsPanel::createSphereEditor() {
    auto widget = new QWidget;
    auto form = new QFormLayout(widget);

    form->addRow("Center X", new QLineEdit);
    form->addRow("Center Y", new QLineEdit);
    form->addRow("Center Z", new QLineEdit);
    form->addRow("Radius", new QLineEdit);

    form->addRow("Material", new QComboBox);  // Material selector
    return widget;
}

QWidget* SceneElementsPanel::createCubeEditor() {
    auto widget = new QWidget;
    auto form = new QFormLayout(widget);

    form->addRow("Position X", new QLineEdit);
    form->addRow("Position Y", new QLineEdit);
    form->addRow("Position Z", new QLineEdit);
    form->addRow("Size", new QLineEdit);

    form->addRow("Material", new QComboBox);
    return widget;
}
