//#pragma once
//
//#include <QWidget>
//#include <qvboxlayout>
//#include <qtreewidget>
//#include <qpushbutton>
//#include <qstackedwidget>
//#include <qformlayout>
//#include <qlineedit>
//#include <qcombobox>
//
//class sceneelementspanel : public QWidget {
//    Q_OBJECT
//
//public:
//    sceneelementspanel(QWidget* parent = nullptr);
//
//signals:
//    void objectupdated(int id);
//    void objectadded();
//    void objectdeleted(int id);
//
//private slots:
//    void onobjectselected(qtreewidgetitem* item, int column);
//    void onaddobject();
//
//private:
//    qtreewidget* object_list_;
//    qstackedwidget* editor_stack_;
//    qpushbutton* add_object_button_;
//
//    QWidget* createsphereeditor();
//    QWidget* createcubeeditor();
//    QWidget* creatematerialeditor();
//
//    void loadsceneobjects();
//};
