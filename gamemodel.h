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
    
    qint32 getFreeTable();
    bool useFreeTable() { return m_useFreeTable; }

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void addCommand(const QString& commandName, const qint32 tableNumber);

    qint32 questionCount() { return m_questionCount; }
signals:
    
public slots:
    void setUseFreeTable(bool value) { m_useFreeTable = value; }
    void setQuestionCount(qint32 questionCount);

private:
    bool m_useFreeTable;
    QMap<qint32, CommandModel*> m_commands;
    qint32 m_questionCount;
};

#endif // GAMEMODEL_H
