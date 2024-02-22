#include "../include/mainwindow.h"
#include "../include/arrowitem.h"
#include "./ui_mainwindow.h"
#include <QMouseEvent> // Include the necessary header for QMouseEvent
#include <QtWidgets>
#include <iostream>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    timer = new QTimer(this);
    // addEllipse(x,y,w,h,pen,brush)
    // movable text
    ui->graphicsView->installEventFilter(this);
    connect(scene, &QGraphicsScene::selectionChanged, this, &MainWindow::handleSelectionChanged);
    connect(timer, &QTimer::timeout, this, &MainWindow::handleRefresh);
    timer->start(33);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void printCircles(QList<CircleItem*> list){
    for (int i=0; i<list.size(); i++){
        qDebug() << *list[i];
    }
}

void MainWindow::handleRefresh(){
    ui->graphicsView->update();
    /*qDebug() << "\n";
    printCircles(circles);

    QMap<QGraphicsEllipseItem*, QList<QGraphicsEllipseItem*>>::const_iterator it;
    for (it = arrows.constBegin(); it != arrows.constEnd(); ++it) {
        QGraphicsEllipseItem* start = it.key();
        QList<QGraphicsEllipseItem*> end = it.value();

        qDebug() << "Arrow Pair: Start at" << start->sceneBoundingRect().center() << "\n";
        for (int i=0; i<end.size(); i++){
            qDebug() << "            End at" << end[i]->sceneBoundingRect().center() << "\n";
        }


    }

    qDebug() << "\n";*/
}

