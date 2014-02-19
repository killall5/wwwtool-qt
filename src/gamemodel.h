#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QMap>
#include <QPrinter>
#include <QVector>

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
    bool save(QString fileName) const;
    void exportHTML(QString fileName) const;
    void exportCSV(QString fileName) const;
    bool load(QString fileName);
signals:
    
public slots:
    void setQuestionCount(quint32 questionCount);
    void click(int col, int row);
    void printBlanks(QPrinter *printer) const;
    bool readFromScanner(const QString& text);

private:
    QList<CommandModel*> m_commands;
    quint32 m_questionCount;
    QVector<qint32> m_questionRating;

    QString toBarcodeText(quint32 hash, quint32 question = 0) const;
    bool fromBarcodeText(const QString& text, quint32 *hash, quint32 *question) const;

    void invertCommandResult(int commandNumber, quint32 question);
};

#endif // GAMEMODEL_H
