#include "../include/arrowitem.h"
#include <QPainter>
#include <iostream>
ArrowItem::ArrowItem(QGraphicsItem *startItem, QGraphicsItem *endItem, QGraphicsItem *parent)
    : QGraphicsItem(parent), startItem(startItem), endItem(endItem)
{
    setFlag(ItemSendsGeometryChanges);
}

QRectF ArrowItem::boundingRect() const
{
    qreal penWidth = 1.0; // Adjust pen width as needed
    qreal extra = (penWidth + 20) / 2.0; // Additional space for the arrow heads
    return QRectF(startItem->sceneBoundingRect().center(),
                  QSizeF(endItem->sceneBoundingRect().center().x() - startItem->sceneBoundingRect().center().x(),
                         endItem->sceneBoundingRect().center().y() - startItem->sceneBoundingRect().center().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}


void ArrowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!startItem || !endItem)
        return;

    QPointF startHomemade(startItem->sceneBoundingRect().center().x(), startItem->sceneBoundingRect().center().y());
    QPointF endHomemade(endItem->sceneBoundingRect().center().x(), endItem->sceneBoundingRect().center().y());
    painter->setPen(QPen(Qt::black, 2));

    qreal arrowSize = 20;
    qreal angle = atan2(endHomemade.y() - startHomemade.y(), endHomemade.x() - startHomemade.x());
    QPointF arrowP1 = endHomemade - QPointF(sin(angle - M_PI / 3) * arrowSize, cos(angle - M_PI / 3) * arrowSize);
    QPointF arrowP2 = endHomemade - QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize, cos(angle - M_PI + M_PI / 3) * arrowSize);

    painter->drawLine(startHomemade, endHomemade);
    painter->drawLine(endHomemade, arrowP1);
    painter->drawLine(endHomemade, arrowP2);
}
