#include "tablepainterdelegate.h"
#include <QPainter>
#include "gamemodel.h"

TablePainterDelegate::TablePainterDelegate(GameModel *m, QObject *parent) :
    QItemDelegate(parent), m_model(m)
{
}

void TablePainterDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.row()%2 || (index.column() > 0 && index.column() == m_model->fixedQuestion())) {
        QBrush brush = option.palette.base();
        brush.setColor(brush.color().darker(110));
        painter->fillRect(option.rect, brush);
    }
    QItemDelegate::paint(painter, option, index);
}
