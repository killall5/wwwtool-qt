#ifndef CODE128_H
#define CODE128_H

#include <QString>
#include <QPainter>

namespace Code128 {

    void drawBarcode(const QString &src, QPainter& painter, const QPointF& point, qreal H);

}

#endif // CODE128_H
