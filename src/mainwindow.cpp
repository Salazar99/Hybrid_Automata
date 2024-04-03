#include "../include/mainwindow.h"
#include "../include/arrowitem.h"
#include "../include/json.hpp"
#include "./ui_mainwindow.h"
#include <QMouseEvent> // Include the necessary header for QMouseEvent
#include <QtWidgets>
#include <iostream>
#include <fstream>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QMessageBox>
#include <QRandomGenerator>
#include "../include/UtilsJson.h"
#include "../include/tools.h"
#include "../include/csvfile.h"
#include <QKeyEvent>
#include <thread>
#include "switch.h"
#include <algorithm>


using json = nlohmann::json;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    sem_init(&semaforo, 0, 0);
    _count = 0;
    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "Main");
    ui->tabWidget->setTabText(1, "Designer");
    posUpdateButton.append(ui->updateButton->pos().x());
    posUpdateButton.append(ui->updateButton->pos().y());
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    QColor colorBackgroundGrid(200, 200, 200);
    QBrush brush(colorBackgroundGrid, Qt::CrossPattern);
    ui->graphicsView->scene()->setBackgroundBrush(brush);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->frameDebug->hide();
    ui->discardInput->hide();
    ui->stopButton->hide();
    ui->pauseButton->hide();
    ui->showVariables->hide();
    ui->frameDebug->setStyleSheet("border: none;");
    //QString hoverStyle = "background-color: #FF0000;";
    ui->frameDebug->hide();
    ui->horizontalSpacer_2->changeSize(10,20);


    //DEBUG_COMMENT("Questo è un commento di debug" << istanti << " \n\n\n");
    for (double i = 0.1; i < 1.2; i+= 0.05)
        trasparenze.append(i);
    for (double i = 1.2; i > 0; i-= 0.05)
        trasparenze.append(i);
    ct = 0;

    qDebug() << trasparenze;
    timer = new QTimer(this);

    // addEllipse(x,y,w,h,pen,brush)
    // movable text
    ui->graphicsView->installEventFilter(this);
    connect(scene, &QGraphicsScene::selectionChanged, this, &MainWindow::handleSelectionChanged);
    connect(timer, &QTimer::timeout, this, &MainWindow::handleRefresh);
    timer->start(33);

    // Ottenere le dimensioni dello schermo primario
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    qDebug() << "Screen: " << screenWidth << ", " << screenHeight << "\n";

    // Calcolare le nuove dimensioni al 80% dello schermo
    int newWidth = screenWidth *0.8;
    int newHeight = screenHeight * 0.8;


    qDebug() << "New Values: " << newWidth << ", " << newHeight << "\n";
    //rightWidgetWidth+leftWidgetWidth : newWidth =

    this->setFixedSize(newWidth,newHeight);
    ui->deltaSpinBox->setMinimum(0.0001);
    // Impostare le dimensioni del QGraphicsView

    qDebug() << "GraphicsView: " << newWidth*0.799 << ", " << newHeight*0.935 << "\n";
    //ui->frameDataOp->move(newHeight,newWidth);
    ui->graphicsView->setFixedSize(newWidth*0.817, newHeight*0.935);
    hideDesignerInput();

    switchDebug = new Switch("DEBUG MODE");
    ui->debugSpace->addWidget(switchDebug);

    //boolean value for the stop button
    stop = new bool (false);
    pause = new bool (false);
}

MainWindow::~MainWindow()
{
    *stop = true;
    sem_post(&semaforo);
    clearAll(1); //attenzione perchè prima d fare questa si dovrebbe fare un mutex con il secondo thread
    delete ui;
}

void printCircles(QList<CircleItem*> list){
    for (int i=0; i<list.size(); i++){
        qDebug() << *list[i];
    }
}


QColor blendColors(const QColor& baseColor, const QColor& overlayColor, qreal overlayOpacity)
{
    // Calcoliamo l'opacità combinata
    qreal alpha = overlayColor.alphaF() * overlayOpacity;

    // Calcoliamo i componenti RGBA combinati
    int red = baseColor.red() * (1 - alpha) + overlayColor.red() * alpha;
    int green = baseColor.green() * (1 - alpha) + overlayColor.green() * alpha;
    int blue = baseColor.blue() * (1 - alpha) + overlayColor.blue() * alpha;

    // Restituiamo il colore combinato
    return QColor(red, green, blue);
}

