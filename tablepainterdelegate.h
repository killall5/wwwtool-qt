#ifndef TABLEPAINTERDELEGATE_H
#define TABLEPAINTERDELEGATE_H

#include <QItemDelegate>

class TablePainterDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit TablePainterDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:

};

#endif // TABLEPAINTERDELEGATE_H
