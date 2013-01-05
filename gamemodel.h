#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QMap>

class CommandModel;

class GameModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit GameModel(QObject *parent = 0);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void addCommand(const QString& commandName);

    qint32 questionCount() { return m_questionCount; }
signals:
    
public slots:
    void setQuestionCount(qint32 questionCount);

private:
    QList<CommandModel*> m_commands;
    qint32 m_questionCount;
};

#endif // GAMEMODEL_H
