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
    void removeCommandsAtRows(const QSet<int>& rows);

    quint32 questionCount() { return m_questionCount; }
    bool save() const;
    bool save(QString fileName) const;
    void exportHTML(QString fileName) const;
    void exportCSV(QString fileName) const;
    bool load(QString fileName);
    QString title() const { return m_title; }
    bool modified() const { return m_modified; }
signals:
    void titleChanged() const;
    
public slots:
    void setQuestionCount(quint32 questionCount);
    void click(int col, int row);
    void printBlanks(QPrinter *printer) const;
    bool readFromScanner(const QString& text);
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

private:
    QList<CommandModel*> m_commands;
    quint32 m_questionCount;
    QVector<qint32> m_questionRating;
    mutable QString m_fileName;
    mutable QString m_title;
    mutable bool m_modified;

    QString toBarcodeText(quint32 hash, quint32 question = 0) const;
    bool fromBarcodeText(const QString& text, quint32 *hash, quint32 *question) const;

    void invertCommandResult(int commandNumber, quint32 question);
};

#endif // GAMEMODEL_H
