#include "tablepainterdelegate.h"
#include <QPainter>
#include "gamemodel.h"

TablePainterDelegate::TablePainterDelegate(GameModel *m, QObject *parent) :
    QItemDelegate(parent), m_model(m)
{
}

void TablePainterDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool willFill = false;
    QBrush brush = option.palette.base();
    if (index.row()%2) {
        brush.setColor(brush.color().darker(110));
        willFill = true;
    }
    if (index.column() > 0 && index.column() == m_model->fixedQuestion()) {
        brush.setColor(brush.color().darker(110));
        willFill = true;
    }
    if (willFill) {
        painter->fillRect(option.rect, brush);
    }
    QItemDelegate::paint(painter, option, index);
}
