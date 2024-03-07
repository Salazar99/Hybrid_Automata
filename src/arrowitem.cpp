#include "../include/arrowitem.h"
#include <QStyleOptionGraphicsItem> // Include the header for QStyleOptionGraphicsItem
#include <QPainter>
#include <QStyle>
#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>

ArrowItem::ArrowItem(QGraphicsItem *startItem, QGraphicsItem *endItem, QGraphicsItem *parent)
    : QGraphicsItem(parent), startItem(startItem), endItem(endItem)
{
    setFlag(ItemSendsGeometryChanges);
    textItem = new QGraphicsTextItem("Arrow", this);
    textItem->setDefaultTextColor(Qt::black);
}

QRectF ArrowItem::calculateArrowRect() const
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

void ArrowItem::updateTextPosition()
{
    // Adjust the offset as needed
    //textItem->setPos(controlPoint);
    QPointF startHomemade(startItem->sceneBoundingRect().center().x(), startItem->sceneBoundingRect().center().y());
    QPointF endHomemade(endItem->sceneBoundingRect().center().x(), endItem->sceneBoundingRect().center().y());
    qreal angle = atan2(startHomemade.y() - endHomemade.y(), startHomemade.x() - endHomemade.x()) + 0.5;
    qreal angle_start = atan2(endHomemade.y() - startHomemade.y(), endHomemade.x() - startHomemade.x()) - 0.5;
    qreal endItemRadius = qMin(endItem->boundingRect().width(), endItem->boundingRect().height()) / 2.0;
    QPointF stopLine = endHomemade + QPointF(endItemRadius*cos(angle),endItemRadius*sin(angle));
    QPointF startLine = startHomemade + QPointF(endItemRadius*cos(angle_start),endItemRadius*sin(angle_start));
    QPointF arrowCenter = (startLine + stopLine) / 2;
    QPointF textOffset(0.5, 0.5); // Adjust the offset as needed
    textItem->setPos(arrowCenter + textOffset);

}

QVariant ArrowItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{    if (!textItem || !change){
        QVariant base;
        return base;
    }
    qDebug() << "QUAQUA\n";
    if (change == ItemPositionHasChanged) {
        // Update text position whenever the arrow item moves
        updateTextPosition();
    }
    return QGraphicsItem::itemChange(change, value);
}

QRectF ArrowItem::boundingRect() const
{
    if (!startItem || !endItem){
        //delete this;
        return QRectF();
    }
    // Calculate bounding rectangle including both arrow and text
    QRectF arrowRect = calculateArrowRect();
    QRectF textRect = textItem->boundingRect();
    return arrowRect.united(textRect);
}

QPainterPath ArrowItem::shape() const
{

    QRectF boundingRect = textItem->mapToParent(textItem->boundingRect()).boundingRect();
    QPainterPath path;
    path.addRect(boundingRect);
    return path;

}

void ArrowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    //qDebug() << "Stiamo paintando\n";

    if (!startItem || !endItem){
        qDebug() << "Qualcuno Ã¨ null!\n";
        return;
    }
    bool adjustText = false;
    bool isSelected = option->state & QStyle::State_Selected;
    //qDebug() << "Paint: " << startItem->sceneBoundingRect().center() << ", " << endItem->sceneBoundingRect().center();
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

    /*QPointF arrowP1 = QPointF(stopLine.x()+ 10*cos(angle-0.5),stopLine.y()+10*sin(angle-0.5));  previous with curves
    QPointF arrowP2 = QPointF(stopLine.x()+ 10*cos(angle+0.5),stopLine.y()+10*sin(angle+0.5));*/
    QPointF arrowP1 = QPointF(stopLine.x()+ 10*cos(angle),stopLine.y()+10*sin(angle));
    QPointF arrowP2 = QPointF(stopLine.x()+ 10*cos(angle-1),stopLine.y()+10*sin(angle-1));

    controlPoint = QPointF();
    // Calcolo del punto di controllo per la curva
    if(abs(startLine.x()-stopLine.x())<80){
        if(startLine.y()-stopLine.y()<0){
            controlPoint = QPointF(((startLine.x() + stopLine.x())/2)+80, ((startLine.y()+stopLine.y())/2));
        }
        else{
            controlPoint = QPointF(((startLine.x() + stopLine.x())/2)-80, ((startLine.y()+stopLine.y())/2));
        }
    }
    else if(abs(startLine.y()-stopLine.y())<80){
        if(startLine.x()-stopLine.x()<0){

            controlPoint = QPointF((startLine.x() + stopLine.x())/2, ((startLine.y()+stopLine.y())/2)-80);
        }
        else{
            controlPoint = QPointF((startLine.x() + stopLine.x())/2,((startLine.y()+stopLine.y())/2)+80);
        }
    }
    else{
        if(startLine.y()-stopLine.y()<0){
            if(startLine.x()-stopLine.x()<0){
                controlPoint = QPointF(((startLine.x() + stopLine.x())/2)+80, ((startLine.y()+stopLine.y())/2)-40);
                adjustText = true;
            }
            else{
                controlPoint = QPointF(((startLine.x() + stopLine.x())/2)+40, ((startLine.y()+stopLine.y())/2)+40);
            }
        }
        else{
            if(startLine.x()-stopLine.x()<0){
                controlPoint = QPointF(((startLine.x() + stopLine.x())/2)-40, ((startLine.y()+stopLine.y())/2)-40);
            }
            else{
                controlPoint = QPointF(((startLine.x() + stopLine.x())/2)-80, ((startLine.y()+stopLine.y())/2)+40);
                adjustText = true;
            }
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
    pen.setWidth(2);

    // Disegna la curva utilizzando strokePath
    painter->strokePath(path, pen);

    // Disegna la curva e la freccia
    QPolygonF arrowHead;
    arrowHead << stopLine << arrowP1 << arrowP2;

    QBrush fillBrush(Qt::red);  // Imposta il colore di riempimento
    painter->setBrush(fillBrush);

    painter->drawPolygon(arrowHead);
    path.addPolygon(arrowHead);
    /*
    painter->drawPath(path);
    painter->drawLine(stopLine, arrowP1);
    painter->drawLine(stopLine, arrowP2);*/

    controlPoint = points[points.size()/2];
    if(adjustText){
        QPointF textOffset(10, 10);
        controlPoint += textOffset;
    }
    else{
        QPointF textOffset(0.4, 0.4);
        controlPoint += textOffset;
    }

    updateTextPosition();
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
