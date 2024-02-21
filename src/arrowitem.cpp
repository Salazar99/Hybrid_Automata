#include "../include/arrowitem.h"
#include <QStyleOptionGraphicsItem> // Include the header for QStyleOptionGraphicsItem
#include <QPainter>
#include <QStyle>
#include <iostream>
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
}

/*void ArrowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    qDebug() << "Stiamo paintando\n";

    if (!startItem || !endItem){
        qDebug() << "Qualcuno Ã¨ null!\n";
        return;
    }

    qDebug() << "Paint: " << startItem->sceneBoundingRect().center() << ", " << endItem->sceneBoundingRect().center();

    QPointF startHomemade(startItem->sceneBoundingRect().center().x(), startItem->sceneBoundingRect().center().y());
    QPointF endHomemade(endItem->sceneBoundingRect().center().x(), endItem->sceneBoundingRect().center().y());
    painter->setPen(QPen(Qt::black, 2));

    qreal arrowSize = 20;
    qreal angle = atan2(endHomemade.y() - startHomemade.y(), endHomemade.x() - startHomemade.x());
    QPointF arrowP1 = endHomemade - QPointF(sin(angle - M_PI / 3) * arrowSize, cos(angle - M_PI / 3) * arrowSize);
    QPointF arrowP2 = endHomemade - QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize, cos(angle - M_PI + M_PI / 3) * arrowSize);

    QPointF controlPoint = QPointF((startHomemade.x() + endHomemade.x()) / 2, startHomemade.y()+80);
    // Calcolo del punto di controllo per la curva
    if(abs(startHomemade.x()-endHomemade.x())<80){
        if(startHomemade.y()-endHomemade.y()<0)
            controlPoint = QPointF(startHomemade.x()+80 / 2, (startHomemade.y()+endHomemade.y())/2);
        else{
            controlPoint = QPointF(startHomemade.x()-80 / 2, (startHomemade.y()+endHomemade.y())/2);
        }
    }
    else{
        if(startHomemade.x()-endHomemade.x()<0)
            controlPoint = QPointF((startHomemade.x() + endHomemade.x()) / 2, startHomemade.y()+80);
        else{
            controlPoint = QPointF((startHomemade.x() + endHomemade.x()) / 2, startHomemade.y()-80);
        }
    }


    QPainterPath path;
    path.moveTo(startHomemade);
    path.quadTo(controlPoint, endHomemade);

    // Disegna la curva e la freccia
    painter->drawPath(path);
    painter->drawLine(endHomemade, arrowP1);
    painter->drawLine(endHomemade, arrowP2);
}*/

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