bool MainWindow::checkSelected(){
    QList<QGraphicsItem*> selectedItems = scene->selectedItems();
    for (int i=0; i<selectedItems.size(); i++)
    {
        if (selectedItems[i]->type() != CIRCLEITEM_TYPE)
            return false;

        CircleItem *selectedCircle = dynamic_cast<CircleItem*>(selectedItems[i]);
        if (selectedCircle->ellipse->sceneBoundingRect().center() != selectedCircle1->sceneBoundingRect().center() && selectedCircle->ellipse->sceneBoundingRect().center() != selectedCircle2->sceneBoundingRect().center())
            return false;

    }
    return true;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->graphicsView) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() != Qt::LeftButton)return true;
            QPointF scenePos = ui->graphicsView->mapToScene(mouseEvent->pos());
            QBrush greenBrush(Qt::green);
            QPen outlinePen(Qt::black);
            outlinePen.setWidth(2);
            QGraphicsEllipseItem *newEllipse = new QGraphicsEllipseItem(scenePos.x(), scenePos.y(), 80, 80);
            newEllipse->setPen(outlinePen);
            newEllipse->setBrush(greenBrush);
            /*newEllipse->setFlag(QGraphicsItem::ItemIsMovable);
            newEllipse->setFlag(QGraphicsItem::ItemIsSelectable);*/
            QGraphicsTextItem *textLabel = new QGraphicsTextItem("default");
            textLabel->setDefaultTextColor(Qt::white);
            textLabel->setFont(QFont("Arial", 10));
            textLabel->setPos(scenePos.x() + 10, scenePos.y() + 10);
            CircleItem *circleItem = new CircleItem(newEllipse, textLabel);
            circleItem->setFlag(QGraphicsItem::ItemIsMovable);
            circleItem->setFlag(QGraphicsItem::ItemIsSelectable);
            circles.append(circleItem);
            scene -> addItem(circleItem);
            qDebug() << "Position of the new circle: " << newEllipse->sceneBoundingRect().center();
            //qDebug() << "Position of the new circle: " << newEllipse->pos().x() << ", " << newEllipse->pos().y() << "\n";
            /*newEllipse->setFlag(QGraphicsItem::ItemIsMovable);
            newEllipse->setFlag(QGraphicsItem::ItemIsSelectable);*/
            qDebug() << "Mouse pressed at scenePos:" << scenePos.x() << ", " << scenePos.y() << "\n";
            return true; // Consume the event
        }
        else if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace) {
                deleteSelectedItems();
                return true; // Consume the event
            }else if(keyEvent->key() == Qt::Key_K){
                if (selectedCircle1 && selectedCircle2 && scene->selectedItems().size()==2 && checkSelected()) {
                    std::cout << "Disegnami Seh\n";
                    /*ArrowItem *arrow = new ArrowItem(selectedCircle1, selectedCircle2);
                    arrow->setFlag(QGraphicsItem::ItemIsSelectable);*/
                    ArrowItem *arrow;
                    //scene->addItem(arrow);

                    if (arrows.contains(selectedCircle1)){
                        QList<QGraphicsEllipseItem*> dest = arrows[selectedCircle1];
                        if (!dest.contains(selectedCircle2)){
                            arrow = new ArrowItem(selectedCircle1, selectedCircle2);
                            arrow->setFlag(QGraphicsItem::ItemIsSelectable);
                            scene->addItem(arrow);
                            drawnArrows.append(arrow);
                            arrows[selectedCircle1].append(selectedCircle2);
                        }
                    }else{
                        arrow = new ArrowItem(selectedCircle1, selectedCircle2);
                        arrow->setFlag(QGraphicsItem::ItemIsSelectable);
                        scene->addItem(arrow);
                        drawnArrows.append(arrow);
                        arrows.insert(selectedCircle1, QList<QGraphicsEllipseItem*>());
                        arrows[selectedCircle1].append(selectedCircle2);
                    }
                    ascendingSelection = true;
                    //arrowPairs.insert(selectedCircle1, selectedCircle2);
                    // Clear previously selected circles
                    scene->selectedItems().clear();
                }
                return true; // Consume the event
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::handleSelectionChanged(){
    // Get selected items
    QList<QGraphicsItem*> selectedItems = scene->selectedItems();
    /*
    a sel1
    b sel2
    c

    */

    if (selectedItems.size() == 0){
        selectedCircle1 = nullptr;
        selectedCircle2 = nullptr;
        ascendingSelection = true;
    }

    if (selectedItems.size() == 1){
        if (selectedItems[0]->type() == CIRCLEITEM_TYPE){
            CircleItem *selectedCircle = dynamic_cast<CircleItem*>(selectedItems[0]);
            if (!selectedCircle)qDebug() << "null\n";

            QGraphicsEllipseItem* temp = selectedCircle->ellipse;

            selectedCircle1 = qgraphicsitem_cast<QGraphicsEllipseItem*>(selectedCircle->ellipse);

        }
    }

    // Check if exactly two circles are selected
    if (selectedItems.size() == 2 && ascendingSelection) {
        if (selectedItems[0]->type() == CIRCLEITEM_TYPE &&
            selectedItems[1]->type() == CIRCLEITEM_TYPE) {
            CircleItem *selectedCircle = dynamic_cast<CircleItem*>(selectedItems[0]);
            CircleItem *selectedCircleSecond = dynamic_cast<CircleItem*>(selectedItems[1]);
            if (selectedCircle1->sceneBoundingRect().center() == qgraphicsitem_cast<QGraphicsEllipseItem*>(selectedCircle->ellipse)->sceneBoundingRect().center())
                selectedCircle2 = qgraphicsitem_cast<QGraphicsEllipseItem*>(selectedCircleSecond->ellipse);
            else
                selectedCircle2 = qgraphicsitem_cast<QGraphicsEllipseItem*>(selectedCircle->ellipse);
            std::cout << "Selected\n";
            std::cout << "Position of first: " << selectedCircle1->sceneBoundingRect().center().x() << ", " << selectedCircle1->sceneBoundingRect().center().y() << "\n";
            std::cout << "Position of second: " << selectedCircle2->sceneBoundingRect().center().x() << ", " << selectedCircle2->sceneBoundingRect().center().y() << "\n";
        }
        ascendingSelection = true;
    }

    if(selectedItems.size() >= 3)ascendingSelection = false;

}

void MainWindow::deleteSelectedItems()
{
    /*QList<QGraphicsItem*> allItems = scene->items();

    for (int i=0; i<allItems.size(); i++){
        if(dynamic_cast<ArrowItem*>(allItems[i]) != nullptr){
            scene->removeItem(allItems[i]);
            delete allItems[i];
        }
    }*/

    // Get a list of all selected items
    QList<QGraphicsItem*> selectedItems = scene->selectedItems();
    int index = -1;
    qDebug() << selectedItems.size()<<"\n";
    int count = 0;
    QGraphicsItem* tempItem;
    for (int i = 0; i<selectedItems.size(); i++){
        tempItem = selectedItems[i];
        if(dynamic_cast<ArrowItem*>(tempItem) != nullptr){
            qDebug() << "Eliminando una freccia\n";
            ArrowItem* temp = static_cast<ArrowItem*>(tempItem);

            QGraphicsItem * inizio= temp->startItem;
            QGraphicsEllipseItem* cerchioInizio = qgraphicsitem_cast<QGraphicsEllipseItem*>(inizio);

            QList<QGraphicsEllipseItem*> toRemove = arrows[cerchioInizio];
            index = -1;
            for (int i = 0; i<toRemove.size(); i++){
                if (toRemove[i] == temp->endItem){
                    index = i;
                }
            }
            if (index!=-1){
                arrows[qgraphicsitem_cast<QGraphicsEllipseItem*>(temp->startItem)].removeAt(index);
                if (arrows[qgraphicsitem_cast<QGraphicsEllipseItem*>(temp->startItem)].isEmpty()){
                    arrows.remove(qgraphicsitem_cast<QGraphicsEllipseItem*>(temp->startItem));
                }
            }

            /*Elimino il cerchio dalla lista dei cerchi*/
            index = -1;
            for (int i = 0; i<drawnArrows.size(); i++){
                if (drawnArrows[i] == temp){
                    index = i;
                }
            }
            if (index!=-1){
                /*int aux = -1;
                for (int j = 0; j<selectedItems.size(); j++){
                    if(selectedItems[j] == nullptr)continue;

                    if(dynamic_cast<ArrowItem*>(selectedItems[j]) != nullptr){
                        ArrowItem* temp = static_cast<ArrowItem*>(selectedItems[j]);
                        if (temp->startItem->sceneBoundingRect().center() == drawnArrows[index]->startItem->sceneBoundingRect().center() && temp->endItem->sceneBoundingRect().center() == drawnArrows[index]->endItem->sceneBoundingRect().center()){
                            aux = j;
                        }
                    }
                }
                if (aux!=-1)selectedItems[aux] = nullptr;*/

                delete drawnArrows[index];
                drawnArrows.removeAt(index);
            }

            selectedItems[i] = nullptr;
        }
    }


    // Delete each selected item
    for (QGraphicsItem* item : selectedItems) {

        if (item == nullptr){
            continue;
        }

        qDebug() << item->sceneBoundingRect().center() << "\n";


        if(dynamic_cast<CircleItem*>(item) == nullptr){
            qDebug() << "Eliminando una freccia\n";
            ArrowItem* temp = static_cast<ArrowItem*>(item);

            QGraphicsItem * inizio= temp->startItem;
            QGraphicsEllipseItem* cerchioInizio = qgraphicsitem_cast<QGraphicsEllipseItem*>(inizio);

            QList<QGraphicsEllipseItem*> toRemove = arrows[cerchioInizio];
            index = -1;
            for (int i = 0; i<toRemove.size(); i++){
                if (toRemove[i] == temp->endItem){
                    index = i;
                }
            }
            if (index!=-1){
                arrows[qgraphicsitem_cast<QGraphicsEllipseItem*>(temp->startItem)].removeAt(index);
                if (arrows[qgraphicsitem_cast<QGraphicsEllipseItem*>(temp->startItem)].isEmpty()){
                    arrows.remove(qgraphicsitem_cast<QGraphicsEllipseItem*>(temp->startItem));
                }
            }

            /*Elimino il cerchio dalla lista dei cerchi*/
            index = -1;
            for (int i = 0; i<drawnArrows.size(); i++){
                if (drawnArrows[i] == temp){
                    index = i;
                }
            }
            if (index!=-1){
                /*int aux = -1;
                for (int j = 0; j<selectedItems.size(); j++){
                    if(selectedItems[j] == nullptr)continue;

                    if(dynamic_cast<ArrowItem*>(selectedItems[j]) != nullptr){
                        ArrowItem* temp = static_cast<ArrowItem*>(selectedItems[j]);
                        if (temp->startItem->sceneBoundingRect().center() == drawnArrows[index]->startItem->sceneBoundingRect().center() && temp->endItem->sceneBoundingRect().center() == drawnArrows[index]->endItem->sceneBoundingRect().center()){
                            aux = j;
                        }
                    }
                }
                if (aux!=-1)selectedItems[aux] = nullptr;*/

                delete drawnArrows[index];
                drawnArrows.removeAt(index);
            }


            continue;
        }
        CircleItem *selectedCircle = dynamic_cast<CircleItem*>(item);
        QGraphicsEllipseItem* temp = qgraphicsitem_cast<QGraphicsEllipseItem*>(selectedCircle->ellipse);

        /*Devo eliminare ogni arco coinvolto con il cerchio*/

        /*Elimino gli archi dove il cerchio è la partenza*/

        if (arrows.contains(temp)){
            arrows.remove(temp);
        }

        /*Elimino gli archi dove il cerchio è la destinazione*/

        QMap<QGraphicsEllipseItem*, QList<QGraphicsEllipseItem*>>::const_iterator it;
        /*
         *
        */
        QList<QGraphicsEllipseItem*> toRemove;
        for (it = arrows.constBegin(); it != arrows.constEnd(); ++it) {
            QGraphicsEllipseItem* start = it.key();
            QList<QGraphicsEllipseItem*> end = it.value();
            index = -1;
            for (int i=0; i<end.size(); i++){
                if (end[i] == temp){
                    index = i;
                }
            }
            if (index!=-1){
                arrows[start].removeAt(index);
                if (arrows[start].isEmpty()){
                    toRemove.append(start);
                }

            }



        }
        for(int i=0; i<toRemove.size(); i++){
            arrows.remove(toRemove[i]);
        }
        QList<int> arrowsToRemove;

        // Iterate over the drawnArrows list in reverse order
        for (int i = 0; i < drawnArrows.size(); i++) {
            if (drawnArrows[i]->isMyStartOrEnd(temp)) {
                scene->removeItem(drawnArrows[i]);
                arrowsToRemove.append(i);
            }
        }

        // Delete the items in reverse order
        for (int i = arrowsToRemove.size() - 1; i >= 0; i--) {
            /*index = -1;
            for (int j = 0; j<selectedItems.size(); j++){

                if(selectedItems[j] == nullptr)continue;

                if(dynamic_cast<ArrowItem*>(selectedItems[j]) != nullptr){
                    ArrowItem* temp = static_cast<ArrowItem*>(selectedItems[j]);
                    if (temp->startItem->sceneBoundingRect().center() == drawnArrows[arrowsToRemove[i]]->startItem->sceneBoundingRect().center() && temp->endItem->sceneBoundingRect().center() == drawnArrows[arrowsToRemove[i]]->endItem->sceneBoundingRect().center()){
                        index = j;
                    }
                }
            }
            if (index!=-1){
                selectedItems[index] = nullptr;
            }*/
            delete drawnArrows[arrowsToRemove[i]];
        }
        // Remove the items from the drawnArrows list in reverse order
        for (int i = arrowsToRemove.size() - 1; i >= 0; i--) {
            drawnArrows.removeAt(arrowsToRemove[i]);
        }

        /*Elimino il cerchio dalla lista dei cerchi*/
        index = -1;
        for (int i = 0; i<circles.size(); i++){
            if (*circles[i] == *selectedCircle){
                index = i;
            }
        }
        if (index!=-1)circles.removeAt(index);


        scene->removeItem(selectedCircle);
        delete item;
    }
}
