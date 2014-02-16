#include "tablepainterdelegate.h"
#include <QPainter>

TablePainterDelegate::TablePainterDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

void TablePainterDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.row()%2) {
        painter->fillRect(option.rect, option.palette.midlight());
    }
    QItemDelegate::paint(painter, option, index);
}
