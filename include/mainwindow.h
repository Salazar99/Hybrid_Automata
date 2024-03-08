#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QPainter>
#include <QFrame>
#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QGraphicsItem>
#include <QLineEdit>
#include <QMap>
#include <QMouseEvent> // Include the necessary header for QMouseEvent
#include "../include/circleitem.h"
#include "../include/arrowitem.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QWidget *leftWidget; // The left QFrame
    QWidget *rightWidget; // The right QFrame
    QGraphicsScene *scene;
    QGraphicsEllipseItem *ellipse;
    QGraphicsEllipseItem *selectedCircle1 = nullptr;
    QGraphicsEllipseItem *selectedCircle2 = nullptr;
    QList<CircleItem*> circles;
    QTimer *timer;
    QMap<QGraphicsEllipseItem*, QList<QGraphicsEllipseItem*>> arrows;
    QMap<QString, QString> variablesValues;
    QMap<QGraphicsEllipseItem*, CircleItem*> ellipseMap;
    QMap<QString, QColor> automataColors;
    QList<QString> actualVariables;
    QList<ArrowItem*> drawnArrows;
    QLineEdit *valueLabel;
    ArrowItem * selectedArrow;
    QList<QString> automatas;
    int finalTime;
    double delta;
    QList<int> posUpdateButton;
    bool isCircleSelected = false;
    bool ascendingSelection=true;
    bool dragMode = false;
    void deleteSelectedItems();
    void handleSelectionChanged();
    void handleRefresh();
    bool checkSelected();
    void hideDesignerInput();
    void showDesignerInput(int mode);
    void clearAll(int mode);
protected:
    bool eventFilter(QObject *watched, QEvent *event); // Declare eventFilter function

private slots:
    void on_updateButton_clicked();
    void on_tabWidget_currentChanged(int index);
    void on_updtateVariable_clicked();
    void on_jsonButton_clicked();
    void on_addAutoma_clicked();
    void on_debugButton_clicked();
    void on_loadData_clicked();
};

#endif // MAINWINDOW_H
