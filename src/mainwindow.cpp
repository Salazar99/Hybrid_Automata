#include "../include/global_variables.h"
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


#include "../include/UtilsJson.h"
#include "../include/tinyexpr.h"
#include "../include/tools.h"

#include "../include/csvfile.h"

using json = nlohmann::json;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "Main");
    ui->tabWidget->setTabText(1, "Designer");
    posUpdateButton.append(ui->updateButton->pos().x());
    posUpdateButton.append(ui->updateButton->pos().y());
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    QColor colorBackgroundGrid(200, 200, 200);
    QBrush brush(colorBackgroundGrid, Qt::CrossPattern);
    ui->graphicsView->scene()->setBackgroundBrush(brush);


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

    ui->graphicsView->setFixedSize(newWidth*0.817, newHeight*0.935);

    hideDesignerInput();

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
            QColor color(77, 77, 77);
            QBrush brush(color);
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
            CircleItem *circleItem = new CircleItem(newEllipse, textLabel);
            circleItem->setFlag(QGraphicsItem::ItemIsMovable);
            circleItem->setFlag(QGraphicsItem::ItemIsSelectable);
            if(circles.isEmpty())
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
        else if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace) {
                deleteSelectedItems();
                hideDesignerInput();
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
            else if(keyEvent->key() == Qt::Key_Escape){
                scene->clearSelection();
                hideDesignerInput();
            }
            else if(keyEvent->key() == Qt::Key_P){
                if(dragMode)
                    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
                else
                    ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
                dragMode = !dragMode;
                QList<QGraphicsItem*> selectedItems = scene->selectedItems();
                if(dragMode && selectedItems.size()==1)
                    showDesignerInput(0);
                else
                    hideDesignerInput();
            }
            else if(keyEvent->key() == Qt::Key_C){ //clear all

                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.setWindowTitle("Attention!");
                msgBox.setText("Do you want to delete all?");
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::Yes);

                int reply = msgBox.exec();

                if (reply == QMessageBox::Yes) {
                    arrows.clear();
                    for (int i = 0; i < drawnArrows.size(); i++){
                        delete drawnArrows[i];
                    }
                    drawnArrows.clear();
                    for (int i = 0; i < circles.size(); i++){
                        delete circles[i];
                    }
                    circles.clear();
                    hideDesignerInput();
                }
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
    QColor color(77, 77, 77);
    QBrush brush(color);
    for (int i = 0; i < circles.size(); i++){

        circles[i]->ellipse->setBrush(brush);
    }
    for (int i = 0; i < selectedItems.size(); i++){
        if (selectedItems[i]->type() == CIRCLEITEM_TYPE){
            CircleItem *selectedCircle = dynamic_cast<CircleItem*>(selectedItems[i]);
            selectedCircle->ellipse->setBrush(QBrush(Qt::cyan));
        }
    }
    if (selectedItems.size() == 0){
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

void MainWindow::on_updateButton_clicked()
{
    QList<QGraphicsItem*> selectedItems = scene->selectedItems();
    bool error = false;
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
                        circles[i]->name = ui->nameLabel->text();
                        circles[i]->description = ui->descriptionLabel->text();
                        circles[i]->startNode = ui->startCheckBox->isChecked();
                        selectedCircle->setSelected(false);
                        qDebug() << circles[i]->startNode << "\n";
                    }else{
                        if (ui->startCheckBox->isChecked())
                            circles[i]->startNode = false;
                    }
                }
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
    QRegularExpression regex("[+-]?[0-9]+(\.[0-9]+)?");
    return str.contains(regex);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{

    //x,y
    //x,c

    //variablesValues
    if(index == 1)
        return;
    qDebug() << "ciao";
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

void MainWindow::showDesignerInput(int mode){
    if(mode == 0){//modalità cerchio
        ui->valueLabel->setVisible(true);
        ui->istruction->setVisible(true);
        ui->istruction->setText("Istructions");
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

void MainWindow::on_jsonButton_clicked()
{
    json systemData;
    json globalData;
    json automataArray;
    json automatonData;
    json nodeArray;
    json transitionsArray;
    json transitionData;
    json variablesArray;
    json variableData;

    string tempAux = ui->deltaSpinBox->text().toStdString();
    globalData["delta"] = replaceCommasWithPeriods(tempAux);
    globalData["finaltime"] = ui->finalTimeSpinBox->text().toStdString();
    bool foundError = false;
    QMap<QString, QString>::const_iterator checkVariablesValues;
    for (checkVariablesValues = variablesValues.constBegin(); checkVariablesValues != variablesValues.constEnd(); ++checkVariablesValues) {
        if (checkVariablesValues.value().toStdString() == "NaN"){
            std::string error = "You can't run the system because the variable '" + checkVariablesValues.key().toStdString() + "' hasn't a initial value!";
            QString qError = QString::fromStdString(error); // Convert std::string to QString
            QMessageBox::information(nullptr, "Warning", qError);
            foundError = true;
        }
    }
    if (foundError)return;

    for (int i = 0; i<circles.size(); i++){

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

    }

    QMap<QString, QString>::const_iterator it;
    for (it = variablesValues.constBegin(); it != variablesValues.constEnd(); ++it) {
        json variableData;
        variableData["name"] = it.key().toStdString();
        variableData["value"] = it.value().toStdString();
        variablesArray.push_back(variableData);
    }

    automatonData["name"] = "Thermostat";
    automatonData["node"] = nodeArray;
    automatonData["variables"] = variablesArray;
    automataArray.push_back(automatonData);
    json test;
    systemData["global"] = globalData;
    systemData["automata"] = automataArray;
    test["system"] = systemData;
    std::string jsonData = test.dump(4);
    qDebug() << jsonData;

#ifdef WINDOWS
    std::ofstream outFile("../output.json");
#else
    std::ofstream outFile("../output.json");
#endif
    if (outFile.is_open()) {
        outFile << jsonData;
        outFile.close();
        qDebug() << "JSON data has been written to output.json\n";
    } else {
        qDebug() << "Error: Unable to open output file\n";
    }


    long start = time(NULL);
    UtilsJson j;
#ifdef WINDOWS
    System s = j.ScrapingJson("..//output.json");
#else
    System s = j.ScrapingJson("../output.json");
#endif
    vector<Automata> v = s.getAutomata();
    std::cout << s;

    int istanti = 0;

    //DEBUG_COMMENT("Questo è un commento di debug" << istanti << " \n\n\n");

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

    std::cout <<"DeltaMain: " << finaltime[0];

    for (double time = 1; time < finaltime[1] + 1 - finaltime[0]; time = time + finaltime[0])
    {
        qDebug() << "################## TIME = " << time << " ##################\n";

        // executing all automatas instructions and checking for possible transitions
        for (int j = 0; j < v.size(); j++)
        {
            v[j].checkForChanges();
            // cout << "\nAutoma " << v[j].getName() << " ,Nodo attuale: " << v[j].getCurrentNode().getName() << "\n";
        }

        // refreshing AutomataVariables
        s.refreshVariables();

        qDebug() << "\nVariables Map: \n";
        printMap(*v[0].getAutomataVariables());
        try
        {
#ifdef WINDOWS
            csvfile csv("../export.csv", false);
#else
            csvfile csv("../export.csv", false);
#endif
            csv << time; //timestamp
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
        qDebug() << "\n\n";
    }

    qDebug() << "Total Istanti: " << istanti;
    qDebug() << "\nCi ha messo " << time(NULL) - start << " secondi";
}



