#ifndef ARROWITEM_H
#define ARROWITEM_H

#include <QGraphicsItem>

class ArrowItem : public QGraphicsItem
{
public:
    ArrowItem(QGraphicsItem *startItem, QGraphicsItem *endItem, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    QGraphicsItem *startItem;
    QGraphicsItem *endItem;
};

#endif // ARROWITEM_H
