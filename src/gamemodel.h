#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QMap>
#include <QPrinter>
#include <QVector>
#include "commandmodel.h"

class GameModel : public QAbstractTableModel
{
    Q_OBJECT
public:
	enum SortCriteria {
		SORT_BY_TITLE,
        SORT_BY_TABLE,
		SORT_BY_RESULT
	};

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
    bool load(QString fileName);
    QString title() const { return m_title; }
    bool modified() const { return m_modified; }
    int fixedQuestion() const { return m_fixedQuestion; }

    void sort_by_criteria(SortCriteria criteria);

    void set_finished(bool);
    bool finished() const { return m_finished; }
signals:
    void titleChanged() const;
    void finishedChanged() const;
    void questionCountChanged(int) const;
    void fixedQuestionChanged(int) const;

public slots:
    void setQuestionCount(quint32 questionCount);
    void click(int col, int row);
    void empty(int col, int row);
    void printBlanks(QPrinter *printer) const;
    void printTables(QPrinter *printer) const;
    bool readFromScanner(const QString& text, QModelIndexList *affectedIndexes);
    void fixQuestion(int column);

private:
    QList<CommandModel*> m_commands;
    quint32 m_questionCount;
    bool m_autoSave;
    bool m_finished;
    QVector<qint32> m_questionRating;
    mutable QString m_fileName;
    mutable QString m_title;
    mutable bool m_modified;
    int m_fixedQuestion;

    QString toBarcodeText(quint32 hash, quint32 question = 0) const;
    bool fromBarcodeText(const QString& text, quint32 *hash, quint32 *question) const;

    void invertCommandResult(int commandNumber, quint32 question);
    void setCommandResult(int commandNumber, quint32 question, CommandModel::CommandAnswer);
    void autoSave();
};

#endif // GAMEMODEL_H
