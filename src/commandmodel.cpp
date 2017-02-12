#include"commandmodel.h"
#include <QCryptographicHash>

CommandModel::CommandModel() : rightAnswersCount(0), rating(0) {
    updateHash();
}

CommandModel::CommandModel(const QString& commandName) : rightAnswersCount(0), rating(0), m_commandName(commandName) {
    updateHash();
}

void CommandModel::setCommandName(const QString& commandName) {
    m_commandName = commandName;
    updateHash();
}

QString CommandModel::commandName() const {
    return m_commandName;
}

quint32 CommandModel::commandNameHash() const {
    return m_commandNameHash;
}

void CommandModel::updateHash() {
    QCryptographicHash sha1(QCryptographicHash::Sha1);
    std::string str = m_commandName.toStdString();
    sha1.addData(str.c_str(), str.length());
    m_commandNameHash = sha1.result().toHex().left(8).toULong(0, 16);
}

// always asceding
bool CommandModel::sort_by_name(const CommandModel *x, const CommandModel *y) {
    return x->commandName().toLower() < y->commandName().toLower();
}

// always desceding
bool CommandModel::sort_by_result(const CommandModel *x, const CommandModel *y) {
    if (x->rightAnswersCount == y->rightAnswersCount) {
        return x->rating > y->rating;
    } else {
        return x->rightAnswersCount > y->rightAnswersCount;
    }
}

