#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QMap>
#include <QPrinter>

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

    quint32 questionCount() { return m_questionCount; }
signals:
    
public slots:
    void setQuestionCount(quint32 questionCount);
    void printBlanks(QPrinter *printer) const;

private:
    QList<CommandModel*> m_commands;
    quint32 m_questionCount;

    QString toBarcodeText(quint32 hash, quint32 question = 0) const;
};

#endif // GAMEMODEL_H