void MainWindow::handleRefresh(){
    ui->graphicsView->update();
    QColor shadowColor(0, 0, 0, 128);
    QColor combinedColor;

    if(goalStep==istanti){
        ui->runForButton->setEnabled(true);
        ui->stepButton->setEnabled(true);
        ui->runForButton->setEnabled(true);
        ui->moreSteps->setEnabled(true);
    }

    if(runningStatus){
        if(switchDebug->isChecked())
            ui->currentStep->setText(QString::fromStdString(std::to_string(istanti) + "/" + std::to_string(static_cast<int>(finalTime/delta))));
        else
            ui->currentStep->setText(QString::fromStdString("STEP: "+std::to_string(istanti) + "/" + std::to_string(static_cast<int>(finalTime/delta))));
        //colorando cerchi
        for(int z = 0; z<circles.size(); z++){
            QPen outlinePen(Qt::black);
            outlinePen.setWidth(2);
            circles[z]->ellipse->setBrush(QBrush(automataColors[circles[z]->automata]));
            circles[z]->ellipse->setPen(outlinePen);
        }
        for(int i=0; i< v.size(); i++){
            if(switchDebug->isChecked() && false)//serve per dopo, mancano altri controlli
            {
                tempMap[v[i].getCurrentNodeName()+"~"+v[i].getName()]->setBrush(QBrush(QColor(Qt::red)));
            }
            else{
                combinedColor =  blendColors(automataColors[QString::fromStdString(v[i].getName())], shadowColor, trasparenze[ct%trasparenze.size()]);
                tempMap[v[i].getCurrentNodeName()+"~"+v[i].getName()]->setBrush(QBrush(combinedColor));
            }
        }
        string temp;
        ui->showVariables->clear();
        for (const auto& pair : updateVariables) {
            temp = pair.first + "=" + std::to_string(pair.second);
            ui->showVariables->addItem(QString::fromStdString(temp));
        }
        ct++;
    }

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
        if (event->type() == QEvent::MouseButtonPress && !runningStatus) {
            if (automatas.size() == 0 || ui->automatasList->currentText().isEmpty())return true;
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() != Qt::RightButton)return true;
            QPointF scenePos = ui->graphicsView->mapToScene(mouseEvent->pos());
            QBrush brush(automataColors[ui->automatasList->currentText()]);

            QPen outlinePen(Qt::black);
            outlinePen.setWidth(2);
            QGraphicsEllipseItem *newEllipse = new QGraphicsEllipseItem(scenePos.x(), scenePos.y(), 80, 80);
            newEllipse->setPen(outlinePen);
            newEllipse->setBrush(brush);
            /*newEllipse->setFlag(QGraphicsItem::ItemIsMovable);
            newEllipse->setFlag(QGraphicsItem::ItemIsSelectable);*/
            QGraphicsTextItem *textLabel = new QGraphicsTextItem("default");
            textLabel->setDefaultTextColor(Qt::white);
            textLabel->setFont(QFont("Arial", 10));
            textLabel->setPos(scenePos.x() + 10, scenePos.y() + 10);
            CircleItem *circleItem = new CircleItem(newEllipse, textLabel, ui->automatasList->currentText());
            ellipseMap[newEllipse] = circleItem;
            circleItem->setFlag(QGraphicsItem::ItemIsMovable);
            circleItem->setFlag(QGraphicsItem::ItemIsSelectable);
            bool found = false;
            for (int i = 0; i < circles.size(); i++){
                if (circles[i]->automata == ui->automatasList->currentText())found = true;
            }
            if(!found)
                circleItem->startNode = true;
            circles.append(circleItem);
            scene -> addItem(circleItem);
            qDebug() << "Position of the new circle: " << newEllipse->sceneBoundingRect().center();
            //qDebug() << "Position of the new circle: " << newEllipse->pos().x() << ", " << newEllipse->pos().y() << "\n";
            /*newEllipse->setFlag(QGraphicsItem::ItemIsMovable);
            newEllipse->setFlag(QGraphicsItem::ItemIsSelectable);*/
            qDebug() << "Mouse pressed at scenePos:" << scenePos.x() << ", " << scenePos.y() << "\n";
            hideDesignerInput();
            return true; // Consume the event
        }
        else if(event->type() == QEvent::Wheel) {
            qDebug("ciao");
            QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
            if (wheelEvent->modifiers() & Qt::ControlModifier) {
                if (wheelEvent->angleDelta().y() > 0) {

                    ui->graphicsView->scale(1.1, 1.1);
                } else {
                    ui->graphicsView->scale(0.9, 0.9);
                }
                return true;
            }
        }
        else if (event->type() == QEvent::KeyPress && !runningStatus) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace) {
                deleteSelectedItems();
                hideDesignerInput();
                return true; // Consume the event
            }else if(keyEvent->key() == Qt::Key_K){
                if (selectedCircle1 && selectedCircle2 && scene->selectedItems().size()==2 && checkSelected()) {
                    if (ellipseMap[selectedCircle1]->automata != ellipseMap[selectedCircle2]->automata )
                        return true;
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
            else if(keyEvent->key() == Qt::Key_Escape){
                scene->clearSelection();
                hideDesignerInput();
            }
            else if(keyEvent->key() == Qt::Key_C){ //clear all
                clearAll(0);
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}


void MainWindow::clearAll(int mode){

    scene->clearSelection();
    QMessageBox msgBox;
    int reply = -1;

    if(mode == 0){
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Attention!");
        msgBox.setText("Do you want to delete all?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        reply = msgBox.exec();
    }

    if (reply == QMessageBox::Yes || mode == 1) {
        arrows.clear();
        ellipseMap.clear();
        automataColors.clear();
        automatas.clear();
        ui->automatasList->clear();
        for (int i = 0; i < drawnArrows.size(); i++){
            //delete drawnArrows[i];
            scene->removeItem(drawnArrows[i]);
        }
        drawnArrows.clear();
        for (int i = 0; i < circles.size(); i++){

            scene->removeItem(circles[i]->ellipse);
            delete circles[i];
        }
        circles.clear();
        variablesValues.clear();
        actualVariables.clear();
        hideDesignerInput();
    }
}




void MainWindow::handleSelectionChanged(){
    // Get selected items
    QList<QGraphicsItem*> selectedItems = scene->selectedItems();
    /*
    a sel1
    b sel2
    c

    */


    for (int i = 0; i < circles.size(); i++){
        QBrush brush(automataColors[circles[i]->automata]);
        circles[i]->ellipse->setBrush(brush);
    }
    for (int i = 0; i < selectedItems.size(); i++){
        if (selectedItems[i]->type() == CIRCLEITEM_TYPE){
            CircleItem *selectedCircle = dynamic_cast<CircleItem*>(selectedItems[i]);
            QColor shadowColor(0, 0, 0, 128); // Black color with 50% alpha
            QColor combinedColor = blendColors(automataColors[selectedCircle->automata], shadowColor, 0.6); // 50% opacity
            selectedCircle->ellipse->setBrush(QBrush(combinedColor));
        }
    }
    if (selectedItems.size() == 0){
        ui->automatasList->setCurrentIndex(-1);
        selectedCircle1 = nullptr;
        selectedCircle2 = nullptr;
        ascendingSelection = true;
        isCircleSelected = false;
        ui->valueLabel->setText("");
        ui->nameLabel->setText("");
        ui->descriptionLabel->setText("");
        ui->startCheckBox->setChecked(false);
        hideDesignerInput();
    }

    if (selectedItems.size() == 1){
        if (selectedItems[0]->type() == CIRCLEITEM_TYPE){
            isCircleSelected = true;
            CircleItem *selectedCircle = dynamic_cast<CircleItem*>(selectedItems[0]);
            ui->automatasList->setCurrentIndex(automatas.indexOf(selectedCircle->automata));
            if (selectedCircle == nullptr)qDebug() << "null\n";
            ui->valueLabel->setText(selectedCircle->textItem->toPlainText());
            ui->nameLabel->setText(selectedCircle->name);
            ui->descriptionLabel->setText(selectedCircle->description);
            if (selectedCircle->startNode){
                ui->startCheckBox->setChecked(true);
            }else
                ui->startCheckBox->setChecked(false);
            QGraphicsEllipseItem* temp = selectedCircle->ellipse;

            selectedCircle1 = qgraphicsitem_cast<QGraphicsEllipseItem*>(selectedCircle->ellipse);

            showDesignerInput(0);


        }else{
            ui->startCheckBox->setChecked(false);
            ui->nameLabel->setText("");
            ui->descriptionLabel->setText("");
            isCircleSelected = false;
            selectedArrow = dynamic_cast<ArrowItem*>(selectedItems[0]);
            ui->valueLabel->setText(selectedArrow->textItem->toPlainText());

            showDesignerInput(1);

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
        ascendingSelection = false;
        hideDesignerInput();
    }

    if(selectedItems.size() >= 3){
        ascendingSelection = false;
        hideDesignerInput();
    }

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

                //
                drawnArrows[index]->textItem->setVisible(false);
                drawnArrows[index]->setVisible(false);
                scene->removeItem(drawnArrows[index]);
                //delete drawnArrows[index];
                ui->graphicsView->invalidateScene();
                ui->graphicsView->update();
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

                drawnArrows[index]->textItem->setVisible(false);
                drawnArrows[index]->setVisible(false);

                scene->removeItem(drawnArrows[index]);
                //delete drawnArrows[index];
                ui->graphicsView->invalidateScene();
                ui->graphicsView->update();
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
                drawnArrows[i]->textItem->setVisible(false);
                drawnArrows[i]->setVisible(false);
                scene->removeItem(drawnArrows[i]);
                ui->graphicsView->invalidateScene();
                ui->graphicsView->update();
                arrowsToRemove.append(i);
            }
        }

        // Delete the items in reverse order
        for (int i = arrowsToRemove.size() - 1; i >= 0; i--) {
            //scene->removeItem(drawnArrows[arrowsToRemove[i]]);
            //delete drawnArrows[arrowsToRemove[i]];
        }
        // Remove the items from the drawnArrows list in reverse order
        for (int i = arrowsToRemove.size() - 1; i >= 0; i--) {
            drawnArrows.removeAt(arrowsToRemove[i]);
        }
        ellipseMap.remove(temp);
        int countCirclesAutomata = 0;
        for (int i = 0; i<circles.size(); i++){
            if (circles[i]->automata == selectedCircle->automata)countCirclesAutomata++;
        }
        if (countCirclesAutomata<2){
            automataColors.remove(selectedCircle->automata);
            int pos;
            for (int x = 0; x < automatas.size(); x++)
            {
                if (automatas[x]==selectedCircle->automata){
                    pos = x;
                }
            }
            automatas.remove(pos);
            ui->automatasList->clear();
            ui->automatasList->addItems(automatas);

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
        //delete item;
    }
    scene->selectedItems().clear();
    scene->clearSelection();
}

void MainWindow::on_updateButton_clicked()
{
    QList<QGraphicsItem*> selectedItems = scene->selectedItems();
    bool error = false;
    int pos = 0;
    if (selectedItems.size() == 1){
        if (isCircleSelected){
            CircleItem *selectedCircle = dynamic_cast<CircleItem*>(selectedItems[0]);
            QGraphicsEllipseItem* ellipse = selectedCircle->ellipse;
            if (ellipse->sceneBoundingRect().center() == selectedCircle1->sceneBoundingRect().center()){
                for (int i = 0; i < circles.size(); i++){
                    if (circles[i] == selectedCircle){
                        if(ui->valueLabel->text().count(";") != ui->valueLabel->text().count("=") || ui->valueLabel->text().count(";") == 0){
                            QMessageBox::information(nullptr, "Warning", "Typo in current intructions");
                            error = true;
                            break;
                        }
                        circles[i]->textItem->setPlainText(ui->valueLabel->text());
                        for(int j=0; j<circles.size(); j++){
                            if(circles[j]->automata == circles[i]->automata && i!=j){
                                if(circles[j]->name == ui->nameLabel->text()){
                                    QMessageBox::information(nullptr, "Warning", "A node with this name already exists");
                                    return;
                                }
                            }
                        }
                        circles[i]->name = ui->nameLabel->text();
                        circles[i]->description = ui->descriptionLabel->text();
                        circles[i]->startNode = ui->startCheckBox->isChecked();
                        qDebug() << circles[i]->startNode << "\n";
                        pos = i;
                    }
                }
                for(int j = 0; j < circles.size(); j++){
                    if (ui->startCheckBox->isChecked() && circles[j]->automata == selectedCircle->automata && j!=pos)
                        circles[j]->startNode = false;
                }
                selectedCircle->setSelected(false);
            }
        }else{
            for (int i = 0; i<drawnArrows.size(); i++){
                if (drawnArrows[i] == selectedArrow){
                    if(!(ui->valueLabel->text().startsWith("(") && ui->valueLabel->text().endsWith(")") &&
                        (ui->valueLabel->text().count("(") == ui->valueLabel->text().count(")")))){
                        QMessageBox::information(nullptr, "Warning", "Typo in current conditions");
                        error = true;
                        break;
                    }
                    drawnArrows[i]->textItem->setPlainText(ui->valueLabel->text());
                }
            }
        }
    }
    if(!error){
        ui->valueLabel->setText("");
        ui->nameLabel->setText("");
        ui->descriptionLabel->setText("");
        ui->startCheckBox->setChecked(false);
        scene->clearSelection();
    }

    qDebug() << "UPDATE\n";
}

bool isNumeric(const QString& str) {
    QRegularExpression regex("[+-]?[0-9]+(\\.[0-9]+)?");
    return str.contains(regex);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{

    //x,y
    //x,c

    //variablesValues
    if(index == 1)
        return;
    //qDebug() << "ciao";
    QList<QString> separator;
    separator << "+" << "-" << "*" << "/" << "'" << ";" << "(" << ")" << "[" << "]" << "=";
    QList<QString> functions;
    functions << "log" << "exp" << "sqrt" << "cos" << "sin" << "ln" << "tan" << "arctan" << "cosin" << "cotan";
    actualVariables.clear();
    QString temp;
    QString tempVar;
    for (int i = 0; i < circles.size(); i++){ //ciclo i cerchi
        temp = circles[i]->textItem->toPlainText();
        tempVar.clear();

        qDebug() << "temp: " << temp <<"\n" << "tempVar: " << tempVar << "\n";
        temp.erase(std::remove(temp.begin(), temp.end(), ' '), temp.end());
        for (int j = 0; j < temp.length(); j++){
            if(!separator.contains(temp[j])){
                tempVar += temp[j];
            }
            else{
                if(functions.contains(tempVar)){ //siamo in una funzione matematica
                    tempVar.clear();
                }
                else{ //ho finito di trovare la variabile
                    if(!isNumeric(tempVar) && !tempVar.isEmpty() && !actualVariables.contains(tempVar)){
                        actualVariables.append(tempVar);
                        if(!variablesValues.contains(tempVar)){
                            variablesValues[tempVar] = "NaN";
                        }
                    }
                    tempVar.clear();
                }
            }
        }
    }

    QList<QString> variablesToRemove;
    QMap<QString,QString>::const_iterator it;
    for (it = variablesValues.constBegin(); it != variablesValues.constEnd(); ++it) {
        QString start = it.key();
        if(!actualVariables.contains(start)){
            variablesToRemove.append(start);
        }
    }

    for(QString toRemove : variablesToRemove){
        variablesValues.remove(toRemove);
    }

    ui->selectVariable->clear();
    ui->selectVariable->addItems(actualVariables);

    temp.clear();
    ui->listVariables->clear();
    for(QString toAdd : actualVariables){
        temp = toAdd + "=" + variablesValues[toAdd];
        ui->listVariables->addItem(temp);
    }

    qDebug() << variablesValues;

}

void MainWindow::on_updtateVariable_clicked()
{
    QString temp;
    variablesValues[ui->selectVariable->currentText()] = QString::number(ui->variableValue->value());
    ui->listVariables->clear();
    for(QString toAdd : actualVariables){
        temp = toAdd + "=" + variablesValues[toAdd];
        ui->listVariables->addItem(temp);
    }

}


std::string replaceCommasWithPeriods(const std::string& input) {
    std::string result = input;
    for (char& c : result) {
        if (c == ',') {
            c = '.';
        }
    }
    return result;
}

void MainWindow::hideDesignerInput(){
    ui->valueLabel->setVisible(false);
    ui->istruction->setVisible(false);
    ui->name->setVisible(false);
    ui->nameLabel->setVisible(false);
    ui->description->setVisible(false);
    ui->descriptionLabel->setVisible(false);
    ui->startCheckBox->setVisible(false);
    ui->updateButton->setVisible(false);
}

void MainWindow::setEditStatus(bool mode){
    QString stringa = "IMPORT INPUT FILE";
    ui->loadData->setVisible(mode);
    ui->saveData->setVisible(mode);
    if(mode == true && ui->inputFileButton->text()!=stringa || mode == false)
        ui->discardInput->setVisible(mode);
    ui->inputFileButton->setVisible(mode);
    ui->automatasList->setEnabled(mode);
    ui->valueLabel->setEnabled(mode);
    ui->nameLabel->setEnabled(mode);
    ui->descriptionLabel->setEnabled(mode);
    ui->startCheckBox->setEnabled(mode);
    ui->updateButton->setEnabled(mode);
    ui->automataName->setEnabled(mode);
    ui->addAutoma->setEnabled(mode);
    ui->selectVariable->setEnabled(mode);
    ui->variableValue->setEnabled(mode);
    ui->updtateVariable->setEnabled(mode);
    ui->finalTimeSpinBox->setEnabled(mode);
    ui->deltaSpinBox->setEnabled(mode);
    ui->jsonButton->setEnabled(mode);
    switchDebug->setEnabled(mode);
    ui->loadData->setEnabled(mode);
    ui->saveData->setEnabled(mode);


}

void MainWindow::showDesignerInput(int mode){
    if(mode == 0){//modalità cerchio selezionato
        ui->valueLabel->setVisible(true);
        ui->istruction->setVisible(true);
        ui->istruction->setText("Instructions");
        ui->name->setVisible(true);
        ui->nameLabel->setVisible(true);
        ui->description->setVisible(true);
        ui->descriptionLabel->setVisible(true);
        ui->startCheckBox->setVisible(true);
        ui->updateButton->setVisible(true);
        ui->updateButton->move(posUpdateButton[0],posUpdateButton[1]);
    }
    else{
        ui->valueLabel->setVisible(true);
        ui->istruction->setVisible(true);
        ui->istruction->setText("Conditions");
        ui->name->setVisible(false);
        ui->nameLabel->setVisible(false);
        ui->description->setVisible(false);
        ui->descriptionLabel->setVisible(false);
        ui->startCheckBox->setVisible(false);
        ui->updateButton->setVisible(true);
        ui->updateButton->move(60,ui->nameLabel->pos().y());
    }
}

void MainWindow::runIt(int mode, string path){
    json systemData;
    json globalData;
    json automataArray;
    json automatonData;
    json nodeArray;
    json transitionsArray;
    json transitionData;
    json variablesArray;
    json variableData;
    *stop = false;
    *pause = false;

    setlocale(LC_ALL, "C");

    QMap<std::string,QGraphicsEllipseItem*> mappetta;

    int debugMode = switchDebug->isChecked()  ? 1 : 0;
    if(debugMode){ //switch to design page
        ui->tabWidget->setCurrentIndex(1);
        *pause = true;
    }
    ui->showVariables->setVisible(true);
    string tempAux = ui->deltaSpinBox->text().toStdString();
    globalData["delta"] = replaceCommasWithPeriods(tempAux);
    globalData["finaltime"] = ui->finalTimeSpinBox->text().toStdString();



    transitionsArray.clear();
    for (int x = 0; x < automatas.size(); x++){
        for (int i = 0; i < circles.size(); i++){
            if (circles[i]->automata == automatas[x]){
                json nodeData;
                nodeData["name"] = circles[i]->name.toStdString();
                nodeData["description"] = circles[i]->description.toStdString();
                nodeData["instructions"] = circles[i]->textItem->toPlainText().toStdString();
                nodeData["x"] = circles[i]->ellipse->sceneBoundingRect().center().x();
                nodeData["y"] = circles[i]->ellipse->sceneBoundingRect().center().y();
                if (circles[i]->startNode)
                    nodeData["flag"] = "start";
                else
                    nodeData["flag"] = "none";
                QList<QGraphicsEllipseItem*> destinations = arrows[circles[i]->ellipse];
                for (int j = 0; j<destinations.size(); j++){
                    for (int x = 0; x<circles.size(); x++){
                        if (circles[x]->ellipse->sceneBoundingRect().center() == destinations[j]->sceneBoundingRect().center()){
                            transitionData["to"] = circles[x]->name.toStdString();
                            break;
                        }
                    }

                    for (int x = 0; x<drawnArrows.size(); x++){
                        if (drawnArrows[x]->startItem->sceneBoundingRect().center() == circles[i]->ellipse->sceneBoundingRect().center() &&
                            drawnArrows[x]->endItem->sceneBoundingRect().center() == destinations[j]->sceneBoundingRect().center()){
                            transitionData["condition"] = drawnArrows[x]->textItem->toPlainText().toStdString();
                            break;
                        }
                    }
                    transitionsArray.push_back(transitionData);
                }
                nodeData["transitions"] = transitionsArray;
                nodeArray.push_back(nodeData);
                transitionsArray.clear();
            }
        }
        variablesArray.clear();
        if (x == 0){
            QMap<QString, QString>::const_iterator it;
            for (it = variablesValues.constBegin(); it != variablesValues.constEnd(); ++it) {
                json variableData;
                variableData["name"] = it.key().toStdString();
                variableData["value"] = it.value().toStdString();
                variablesArray.push_back(variableData);
            }
        }
        automatonData["name"] = automatas[x].toStdString();
        automatonData["node"] = nodeArray;
        automatonData["variables"] = variablesArray;
        automataArray.push_back(automatonData);
        nodeArray.clear();
    }

    /*for (int i = 0; i<circles.size(); i++){

        json nodeData;
        nodeData["name"] = circles[i]->name.toStdString();
        nodeData["description"] = circles[i]->description.toStdString();
        nodeData["instructions"] = circles[i]->textItem->toPlainText().toStdString();
        if (circles[i]->startNode)
            nodeData["flag"] = "start";
        else
            nodeData["flag"] = "none";
        QList<QGraphicsEllipseItem*> destinations = arrows[circles[i]->ellipse];
        for (int j = 0; j<destinations.size(); j++){
            for (int x = 0; x<circles.size(); x++){
                if (circles[x]->ellipse->sceneBoundingRect().center() == destinations[j]->sceneBoundingRect().center()){
                    transitionData["to"] = circles[x]->name.toStdString();
                    break;
                }
            }

            for (int x = 0; x<drawnArrows.size(); x++){
                if (drawnArrows[x]->startItem->sceneBoundingRect().center() == circles[i]->ellipse->sceneBoundingRect().center() &&
                    drawnArrows[x]->endItem->sceneBoundingRect().center() == destinations[j]->sceneBoundingRect().center()){
                    transitionData["condition"] = drawnArrows[x]->textItem->toPlainText().toStdString();
                    break;
                }
            }
            transitionsArray.push_back(transitionData);
        }
        nodeData["transitions"] = transitionsArray;
        nodeArray.push_back(nodeData);
        transitionsArray.clear();

    }*/
    json test;
    systemData["global"] = globalData;
    systemData["automata"] = automataArray;
    test["system"] = systemData;
    std::string jsonData = test.dump(4);
    qDebug() << jsonData;

    std::ofstream outFile(path);

    if (outFile.is_open()) {
        outFile << jsonData;
        outFile.close();
        qDebug() << "JSON data has been written to output.json\n";
    } else {
        qDebug() << "Error: Unable to open output file\n";
    }

    if(mode){ //savataggio e basta
        return;
    }

    std::vector<string> auxVar;
    std::ifstream fileTemp(inputFile.toStdString());
    std::ifstream file(inputFile.toStdString());
    std::ifstream fileLookahead(inputFile.toStdString());
    std::string tempStringa;
    std::getline(file, tempStringa);
    std::getline(fileLookahead, tempStringa);
    std::getline(fileLookahead, tempStringa);
    double startInputTime;
    double deltaSim;
    char separator = ',';
    if (inputFile != "void"){


        if (!fileTemp.is_open()) {
            //std::cerr << "Failed to open file: " << inputFile << std::endl;
            return;
        }

        //times, x, a

        std::string line;
        if (std::getline(fileTemp, line)) {
            std::istringstream iss(line);
            std::vector<std::string> cells;
            std::string cell;
            int count = 0;
            if (line.find(';')!=string::npos)
                separator = ';';
            while (std::getline(iss, cell, separator)) {
                auxVar.push_back(cell);
            }
            std::cout << std::endl;
        } else {
            //std::cerr << "File is empty: " << inputFile << std::endl;
        }


        vector<string> tempValue;
        if (std::getline(fileTemp, line)){ //prima riga di dati
            tempValue = split_string(line, separator);
            setlocale(LC_ALL, "C");
            startInputTime = stod(tempValue[0]);
        }
        if (std::getline(fileTemp, line)){ //seconda riga di dati
            tempValue = split_string(line, separator);
            setlocale(LC_ALL, "C");
            deltaSim = stod(tempValue[0])-startInputTime;
        }

        fileTemp.close();
    }



    long start = time(NULL);
    UtilsJson j;

    System s = j.ScrapingJson(path);
    finalTime = s.numSeconds;
    delta = s.delta;
    v = s.getAutomata();
    runningStatus = true;
    std::cout << s;



    if(true){
        for(int i=0;i<v.size();i++){
            vector<Node> vettoreNodi = v[i].getNodes();
            for(int j = 0;j<vettoreNodi.size();j++){
                for(int z=0;z<circles.size();z++){
                    if(circles[z]->automata.toStdString() == v[i].getName() && vettoreNodi[j].getName()==circles[z]->name.toStdString()){
                        mappetta[vettoreNodi[j].getName()+"~"+v[i].getName()] = circles[z]->ellipse;
                    }
                }
            }
        }
    }

    istanti = 0;



    try
    {
#ifdef WINDOWS
        csvfile csv("../export.csv", true);
#else
        csvfile csv("../export.csv", true);
#endif
        // throws exceptions!
        csv << "TIMES";
        for (auto const &key : s.getAutomataDependence())
        {
            csv << key.first;
        }
        csv << endrow;
    }
    catch (const exception &ex)
    {
        qDebug() << "Exception was thrown: " << ex.what();
    }
    updateVariables.clear();
    for (const auto& pair : s.getVariables()) {
        updateVariables.insert(pair);
    }
    std::cout <<"DeltaMain: " << s.delta;

    ////////////////
    /// \brief mapVar
    ///
    ///

    bool leggi = false;
    int nextTime = 0;
    unordered_map<string, double> mapVar;
    for(int i=1;i<auxVar.size();i++){ //inizializzo mappa valori a zero
        mapVar[auxVar[i]] = 0;
    }
    for (currentTime = 1; currentTime < s.numSeconds + 1 + 0.000001 - s.delta; currentTime = currentTime + s.delta)
    {
        //qDebug() << "################## TIME = " << currentTime << " ##################\n";
        if(*stop)
            break;
        if(*pause)
            sem_wait(&semaforo);

        // executing all automatas instructions and checking for possible transitions
        bool back = false;
        if (inputFile != "void"){

            if(currentTime+0.000001>=startInputTime){
                std::getline(fileLookahead, tempStringa);
                std::istringstream iss(tempStringa);
                std::vector<std::string> cells;
                std::string cell;

                if (std::getline(iss, cell, separator)) {
                    if(cell != ""){
                        startInputTime = stod(cell);
                        leggi = true;
                    }
                }
                else{
                    leggi = true;
                }
            }

            std::string line;
            if(leggi){ //se devo leggere la next riga csv
                leggi = false;
                if (std::getline(file, line)) {
                    std::istringstream iss(line);
                    std::vector<std::string> cells;
                    std::string cell;
                    int count = 0;
                    while (std::getline(iss, cell, separator)) {
                        if (count == 0){
                            count++;
                            continue;
                        }
                        if(cell != "")
                            mapVar[auxVar[count]] = stod(cell);
                        count++;
                    }
                    std::cout << std::endl;
                } else {
                    //std::cerr << "File is empty: " << inputFile << std::endl;
                }
            }
            for (int j = 0; j < v.size(); j++){
                v[j].currentNode.setFileValues(mapVar);
            }
        }

        for (int j = 0; j < v.size(); j++)
        {
            v[j].checkForChanges();
            // cout << "\nAutoma " << v[j].getName() << " ,Nodo attuale: " << v[j].getCurrentNode().getName() << "\n";
        }

        // refreshing AutomataVariables
        s.refreshVariables();
        //updateVariables.clear();
        for (const auto& pair : s.getVariables()) {
            updateVariables[pair.first] = pair.second;
        }

        QList<string> attuali;
        if(false){
            QColor shadowColor(0, 0, 0, 128); // Black color with 50% alpha
            //colorando cerchi
            for(int z = 0; z<circles.size(); z++){
                if(!attuali.contains(circles[z]->name.toStdString())){ //evito di pulire i nodi attuali
                    QPen outlinePen(Qt::black);
                    outlinePen.setWidth(2);
                    circles[z]->ellipse->setBrush(QBrush(automataColors[circles[z]->automata]));
                    circles[z]->ellipse->setPen(outlinePen);
                }
            }
            attuali.clear();

            for (int j = 0; j < v.size(); j++)
            {
                if(true){
                    QPen outlinePen(QColor(204,255,0));
                    outlinePen.setWidth(3);
                    //QColor combinedColor = blendColors(automataColors[QString::fromStdString(v[j].getName())], shadowColor, trasparenze[ct%trasparenze.size()]); //
                    QBrush tempbrush(QColor(Qt::red));
                    string temps = v[j].getCurrentNodeName();
                    string temps2 = temps + "~";
                    string temps3 = temps2 + v[j].getName();
                    mappetta[temps3]->setBrush(QBrush(QColor(Qt::red)));
                    //mappetta[v[j].getCurrentNode().getName()+"~"+v[j].getName()]->setPen(outlinePen);
                    ct++;
                }
                attuali.append(v[j].getCurrentNodeName());
            }
        }

        //qDebug() << "\nVariables Map: \n";
        //printMap(*v[0].getAutomataVariables());
        try
        {
#ifdef WINDOWS
            csvfile csv("../export.csv", false);
#else
            csvfile csv("../export.csv", false);
#endif
            csv << currentTime; //timestamp
            for (auto const &key : s.getAutomataDependence())
            {

                // if the variable has not already been assigned then i print 0
                if ((*(v[0].getAutomataVariables())).find(key.first) == (*(v[0].getAutomataVariables())).end())
                {
                    csv << 0;
                }
                else
                {
                    csv << *(*(v[0].getAutomataVariables()))[key.first];
                }
            }

            csv << endrow;
        }
        catch (const exception &ex)
        {
            qDebug() << "Exception was thrown: " << ex.what();
        }

        //this_thread::sleep_for(chrono::milliseconds(0));
        istanti++;
        //qDebug() << "\n\n";
    }
    qDebug() << "Total Istanti: " << istanti;
    qDebug() << "\nCi ha messo " << time(NULL) - start << " secondi";

    for(int z = 0; z<circles.size(); z++){
        QPen outlinePen(Qt::black);
        outlinePen.setWidth(2);
        circles[z]->ellipse->setBrush(QBrush(automataColors[circles[z]->automata]));
        circles[z]->ellipse->setPen(outlinePen);
    }

    ui->stopButton->hide();
    ui->pauseButton->hide();

    setEditStatus(true);
    runningStatus = false;
    ui->frameDebug->hide();
    ui->commands->show();
    ui->showVariables->hide();
    ui->runForButton->setEnabled(true);
    ui->stepButton->setEnabled(true);
    ui->runForButton->setEnabled(true);
    ui->moreSteps->setEnabled(true);
}

void MainWindow::on_jsonButton_clicked() {

    string path;
#ifdef WINDOWS
    path = "..//output.json";
#else
    path = "../output.json";
#endif

    if(actualVariables.isEmpty()){
        QMessageBox::information(nullptr, "Error", "No variables in the system");
        return;
    }

    bool foundError = false;
    if(circles.empty()){
        foundError = true;
    }
    int count = 0;
    for(int i=0; i<automatas.size(); i++){
        for(int j=0; j<circles.size(); j++){
            if(circles[j]->automata == automatas[i]){
                if(circles[j]->startNode){
                    count++;
                }
            }
        }
        if(count == 0){
            QMessageBox::information(nullptr, "Error", "No Start found in " + automatas[i]);
            return;
        }
        else if(count >1){
            QMessageBox::information(nullptr, "Error", "Multiple start nodes found in " + automatas[i]);
            return;
        }

        count = 0;
    }
    QMap<QString, QString>::const_iterator checkVariablesValues;
    for (checkVariablesValues = variablesValues.constBegin(); checkVariablesValues != variablesValues.constEnd(); ++checkVariablesValues) {
        if (checkVariablesValues.value().toStdString() == "NaN"){
            std::string error = "You can't run the system because the variable '" + checkVariablesValues.key().toStdString() + "' hasn't a initial value!";
            QString qError = QString::fromStdString(error); // Convert std::string to QString
            QMessageBox::information(nullptr, "Warning", qError);
            foundError = true;
        }
    }
    if (foundError){
        QMessageBox::information(nullptr, "Warning", "Check your system, something went wrong");
        return;
    }

    setEditStatus(false);
    tempMap.clear();

    for(int k = 0; k<circles.size();k++){
        QString t = circles[k]->name+"~"+(circles[k]->automata);
        tempMap[t.toStdString()] = circles[k]->ellipse;
    }

    ui->frameDebug->show();
    ui->commands->hide();
    ui->pauseButton->setText("PAUSE");
    ui->moreSteps->setValue(0);
    sem_destroy(&semaforo);
    sem_init(&semaforo, 0, 0);
    std::thread thread_obj(&MainWindow::runIt, this,0,path);
    //thread_obj.join();
    thread_obj.detach(); // Permette al thread di eseguire in background
    if(!switchDebug->isChecked()){
        ui->stopButton->show();
        ui->pauseButton->show();
        ui->stepButton->hide();
        ui->runForButton->hide();
        ui->moreSteps->hide();
    }
    else{
        ui->stopButton->show();
        ui->stepButton->show();
        ui->runForButton->show();
        ui->moreSteps->show();
    }
}



void MainWindow::on_addAutoma_clicked()
{
    if(automatas.size() == 10){
        QMessageBox::information(nullptr, "Information", "Maximum number of automatas reached");
        return;
    }
    if(ui->automataName->text().isEmpty()){
        QMessageBox::information(nullptr, "Information", "Please insert a name for the automa");
        return;
    }
    if(automatas.contains(ui->automataName->text())){
        QMessageBox::information(nullptr, "Information", "An automata with this name already exist");
        return;
    }
    QList<QColor> colors;
    colors << QColor(Qt::cyan)
              << QColor(Qt::magenta)
              << QColor(Qt::red)
              << QColor(Qt::darkRed)
              << QColor(Qt::darkCyan)
              << QColor(Qt::darkMagenta)
              << QColor(255,117,20)
              << QColor(0,102,204)
              << QColor(Qt::gray)
              << QColor(0, 255, 0);

    bool found = true;
    QColor randomColor;
    while(true){
        // Generate a random index within the range of the list
        int randomIndex = QRandomGenerator::global()->bounded(colors.size());

        // Retrieve the color at the random index
        randomColor = colors[randomIndex];
        QMap<QString, QColor>::const_iterator it;
        for (it = automataColors.constBegin(); it != automataColors.constEnd(); ++it) {
            if (it.value() == randomColor){
                found = false;
            }
        }
        if (found)break;
        found = true;
    }

    QString newAutomata = ui->automataName->text();
    automataColors[newAutomata] = randomColor;
    automatas.append(newAutomata);
    ui->automatasList->clear();
    ui->automatasList->addItems(automatas);
    ui->automataName->clear();

}


void MainWindow::on_loadData_clicked()
{
    clearAll(1);
    QString filePath = QFileDialog::getOpenFileName(this, tr("Seleziona un file JSON"), QDir::currentPath(), tr("File JSON (*.json)"));

    if (filePath.isEmpty()) return;

    std::ifstream f(filePath.toStdString());
    json data = json::parse(f);
     setlocale(LC_ALL, "C");
    string h_string = data["system"]["global"]["delta"];
/*
#ifdef WINDOWS
    ;
#else
    replace(h_string.begin(), h_string.end(), '.', ',');
#endif
*/
    double system_delta = stod(h_string);

    ui->deltaSpinBox->setValue(system_delta);

    h_string = data["system"]["global"]["finaltime"];

/*#ifdef WINDOWS
    ;
#else
    replace(h_string.begin(), h_string.end(), '.', ',');
#endif
*/
    double system_finaltime = stod(h_string);

    ui->finalTimeSpinBox->setValue(system_finaltime);

    QList<QColor> colors;
    colors << QColor(Qt::cyan)
           << QColor(Qt::magenta)
           << QColor(Qt::red)
           << QColor(Qt::darkRed)
           << QColor(Qt::darkCyan)
           << QColor(Qt::darkMagenta)
           << QColor(255,117,20)
           << QColor(0,102,204)
           << QColor(Qt::gray)
           << QColor(0, 255, 0);

    // find all the automata in settings.json
    for (json automata : data["system"]["automata"]){
        automatas.append(QString::fromStdString(automata["name"]));
        bool found = true;
        QColor randomColor;
        while(true){
            // Generate a random index within the range of the list
            int randomIndex = QRandomGenerator::global()->bounded(colors.size());

            // Retrieve the color at the random index
            randomColor = colors[randomIndex];
            QMap<QString, QColor>::const_iterator it;
            for (it = automataColors.constBegin(); it != automataColors.constEnd(); ++it) {
                if (it.value() == randomColor){
                    found = false;
                }
            }
            if (found)break;
            found = true;
        }
        automataColors[QString::fromStdString(automata["name"])] = randomColor;
    }
    ui->automatasList->addItems(automatas);
    QMap<QString, QGraphicsEllipseItem*> mappetta;
    for (json automata : data["system"]["automata"]){
        for (json node : automata["node"])
        {
            QPointF point(node["x"],node["y"]);
            QBrush brush(automataColors[QString::fromStdString(automata["name"])]);
            QPen outlinePen(QColor(0, 0, 0));
            outlinePen.setWidth(2);
            QGraphicsEllipseItem *newEllipse = new QGraphicsEllipseItem(point.x(), point.y(), 80, 80);
            newEllipse->setPen(outlinePen);
            newEllipse->setBrush(brush);
            string temp = node["instructions"];
            QGraphicsTextItem *textLabel = new QGraphicsTextItem(temp.c_str());
            textLabel->setDefaultTextColor(Qt::white);
            textLabel->setFont(QFont("Arial", 10));
            textLabel->setPos(point.x() + 10, point.y() + 10);
            CircleItem *circleItem = new CircleItem(newEllipse, textLabel, QString::fromStdString(automata["name"]));
            ellipseMap[newEllipse] = circleItem;
            circleItem->setFlag(QGraphicsItem::ItemIsMovable);
            circleItem->setFlag(QGraphicsItem::ItemIsSelectable);
            circleItem->description = QString::fromStdString(node["description"]);
            circleItem->name = QString::fromStdString(node["name"]);
            mappetta[circleItem->name+"~"+(circleItem->automata)] = newEllipse;
            temp = node["flag"];

            if(temp == "start"){
                circleItem->startNode = true;
            }
            else{
                circleItem->startNode = false;
            }

            circles.append(circleItem);
            scene -> addItem(circleItem);
            hideDesignerInput();
        }
    }

    for (json automata : data["system"]["automata"]){
        for (json node : automata["node"])
        {
            for(json transition : node["transitions"]){
                ArrowItem* arrow;
                if (arrows.contains(mappetta[QString::fromStdString(node["name"])+"~"+(QString::fromStdString(automata["name"]))])){
                    QList<QGraphicsEllipseItem*> dest = arrows[mappetta[QString::fromStdString(node["name"])+"~"+(QString::fromStdString(automata["name"]))]];
                    if (!dest.contains(mappetta[QString::fromStdString(transition["to"])+"~"+(QString::fromStdString(automata["name"]))])){
                        arrow = new ArrowItem(mappetta[QString::fromStdString(node["name"])+"~"+(QString::fromStdString(automata["name"]))], mappetta[QString::fromStdString(transition["to"])+"~"+(QString::fromStdString(automata["name"]))]);
                        arrow->setFlag(QGraphicsItem::ItemIsSelectable);
                        arrow->textItem->setPlainText(QString::fromStdString(transition["condition"]));
                        scene->addItem(arrow);
                        drawnArrows.append(arrow);
                        arrows[mappetta[QString::fromStdString(node["name"])+"~"+(QString::fromStdString(automata["name"]))]].append(mappetta[QString::fromStdString(transition["to"])+"~"+(QString::fromStdString(automata["name"]))]);
                    }
                }else{
                    arrow = new ArrowItem(mappetta[QString::fromStdString(node["name"])+"~"+(QString::fromStdString(automata["name"]))], mappetta[QString::fromStdString(transition["to"])+"~"+(QString::fromStdString(automata["name"]))]);
                    arrow->setFlag(QGraphicsItem::ItemIsSelectable);
                    arrow->textItem->setPlainText(QString::fromStdString(transition["condition"]));
                    scene->addItem(arrow);
                    drawnArrows.append(arrow);
                    arrows.insert(mappetta[QString::fromStdString(node["name"])+"~"+(QString::fromStdString(automata["name"]))], QList<QGraphicsEllipseItem*>());
                    arrows[mappetta[QString::fromStdString(node["name"])+"~"+(QString::fromStdString(automata["name"]))]].append(mappetta[QString::fromStdString(transition["to"])+"~"+(QString::fromStdString(automata["name"]))]);
                }

            }
        }
        for(json variables : automata["variables"]){
            variablesValues[QString::fromStdString(variables["name"])] = QString::fromStdString(variables["value"]);
            actualVariables.append(QString::fromStdString(variables["name"]));
        }
    }


    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_listVariables_itemDoubleClicked(QListWidgetItem *item)
{
    QString testo = item->text().split('=')[0];
    int index = actualVariables.indexOf(item->text());
    ui->selectVariable->setCurrentIndex(actualVariables.indexOf(item->text().split('=')[0]));
    ui->variableValue->setValue(ui->listVariables->currentItem()->text().split('=')[1].toDouble());
}


void MainWindow::on_selectVariable_currentIndexChanged(int index)
{
    ui->listVariables->setCurrentRow(index);
}


void MainWindow::on_saveData_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Salva il file JSON"), QDir::currentPath(), tr("File JSON (*.json)"));

    if (filePath.isEmpty()) {
        qDebug() << "File selezionato per il salvataggio: " << filePath;
        return;
    }
    runIt(1,filePath.toStdString());
}


void MainWindow::on_pauseButton_clicked()
{
    if(ui->pauseButton->text() == "PAUSE"){
        *pause = true;
        ui->pauseButton->setText("RESUME");
        //ui->stopButton->setEnabled(false);
    }
    else{
        sem_post(&semaforo);
        *pause = false;
        ui->pauseButton->setText("PAUSE");
        ui->stopButton->setEnabled(true);
    }


}

void MainWindow::on_stopButton_clicked()
{
    *stop = true;
    sem_post(&semaforo);
    ui->showVariables->hide();
}

void MainWindow::runDebuggingSteps(int steps){

    for(int i=0; i<steps; i++){
        sem_post(&semaforo);
    }
}

void MainWindow::on_runForButton_clicked()
{
    QString number = ui->moreSteps->text();
    bool ciao;
    int steps = number.toInt(&ciao);
    /*
    0.01
    50
    51-0.01-0.01)-currentTime
    */

    int left = (((finalTime + 1)-currentTime)/delta)+1;
    ui->moreSteps->setValue(std::min(steps, left-1));
    goalStep = istanti + steps;
    ui->runForButton->setEnabled(false);
    ui->stepButton->setEnabled(false);
    ui->runForButton->setEnabled(false);
    ui->moreSteps->setEnabled(false);
    qDebug() << "Numero di step rimanenti: " << left;
    std::thread thread_obj(&MainWindow::runDebuggingSteps, this, std::min(steps, left));
    thread_obj.detach(); // Permette al thread di eseguire in background
}


void MainWindow::on_stepButton_clicked()
{
    sem_post(&semaforo);
}





void MainWindow::on_inputFileButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Seleziona un file CSV"), QDir::currentPath(), tr("File CSV (*.csv)"));
    inputFile = filePath;
    if (filePath.isEmpty()) return;
    std::ifstream f(filePath.toStdString());
    ui->inputFileButton->setText(filePath.split('/').last());
    ui->discardInput->show();
}


void MainWindow::on_discardInput_clicked()
{
    ui->discardInput->hide();
    ui->inputFileButton->setText("IMPORT INPUT FILE");
    inputFile = "void";
}

