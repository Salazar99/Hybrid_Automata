#include "../include/arrowitem.h"
#include <QStyleOptionGraphicsItem> // Include the header for QStyleOptionGraphicsItem
#include <QPainter>
#include <QStyle>
#include <QPainterPath>
ArrowItem::ArrowItem(QGraphicsItem *startItem, QGraphicsItem *endItem, QGraphicsItem *parent)
    : QGraphicsItem(parent), startItem(startItem), endItem(endItem)
{
    setFlag(ItemSendsGeometryChanges);
}

QRectF ArrowItem::boundingRect() const
{
    if (!startItem || !endItem){
        //delete this;
        return QRectF();
    }
    qreal penWidth = 1.0; // Adjust pen width as needed
    qreal extra = 10; // Additional space for the arrow heads
    return QRectF(startItem->sceneBoundingRect().center(),
                  QSizeF(endItem->sceneBoundingRect().center().x() - startItem->sceneBoundingRect().center().x(),
                         endItem->sceneBoundingRect().center().y() - startItem->sceneBoundingRect().center().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

/*
QPainterPath ArrowItem::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(17.5); // Adjust the width of the stroke for padding

    QPainterPath path;
    path.moveTo(startItem->sceneBoundingRect().center());
    path.lineTo(endItem->sceneBoundingRect().center());

    // Apply the stroke to the path to add padding
    QPainterPath paddedPath = stroker.createStroke(path);
    return paddedPath;
}
*/


/*QPainterPath ArrowItem::shape() const
{
    QPainterPath path;
    path.moveTo(startItem->sceneBoundingRect().center());
    path.lineTo(endItem->sceneBoundingRect().center());
    // Include arrowhead in the shape
    qreal arrowSize = 20;
    qreal angle = atan2(endItem->sceneBoundingRect().center().y() - startItem->sceneBoundingRect().center().y(),
                        endItem->sceneBoundingRect().center().x() - startItem->sceneBoundingRect().center().x());
    QPointF arrowP1 = endItem->sceneBoundingRect().center() - QPointF(sin(angle - M_PI / 3) * arrowSize, cos(angle - M_PI / 3) * arrowSize);
    QPointF arrowP2 = endItem->sceneBoundingRect().center() - QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize, cos(angle - M_PI + M_PI / 3) * arrowSize);
    path.lineTo(arrowP1);
    path.moveTo(endItem->sceneBoundingRect().center());
    path.lineTo(arrowP2);
    return path;
}*/

/*
void ArrowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    qDebug() << "Stiamo paintando\n";

    if (!startItem || !endItem){
        qDebug() << "Qualcuno è null!\n";
        return;
    }

    bool isSelected = option->state & QStyle::State_Selected;
    qDebug() << "Paint: " << startItem->sceneBoundingRect().center() << ", " << endItem->sceneBoundingRect().center();
    QPen pen(isSelected ? Qt::red : Qt::black);
    pen.setWidth(2);
    QPointF startHomemade(startItem->sceneBoundingRect().center().x(), startItem->sceneBoundingRect().center().y());
    QPointF endHomemade(endItem->sceneBoundingRect().center().x(), endItem->sceneBoundingRect().center().y());
    painter->setPen(pen);

    qreal arrowSize = 20;
    qreal angle = atan2(endHomemade.y() - startHomemade.y(), endHomemade.x() - startHomemade.x());
    QPointF arrowP1 = endHomemade - QPointF(sin(angle - M_PI / 3) * arrowSize, cos(angle - M_PI / 3) * arrowSize);
    QPointF arrowP2 = endHomemade - QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize, cos(angle - M_PI + M_PI / 3) * arrowSize);

    painter->drawLine(startHomemade, endHomemade);
    painter->drawLine(endHomemade, arrowP1);
    painter->drawLine(endHomemade, arrowP2);
}*/

QPainterPath ArrowItem::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(17.5); // Adjust the width of the stroke for padding

    qreal arrowSize = 20;
    qreal endItemRadius = qMin(endItem->boundingRect().width(), endItem->boundingRect().height()) / 2.0;
    QPointF startHomemade(startItem->sceneBoundingRect().center().x(), startItem->sceneBoundingRect().center().y());
    QPointF endHomemade(endItem->sceneBoundingRect().center().x(), endItem->sceneBoundingRect().center().y());

    qreal angle = atan2(startHomemade.y() - endHomemade.y(), startHomemade.x() - endHomemade.x()) + 0.5 ;
    qreal angle_start = atan2(endHomemade.y() - startHomemade.y(), endHomemade.x() - startHomemade.x()) - 0.5;

    QPointF stopLine = endHomemade + QPointF(endItemRadius*cos(angle),endItemRadius*sin(angle));
    QPointF startLine = startHomemade + QPointF(endItemRadius*cos(angle_start),endItemRadius*sin(angle_start));

    QPointF arrowP1 = QPointF(stopLine.x() - sin(angle - M_PI / 3) * arrowSize, stopLine.y() - cos(angle - M_PI / 3) * arrowSize);
    QPointF arrowP2 = QPointF(stopLine.x() - sin(angle - M_PI + M_PI / 3) * arrowSize, stopLine.y() - cos(angle - M_PI + M_PI / 3) * arrowSize);

    QPointF controlPoint = QPointF();
    // Calcolo del punto di controllo per la curva
    if (abs(startLine.x() - stopLine.x()) < 80) {
        if (startLine.y() - stopLine.y() < 0) {
            controlPoint = QPointF(((startLine.x() + stopLine.x()) / 2) - 80, ((startLine.y() + stopLine.y()) / 2));
        } else {
            controlPoint = QPointF(((startLine.x() + stopLine.x()) / 2) + 80, ((startLine.y() + stopLine.y()) / 2));
        }
    } else if (abs(startLine.y() - stopLine.y()) < 80) {
        if (startLine.x() - stopLine.x() < 0) {
            controlPoint = QPointF((startLine.x() + stopLine.x()) / 2, ((startLine.y() + stopLine.y()) / 2) + 80);
        } else {
            controlPoint = QPointF((startLine.x() + stopLine.x()) / 2, ((startLine.y() + stopLine.y()) / 2) - 80);
        }
    } else {
        if (startLine.y() - stopLine.y() < 0) {
            controlPoint = QPointF(((startLine.x() + stopLine.x()) / 2) - 40, ((startLine.y() + stopLine.y()) / 2) - 40);
        } else {
            controlPoint = QPointF(((startLine.x() + stopLine.x()) / 2) + 40, ((startLine.y() + stopLine.y()) / 2) + 40);
        }
    }


    /*
    QPainterPath path;
    path.moveTo(startLine);
    path.quadTo(controlPoint, stopLine);*/
    // Costruiamo il percorso utilizzando i punti intermedi
    QPainterPath path;
    path.moveTo(startLine);
    for (int i = 1; i < points.size(); ++i) {
        path.lineTo(points[i]);
    }

    return stroker.createStroke(path);
}




void ArrowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    qDebug() << "Stiamo paintando\n";

    if (!startItem || !endItem){
        qDebug() << "Qualcuno Ã¨ null!\n";
        return;
    }

    bool isSelected = option->state & QStyle::State_Selected;
    qDebug() << "Paint: " << startItem->sceneBoundingRect().center() << ", " << endItem->sceneBoundingRect().center();
    QPen pen(isSelected ? Qt::red : Qt::black);
    pen.setWidth(2);
    QPointF startHomemade(startItem->sceneBoundingRect().center().x(), startItem->sceneBoundingRect().center().y());
    QPointF endHomemade(endItem->sceneBoundingRect().center().x(), endItem->sceneBoundingRect().center().y());

    // Calcola il raggio del endItem

    painter->setPen(pen);

    qreal arrowSize = 20;
    qreal angle = atan2(startHomemade.y() - endHomemade.y(), startHomemade.x() - endHomemade.x()) + 0.5;
    qreal angle_start = atan2(endHomemade.y() - startHomemade.y(), endHomemade.x() - startHomemade.x()) - 0.5;

    qreal endItemRadius = qMin(endItem->boundingRect().width(), endItem->boundingRect().height()) / 2.0;

    QPointF stopLine = endHomemade + QPointF(endItemRadius*cos(angle),endItemRadius*sin(angle));
    QPointF startLine = startHomemade + QPointF(endItemRadius*cos(angle_start),endItemRadius*sin(angle_start));

    QPointF arrowP1 = QPointF(stopLine.x()+ 10*cos(angle),stopLine.y()+10*sin(angle));
    QPointF arrowP2 = QPointF(stopLine.x()+ 10*cos(angle-1.3),stopLine.y()+10*sin(angle-1.3));

    QPointF controlPoint = QPointF();
    // Calcolo del punto di controllo per la curva
    if(abs(startLine.x()-stopLine.x())<80){
        if(startLine.y()-stopLine.y()<0){
            controlPoint = QPointF(((startLine.x() + stopLine.x())/2)-80, ((startLine.y()+stopLine.y())/2));
            qDebug() << "zioporco";
        }
        else{
            controlPoint = QPointF(((startLine.x() + stopLine.x())/2)+80, ((startLine.y()+stopLine.y())/2));
            qDebug() << "zioporco 1";
        }
    }
    else if(abs(startLine.y()-stopLine.y())<80){
        if(startLine.x()-stopLine.x()<0){
            controlPoint = QPointF((startLine.x() + stopLine.x())/2,((startLine.y()+stopLine.y())/2)+80);
            qDebug() << "dentro qui";
        }
        else{
            controlPoint = QPointF((startLine.x() + stopLine.x())/2, ((startLine.y()+stopLine.y())/2)-80);
            qDebug() << "dentro qui 2";
        }
    }
    else{
        if(startLine.y()-stopLine.y()<0){
            controlPoint = QPointF(((startLine.x() + stopLine.x())/2)-40, ((startLine.y()+stopLine.y())/2)-40);
            qDebug() << "zioporco 4";
        }
        else{
            controlPoint = QPointF(((startLine.x() + stopLine.x())/2)+40, ((startLine.y()+stopLine.y())/2)+40);
            qDebug() << "zioporco 3";
        }
    }



    /*
    QPainterPath path;
    path.moveTo(startLine);
    path.quadTo(controlPoint, stopLine);*/

    // Calcoliamo i punti intermedi lungo la curva
    points.clear();
    int numPoints = 1;  // Numero di punti intermedi per la curva
    for (int i = 0; i <= numPoints; ++i) {
        qreal t = qreal(i) / qreal(numPoints);
        qreal mt = 1.0 - t;
        qreal mt2 = mt * mt;
        qreal t2 = t * t;
        QPointF point = mt2 * startLine + 2.0 * mt * t * controlPoint + t2 * stopLine;
        points.append(point);
    }

    // Costruiamo il percorso utilizzando i punti intermedi
    QPainterPath path;
    path.moveTo(startLine);
    for (int i = 1; i < points.size(); ++i) {
        path.lineTo(points[i]);
    }

    // Creiamo una penna per il disegno della curva
    QPen curvePen(Qt::black);
    curvePen.setWidth(2);

    // Disegna la curva utilizzando strokePath
    painter->strokePath(path, curvePen);

    // Disegna la curva e la freccia
    painter->drawPath(path);
    painter->drawLine(stopLine, arrowP1);
    painter->drawLine(stopLine, arrowP2);
}



bool ArrowItem::operator==(const ArrowItem& other) const {
    return startItem->sceneBoundingRect().center() == other.startItem->sceneBoundingRect().center() && endItem->sceneBoundingRect().center() == other.endItem->sceneBoundingRect().center();
}

bool ArrowItem::isMyStartOrEnd(QGraphicsEllipseItem *check){
    if(!startItem || !endItem){
        return true;
    }
    qDebug() << check->sceneBoundingRect().center() << "\n";
    qDebug() << startItem->sceneBoundingRect().center() << "\n";
    qDebug() << endItem->sceneBoundingRect().center() << "\n";
    if (check->sceneBoundingRect().center() == startItem->sceneBoundingRect().center() || check->sceneBoundingRect().center() == endItem->sceneBoundingRect().center())return true;
    return false;
}
