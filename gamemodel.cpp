#include "gamemodel.h"
#include <QException>
#include <QCryptographicHash>
#include <QDebug>
#include <QPainter>
#include <QFile>
#include <QXmlStreamWriter>
#include <cmath>
#include "code128.h"

class CommandModel {
public:
    CommandModel() {
        updateHash();
    }

    CommandModel(const QString& commandName) : m_commandName(commandName) {
        updateHash();
    }

    void setCommandName(const QString& commandName) {
        m_commandName = commandName;
        updateHash();
    }

    QString commandName() const { return m_commandName; }
    quint32 commandNameHash() const { return m_commandNameHash; }

    enum CommandAnswer {
        ANSWER_UNKNOWN = 0,
        ANSWER_RIGHT,
        ANSWER_WRONG
    };

    QVector<CommandAnswer> m_answers;
private:
    QString m_commandName;
    quint32 m_commandNameHash;

    void updateHash() {
        QCryptographicHash sha1(QCryptographicHash::Sha1);
        std::string str = m_commandName.toStdString();
        sha1.addData(str.c_str(), str.length());
        m_commandNameHash = sha1.result().toHex().left(8).toULong(0, 16);
    }
};

GameModel::GameModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_questionCount = 0;
}

#include <QDebug>

void GameModel::addCommand(const QString& commandName)
{
//    if (m_commands.indexOf(commandName) != -1)
//        QException().raise();
//    }

    QModelIndex index;
    beginInsertRows(index, m_commands.size(), m_commands.size());
    CommandModel *command = new CommandModel(commandName);
    command->m_answers.resize(m_questionCount);
    m_commands.push_back(command);
    endInsertRows();
}

