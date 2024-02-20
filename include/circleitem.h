#include <QGraphicsEllipseItem>
#include <QBrush>
#include <QPen>
#include <QPointF>

#ifndef CIRCLEITEM_H
#define CIRCLEITEM_H

class CircleItem {
public:
    CircleItem(QGraphicsEllipseItem* ellipse) {
        // Retrieve properties from the ellipse
        this->ellipse = ellipse;
        position = ellipse->sceneBoundingRect().center();
        size = ellipse->rect().size();
        pen = ellipse->pen();
        brush = ellipse->brush();
    }

    // Comparison operator to check equality
    bool operator==(const CircleItem& other) const {
        return ellipse->sceneBoundingRect().center() == other.ellipse->sceneBoundingRect().center() &&
               ellipse->rect().size() == other.ellipse->rect().size() &&
               ellipse->pen() == other.ellipse->pen() &&
               ellipse->brush() == other.ellipse->brush();
    }

    // Overload the << operator for qDebug()
    friend QDebug operator<<(QDebug dbg, const CircleItem& circleItem) {
        dbg.nospace() << "CircleItem: "
                      << "Position=" << circleItem.ellipse->sceneBoundingRect().center() << "\n";
        return dbg.space();
    }

private:
    QPointF position;
    QSizeF size;
    QPen pen;
    QBrush brush;
    QGraphicsEllipseItem* ellipse;
};


#endif // CIRCLEITEM_H
