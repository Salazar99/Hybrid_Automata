#ifndef ARROWITEM_H
#define ARROWITEM_H

#include <QGraphicsItem>

class ArrowItem : public QGraphicsItem
{
public:
    ArrowItem(QGraphicsItem *startItem, QGraphicsItem *endItem, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QPainterPath shape() const override;
    bool isMyStartOrEnd(QGraphicsEllipseItem* check);
    bool operator==(const ArrowItem& other) const;
    QGraphicsItem *startItem;
    QGraphicsItem *endItem;

    QList<QPointF> points;


};

#endif // ARROWITEM_H
