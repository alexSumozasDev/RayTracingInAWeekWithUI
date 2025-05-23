#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>

class SceneElementsPanel : public QWidget {
    Q_OBJECT

public:
    SceneElementsPanel(QWidget* parent = nullptr);

signals:
    void objectUpdated(int id);
    void objectAdded();
    void objectDeleted(int id);

private slots:
    void onObjectSelected(QTreeWidgetItem* item, int column);
    void onAddObject();

private:
    QTreeWidget* object_list_;
    QStackedWidget* editor_stack_;
    QPushButton* add_object_button_;

    QWidget* createSphereEditor();
    QWidget* createCubeEditor();
    QWidget* createMaterialEditor();

    void loadSceneObjects();
};