void GameModel::setQuestionCount(quint32 questionCount)
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
    for (int i = 0; i < m_commands.length(); ++i) {
        m_commands[i]->m_answers.resize(questionCount);
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
        return m_commands[index.row()]->commandName();
    case 1:
        return QString("0/0");
    default:
        switch (m_commands[index.row()]->m_answers[index.column()-2]) {
        case CommandModel::ANSWER_RIGHT:
            return QString("+");
        case CommandModel::ANSWER_WRONG:
            return QString("–");
        }
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

void GameModel::printBlanks(QPrinter *printer) const
{
    const quint32 COLS=3;
    const quint32 ROWS=5;

    QPagedPaintDevice::Margins m = {5, 5, 5, 5};
    printer->setMargins(m);

    QPainter painter(printer);

    qreal pageWidth = printer->width();
    qreal cellWidth = pageWidth/COLS;
    qreal pageHeight = printer->height();
    qreal cellHeight = pageHeight/ROWS;

    QPen defaultPen(QColor(0, 0, 0));

    QVector<qreal> dashPattern;
    QPen defaultHPen(QColor(0, 0, 0));
    dashPattern << cellWidth/24 << cellWidth/24 << cellWidth/24 << 0;
    defaultHPen.setDashPattern(dashPattern);

    dashPattern.clear();
    QPen defaultVPen(QColor(0, 0, 0));
    dashPattern << cellHeight/24 << cellHeight/24 << cellHeight/24 << 0;
    defaultVPen.setDashPattern(dashPattern);

    QPen questionPen(QColor(0, 0, 0, 32));

    int pagesCount = ceil(m_questionCount*1.0/COLS/ROWS);
    QFont questionFont = painter.font();
    qreal ratio = cellHeight/2 / painter.fontMetrics().boundingRect(QString("%1").arg(m_questionCount)).size().height();
    questionFont.setPointSizeF(questionFont.pointSizeF()*ratio);

    painter.setBrush(QBrush(Qt::SolidPattern));

    for (int command=0; command < m_commands.size(); ++command) {
        QString commandName = m_commands[command]->commandName();

        QFont commandNameFont(painter.font());
        ratio = cellWidth*0.85 / painter.fontMetrics().boundingRect(commandName).size().width();
        commandNameFont.setPointSizeF(qMin(14.0, painter.font().pointSizeF()*ratio));

        for (int page=0; page < pagesCount; ++page) {
            quint32 col, row, q;
            painter.setPen(defaultVPen);
            for (col=1; col < COLS; ++col) {
                painter.drawLine(col*cellWidth, 0, col*cellWidth, pageHeight);
            }
            painter.setPen(defaultHPen);
            for (row=1; row < ROWS; ++row) {
                painter.drawLine(0, row*cellHeight, pageWidth, row*cellHeight);
            }

            for (row=0; row < ROWS; ++row) {
                for (col=0; col < COLS; ++col) {
                    q=page*COLS*ROWS+col+row*COLS+1;

                    painter.setPen(defaultPen);
                    painter.setFont(commandNameFont);
                    QRect bounds = painter.fontMetrics().boundingRect(commandName);
                    painter.drawText(col*cellWidth + cellWidth/20, row*cellHeight + cellHeight/20, cellWidth*0.90, bounds.height(), Qt::AlignTop|Qt::AlignHCenter, commandName);

                    QRectF barcodeRect(col*cellWidth + cellWidth/4, (row+1)*cellHeight - cellHeight/20 - cellWidth/6, cellWidth/2, cellWidth/6);
                    if (q <= m_questionCount) {
                        Code128::drawBarcode(toBarcodeText(m_commands[command]->commandNameHash(), q), painter, barcodeRect);

                        painter.setPen(questionPen);
                        painter.setFont(questionFont);
                        painter.drawText(col*cellWidth, row*cellHeight + cellHeight/20 + bounds.height(), cellWidth, barcodeRect.top() - (row*cellHeight + cellHeight/20 + bounds.height()), Qt::AlignCenter, QString("%1").arg(q));
                    } else {
                        Code128::drawBarcode(toBarcodeText(m_commands[command]->commandNameHash()), painter, barcodeRect);
                    }
                }
            }

            if (page != pagesCount-1) {
                printer->newPage();
            }
        }

        if (command != m_commands.size()-1) {
            printer->newPage();
        }
    }
}

QString GameModel::toBarcodeText(quint32 hash, quint32 question) const
{
    quint64 res = (quint64)hash << 32 | question;
    return QString("%1").arg(res, 20, 10, QLatin1Char('0'));
}

bool GameModel::fromBarcodeText(const QString& text, quint32 *hash, quint32 *question) const {
    //if (text.length() != 20)
    //    return false;

    bool ok = false;
    quint64 res = text.toULongLong(&ok);
    *hash = res >> 32;
    *question = res;
    return ok;
}

bool GameModel::save(QString fileName) {
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();

    xml.writeStartElement("WWWTool");

    xml.writeStartElement("Options");
    xml.writeTextElement("QuestionCount", QString("%1").arg(m_questionCount));
    xml.writeEndElement();

    xml.writeStartElement("Commands");
    for (int i = 0; i != m_commands.length(); ++i) {
        CommandModel *command = m_commands[i];
        xml.writeStartElement("Command");
        xml.writeTextElement("Name", command->commandName());
        for (quint32 q = 0; q < m_questionCount; ++q) {
            if (command->m_answers[q] != CommandModel::ANSWER_UNKNOWN) {
                xml.writeStartElement("Question");
                xml.writeAttribute("number", QString("%1").arg(q+1));
                xml.writeAttribute("value", QString("%1").arg(command->m_answers[q]));
                xml.writeEndElement();
            }
        }
        xml.writeEndElement();
    }
    xml.writeEndElement();

    xml.writeEndDocument();

    file.close();
    return !xml.hasError();
}

bool GameModel::load(QString fileName) {
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    QXmlStreamReader xml(&file);
    // Read this:
    //  http://www.developer.nokia.com/Community/Wiki/QXmlStreamReader_to_parse_XML_in_Qt

    quint32 l_questionCount = 0;
    QList<CommandModel *> l_commands;
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartDocument) {
            continue;
        }

        if (token == QXmlStreamReader::StartElement) {
            if (xml.name() == "QuestionCount") {
                xml.readNext();
                l_questionCount = xml.text().toString().toInt() ;
            }
            if (xml.name() == "Command") {
                CommandModel *command = new CommandModel;
                while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "Command")) {
                    if (xml.tokenType() == QXmlStreamReader::StartElement) {
                        if (xml.name() == "Name") {
                            xml.readNext();
                            command->setCommandName(xml.text().toString());
                        } else if (xml.name() == "Question") {
                            QXmlStreamAttributes attr = xml.attributes();
                            if (attr.hasAttribute("number") && attr.hasAttribute("value")) {
                                int value = attr.value("value").toString().toInt();
                                int q = attr.value("number").toString().toInt();

                                command->m_answers.resize(qMax(command->m_answers.size(), q));
                                command->m_answers[q-1] = static_cast<CommandModel::CommandAnswer>(value);
                            }

                        }
                    }
                    xml.readNext();
                }
                l_commands.append(command);
            }
        }
    }

    if (xml.hasError()) {
        return false;
    }

    beginResetModel();
    m_commands.swap(l_commands);
    endResetModel();

    setQuestionCount(l_questionCount);
    return true;
}

void GameModel::click(int col, int row) {
    if (col > 1) {
        CommandModel::CommandAnswer *a = &m_commands[row]->m_answers[col-2];
        if (*a != CommandModel::ANSWER_RIGHT) {
            *a = CommandModel::ANSWER_RIGHT;
        } else {
            *a = CommandModel::ANSWER_WRONG;
        }
        QModelIndex i = createIndex(row, col);
        dataChanged(i, i);
    }
}

void GameModel::readFromScanner(const QString &text) {
    quint32 hash, q;
    if (fromBarcodeText(text, &hash, &q)) {
        qDebug() << "read from scanner" << text << ": hash=" << hash << "q=" << q;
    }
}
