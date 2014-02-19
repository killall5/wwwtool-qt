#ifndef CODE128_H
#define CODE128_H

#include <QString>
#include <QPainter>

namespace Code128 {

    void drawBarcode(const QString &src, QPainter& painter, const QRectF &rect);

}

#endif // CODE128_H
