#include "tablepainterdelegate.h"
#include <QPainter>

TablePainterDelegate::TablePainterDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

void TablePainterDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.row()%2) {
        QBrush brush = option.palette.base();
        brush.setColor(brush.color().darker(110));
        painter->fillRect(option.rect, brush);
    }
    QItemDelegate::paint(painter, option, index);
}
