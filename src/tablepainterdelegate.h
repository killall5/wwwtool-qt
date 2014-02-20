#ifndef TABLEPAINTERDELEGATE_H
#define TABLEPAINTERDELEGATE_H

#include <QItemDelegate>

class GameModel;

class TablePainterDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit TablePainterDelegate(GameModel *, QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:

private:
    GameModel *m_model;
};

#endif // TABLEPAINTERDELEGATE_H
