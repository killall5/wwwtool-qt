#include "gamemodel.h"
#include <QException>

class CommandModel {
public:
    CommandModel(QString commandName) : m_commandName(commandName) {}

    QString commandName() const { return m_commandName; }
private:
    QString m_commandName;

};

GameModel::GameModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_questionCount = 0;
}

qint32 GameModel::getFreeTable()
{
    if (m_useFreeTable)
        return m_commands.count()+1;
    return 10;
}

#include <QDebug>

void GameModel::addCommand(const QString& commandName, const qint32 tableNumber)
{
    if (m_commands.contains(tableNumber)) {
        QException().raise();
    }

    QModelIndex index;
    beginInsertRows(index, m_commands.count(), m_commands.count());
    m_commands[tableNumber] = new CommandModel(commandName);
    endInsertRows();
}

void GameModel::setQuestionCount(qint32 questionCount)
{
    QModelIndex index;
    if (questionCount < m_questionCount) {
        beginRemoveColumns(index, questionCount, m_questionCount-1);
        m_questionCount = questionCount;
        endRemoveColumns();
    } else if (questionCount > m_questionCount) {
        beginInsertColumns(index, m_questionCount, questionCount-1);
        m_questionCount = questionCount;
        endInsertColumns();
    }

}

int GameModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_commands.count();
}

int GameModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_questionCount+2;
}

QVariant GameModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    switch (index.column()) {
    case 0:
        return QString("Команда %1").arg(index.row());
    case 1:
        return QString("0/0");
    }

    return QVariant();
}

QVariant GameModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString("Название");
        case 1:
            return QString("Результат");
        default:
            return QString("%1").arg(section-1);
        }
    }

    return QVariant();
}
