#ifndef COMMANDMODEL_H
#define COMMANDMODEL_H

#include <QString>
#include <QVector>

class CommandModel {
public:
    CommandModel();

    CommandModel(const QString& commandName);

    void setCommandName(const QString& commandName);

    QString commandName() const;
    quint32 commandNameHash() const;

    enum CommandAnswer {
        ANSWER_UNKNOWN = 0,
        ANSWER_RIGHT,
        ANSWER_WRONG
    };

    QVector<CommandAnswer> m_answers;
    quint32 rightAnswersCount;
    quint32 rating;
    QString m_commandId;
    QString m_commandLocation;
    QString m_tableNumber;

    static bool sort_by_name(const CommandModel *x, const CommandModel *y);
    static bool sort_by_result(const CommandModel *x, const CommandModel *y);
private:
    QString m_commandName;
    quint32 m_commandNameHash;

    void updateHash();
};



#endif // COMMANDMODEL_H
