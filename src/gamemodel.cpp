#include "gamemodel.h"
#include <QException>
#include <QCryptographicHash>
#include <QDebug>
#include <QPainter>
#include <QFile>
#include <QXmlStreamWriter>
#include <QFileInfo>
#include <cmath>
#include "code128.h"
#include <functional>
#include <QDateTime>
#include <QMarginsF>

GameModel::GameModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_questionCount = 0;
    m_modified = false;
    m_fixedQuestion = 0;
    m_autoSave = false;
    m_finished = false;
}

void GameModel::autoSave() {
    if (!m_autoSave || !m_modified || m_fileName.isEmpty()) return;
    save();
    //exportJSON(m_fileName + ".json");
}

void GameModel::addCommand(const QString& commandName)
{
//    if (m_commands.indexOf(commandName) != -1)
//        QException().raise();
//    }

    QModelIndex index;
    CommandModel *command = new CommandModel(commandName);
    command->m_answers.resize(m_questionCount);
    // update ratings:
    // for questions
    for (quint32 i = 0; i < m_questionCount; ++i) {
        m_questionRating[i]++;
    }
    // for commands
    for (int i = 0; i < m_commands.size(); ++i) {
        m_commands[i]->rating += m_commands[i]->rightAnswersCount;
    }

    beginInsertRows(index, m_commands.size(), m_commands.size());
    m_commands.push_back(command);
    endInsertRows();
    m_modified = true;
    autoSave();
}

void GameModel::setQuestionCount(quint32 questionCount)
{
    if (questionCount == m_questionCount) {
        return;
    }
    QModelIndex index;
    quint32 oldQC = m_questionCount;
    m_modified = m_modified || questionCount != m_questionCount;
    if (questionCount < m_questionCount) {
        beginRemoveColumns(index, questionCount+1, m_questionCount);
        m_questionCount = questionCount;
        endRemoveColumns();
        if ((quint32)m_fixedQuestion > m_questionCount) {
            m_fixedQuestion = 0;
            layoutChanged();
        }
        for (int i = 0; i < m_commands.size(); ++i) {
            bool changed = false;
            for (quint32 q = m_questionCount; q < oldQC; ++q) {
                if (m_commands[i]->m_answers[q] == CommandModel::ANSWER_RIGHT) {
                    changed = true;
                    m_commands[i]->rightAnswersCount--;
                    m_commands[i]->rating -= m_questionRating[q];
                }
            }
            if (changed) {
                QModelIndex idx = createIndex(i, 0);
                dataChanged(idx, idx);
            }
        }
    } else if (questionCount > m_questionCount) {
        beginInsertColumns(index, m_questionCount+1, questionCount);
        m_questionCount = questionCount;
        m_questionRating.resize(m_questionCount);
        for (quint32 q = oldQC; q < m_questionCount; ++q) {
            m_questionRating[q] = m_commands.size() + 1;
        }
        endInsertColumns();
    }
    for (int i = 0; i < m_commands.length(); ++i) {
        m_commands[i]->m_answers.resize(questionCount);
    }
    // update results (rating + right answers count)
    autoSave();
    emit questionCountChanged(m_questionCount);
}

int GameModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_commands.count();
}

int GameModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_questionCount+1;
}

QVariant GameModel::data(const QModelIndex &index, int role) const
{
    if (index.column() > 0 && role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }
    if (role != Qt::DisplayRole)
        return QVariant();

    switch (index.column()) {
    case 0:
        return QString("%1/%2").arg(m_commands[index.row()]->rightAnswersCount).arg(m_commands[index.row()]->rating);
    default:
        switch (m_commands[index.row()]->m_answers[index.column()-1]) {
        case CommandModel::ANSWER_RIGHT:
            return QString("+");
        default:
            return QVariant();
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
            return QString("Результат");
        default:
            return QString("%1").arg(section);
        }
    } else if (orientation == Qt::Vertical) {
        QString ret = m_commands[section]->commandName();
        if (!m_commands[section]->m_tableNumber.isEmpty()) {
            ret = m_commands[section]->m_tableNumber + " " + ret;
        }
        if (!m_commands[section]->m_commandLocation.isEmpty()) {
            ret = ret + ", " + m_commands[section]->m_commandLocation;
        }
        if (m_commands[section]->m_tags.length() > 0) {
            ret += " [" + m_commands[section]->m_tags.join(", ") + "]";
        }
        return ret;
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

    int pagesCount = ceil(m_questionCount*1.0/COLS/ROWS);

    painter.setBrush(QBrush(Qt::SolidPattern));

    QFont defaultFont(painter.font());
    defaultFont.setPointSize(16);
    painter.setFont(defaultFont);
    QRectF questionBounds;

    for (int command=0; command < m_commands.size(); ++command) {
        QString commandName = QString("%1 %2").arg(m_commands[command]->m_tableNumber, m_commands[command]->commandName());

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

                    QRectF commandRect(col*cellWidth + cellWidth/20, row*cellHeight + cellHeight/20, cellWidth*3/4, cellHeight/4);
                    QFont font(defaultFont);
                    bool accept = false;
                    QRectF requiredRect;
                    while (!accept) {
                        painter.setFont(font);
                        requiredRect = painter.boundingRect(commandRect, Qt::TextWordWrap, commandName);
                        accept = commandRect.contains(requiredRect);
                        font.setPointSizeF(font.pointSizeF()-1);
                    }

                    painter.drawText(commandRect, Qt::AlignTop|Qt::AlignLeft|Qt::TextWordWrap, commandName);

                    QRectF barcodeRect(col*cellWidth + cellWidth/4, (row+1)*cellHeight - cellHeight/20 - cellWidth/6, cellWidth/2, cellWidth/6);
                    if (q <= m_questionCount) {
                        Code128::drawBarcode(toBarcodeText(m_commands[command]->commandNameHash(), q), painter, barcodeRect);

                        QRectF questionRect(col*cellWidth + cellWidth*3/4, row*cellHeight + cellHeight/20, cellWidth/4 - cellWidth/20, cellHeight/2);
                        painter.setFont(defaultFont);
                        painter.drawText(questionRect, Qt::AlignTop|Qt::AlignRight, QString("%1").arg(q));
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

void drawTableSide(QPainter* painter, CommandModel* const command) {
    QString commandName = command->commandName();
    QString commandLocation = command->m_commandLocation;
    QString table;
    if (!command->m_tableNumber.isEmpty()) {
        table = command->m_tableNumber;
    }

    QPen defaultPen(QColor(0, 0, 0));
    QPen locationPen(QColor(0, 0, 0, 64));

    QFont font;
    QRectF rect = painter->clipBoundingRect();
    rect = rect.marginsRemoved(QMarginsF(rect.width()/40, 0, rect.width()/40, 0));

    QRectF tableRect = rect;
    tableRect.setWidth(rect.width()*5/12);
    tableRect.setHeight(tableRect.width());
    tableRect.moveTop(rect.top() + rect.height()/12);

    QRectF nameRect;
    nameRect.setTop(rect.top() + rect.height()/6);
    nameRect.setLeft(tableRect.right());
    nameRect.setWidth(rect.width() - tableRect.width());
    nameRect.setHeight(rect.height()/2);

    QRectF locationRect = nameRect;
    locationRect.moveTop(nameRect.bottom());

    qreal margin = rect.width()/60;
    QMarginsF margins(margin, margin, margin, margin);
    tableRect -= margins;
    nameRect -= margins;
    locationRect -= margins;

    const qreal BASE_FONT_SIZE = 160;
    font = painter->font();
    font.setPointSizeF(BASE_FONT_SIZE);
    painter->setFont(font);

    painter->setPen(defaultPen);
//    QBrush red(QColor(255,0,0));
//    painter->fillRect(tableRect, red);
    painter->drawText(tableRect, Qt::AlignTop|Qt::AlignHCenter, table);

    font.setPointSizeF(BASE_FONT_SIZE/4);
    bool accept = false;
    QRectF requiredRect;
    while (!accept) {
        painter->setFont(font);
        requiredRect = painter->boundingRect(nameRect, Qt::TextWordWrap, commandName);
        accept = nameRect.contains(requiredRect);
        font.setPointSizeF(font.pointSizeF()-1);
    }
//    QBrush blue(QColor(0,0,255));
//    painter->fillRect(requiredRect, blue);
    painter->drawText(nameRect, Qt::AlignTop|Qt::TextWordWrap, commandName);

    locationRect.moveTop(locationRect.top() - (nameRect.height() - requiredRect.height()));
//    QBrush green(QColor(0,255,0));
//    painter->fillRect(locationRect, green);
    font.setPointSizeF(BASE_FONT_SIZE/8);
    painter->setFont(font);
    painter->setPen(locationPen);
    painter->drawText(locationRect, Qt::AlignTop|Qt::TextWordWrap, commandLocation);
}

void GameModel::printTables(QPrinter *printer) const
{
    QPainter painter(printer);
    QVector<qreal> dashPattern;
    QPen linePen(QColor(0, 0, 0));
    dashPattern << painter.viewport().width()/48 << painter.viewport().width()/24 << painter.viewport().width()/48 << 0;
    linePen.setDashPattern(dashPattern);

    QRectF helperRect = QRect(0, 0, painter.viewport().width(), painter.viewport().height()/2/3);
    QFont helperFont = painter.font();
    helperFont.setPointSize(16);
    QPen helperPen = QPen(QColor(0, 0, 0));
    QString helperText = "Скрепить здесь";

    QRectF clipRect = painter.viewport();
//    qreal margin = clipRect.height()/7;
    clipRect.setHeight(clipRect.height()/2 - helperRect.height());

    for (int i = 0; i < m_commands.size(); ++i) {
        // top command info
        painter.resetTransform();
        painter.translate(0, helperRect.height());
        painter.setClipRect(clipRect);
        painter.rotate(180);

        drawTableSide(&painter, m_commands[i]);

        // bottom command info
        painter.resetTransform();
        painter.translate(0, helperRect.height() + clipRect.height());
        painter.setClipRect(clipRect);

        drawTableSide(&painter, m_commands[i]);

        // top cut helper
        painter.resetTransform();
        painter.setClipRect(helperRect);
        painter.rotate(180);

        painter.setFont(helperFont);
        painter.setPen(helperPen);
        painter.drawText(painter.clipBoundingRect(), Qt::AlignCenter, helperText);

        // bottom cut helper
        painter.resetTransform();
        painter.translate(0, helperRect.height() + 2*clipRect.height());
        painter.setClipRect(helperRect);

        painter.setFont(helperFont);
        painter.setPen(helperPen);
        painter.drawText(helperRect, Qt::AlignCenter, helperText);

        // cut lines
        painter.setClipping(false);
        painter.resetTransform();
        painter.setPen(linePen);
        painter.drawLine(0, helperRect.height(), painter.viewport().width(), helperRect.height());
        painter.drawLine(0, helperRect.height() + clipRect.height(), painter.viewport().width(), helperRect.height() + clipRect.height());
        painter.drawLine(0, helperRect.height() + 2*clipRect.height(), painter.viewport().width(), helperRect.height() + 2*clipRect.height());

        if (i < m_commands.size() - 1) {
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
    if (text.length() != 20)
        return false;

    bool ok = false;
    quint64 res = text.toULongLong(&ok);
    *hash = res >> 32;
    *question = res;
    return ok;
}

bool GameModel::save() const {
    return save(m_fileName);
}

bool GameModel::save(QString fileName) const {
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();

    xml.writeStartElement("WWWTool");

    xml.writeStartElement("Options");
    xml.writeTextElement("QuestionCount", QString("%1").arg(m_questionCount));
    xml.writeTextElement("AutoSave", QString("%1").arg(m_autoSave));
    xml.writeTextElement("FixedQuestion", QString("%1").arg(m_fixedQuestion));
    xml.writeTextElement("Finished", QString("%1").arg(m_finished));
    xml.writeEndElement();

    xml.writeStartElement("Commands");
    for (int i = 0; i != m_commands.length(); ++i) {
        CommandModel *command = m_commands[i];
        xml.writeStartElement("Command");
        xml.writeTextElement("Name", command->commandName());
        if (!command->m_commandId.isEmpty()) {
            xml.writeTextElement("Id", command->m_commandId);
        }
        if (!command->m_commandLocation.isEmpty()) {
            xml.writeTextElement("Location", command->m_commandLocation);
        }
        if (!command->m_tableNumber.isEmpty()) {
            xml.writeTextElement("Table", command->m_tableNumber);
        }
        if (command->m_tags.length() > 0) {
            xml.writeStartElement("Tags");
            for(const auto &tag: command->m_tags) {
                xml.writeTextElement("Tag", tag);
            }
            xml.writeEndElement();
        }
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
    if (fileName != m_fileName) {
        m_fileName = fileName;
        QFileInfo fileInfo(fileName);
        m_title = fileInfo.baseName();
        emit titleChanged();
    }
    m_modified = false;
    return !xml.hasError();
}


QByteArray GameModel::get_jsonResults() const {
    QByteArray jsonResults;
    QString now = QDateTime::currentDateTime().toString(Qt::ISODate);
    jsonResults.append(QString("{\"qc\":%1,\"updated\":\"%2\",\"fixed_question\":%3,").arg(m_questionCount).arg(now).arg(m_fixedQuestion));
    jsonResults.append(QString("\"format\":{\"name\":0,\"right_answers_count\":1,\"rating\":2,\"right_answers_array\":3,\"id\":4,\"location\":5,\"table\":6},"));
    jsonResults.append(QString("\"res\":["));
    for (int i = 0; i < m_commands.size(); ++i) {
        if (i) jsonResults += ",";
        jsonResults += "["
                "\"" + m_commands[i]->commandName() + "\"," +
                QString("%1").arg(m_commands[i]->rightAnswersCount) + "," +
                QString("%1").arg(m_commands[i]->rating) + "," +
                "[";
        bool comma = false;
        for (quint32 q = 0; q < m_questionCount; ++q) {
            if (m_commands[i]->m_answers[q] == CommandModel::ANSWER_RIGHT) {
                if (comma) {
                    jsonResults += ",";
                } else {
                    comma = true;
                }
                jsonResults += QString("%1").arg(q);
            }
        }
        jsonResults += "],"
                       "\"" + m_commands[i]->m_commandId + "\","
                       "\"" + m_commands[i]->m_commandLocation + "\","
                       "\"" + m_commands[i]->m_tableNumber + "\""
                       "]";
    }
    jsonResults += "]}";
    return jsonResults;
}

void GameModel::exportJSON(QString fileName) const {
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    file.write(get_jsonResults());
    file.close();
}

void GameModel::exportHTML(QString fileName) const {
    QFile source(":/templates/demo.tht");
    source.open(QIODevice::ReadOnly);
    QByteArray templ = source.readAll();
    source.close();

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    templ.replace(QString("{/*Results*/}"), get_jsonResults());
    file.write(templ);
    file.close();
}

void GameModel::exportCSV(QString fileName) const {
    // preparing
    QList<CommandModel*> sorted_commands = m_commands;
    qSort(sorted_commands.begin(), sorted_commands.end(), CommandModel::sort_by_result);

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out.setGenerateByteOrderMark(true);
    out.setCodec("UTF-8");

    out << QString(";Название;Результат;Рейтинг");
    int i; quint32 q;
    for (q = 0; q < m_questionCount; ++q) {
        out << QString(";%1").arg(q+1);
    }
    out << "\r\n";
    out << QString(";;;");
    for (q = 0; q < m_questionCount; ++q) {
        out << QString(";%1").arg(m_questionRating[q]);
    }
    out << "\r\n";

    quint32 prevAnswersCount = -1, prevRating = -1, place = 0;
    for (i = 0; i < sorted_commands.size(); ++i) {
        if (prevAnswersCount != sorted_commands[i]->rightAnswersCount || prevRating != sorted_commands[i]->rating) {
            prevAnswersCount = sorted_commands[i]->rightAnswersCount;
            prevRating = sorted_commands[i]->rating;
            place++;
        }
        out << QString("%4;\"%1\";%2;%3").arg(sorted_commands[i]->commandName()).arg(sorted_commands[i]->rightAnswersCount).arg(sorted_commands[i]->rating).arg(place);
        for (q = 0; q < m_questionCount; ++q) {
            out << QString(";%1").arg(sorted_commands[i]->m_answers[q]==CommandModel::ANSWER_RIGHT?"+":"–");
        }
        out << "\r\n";
    }
    file.close();
}

bool GameModel::load(QString fileName) {
    m_autoSave = false;
    setQuestionCount(0);

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    QXmlStreamReader xml(&file);
    // Read this:
    //  http://www.developer.nokia.com/Community/Wiki/QXmlStreamReader_to_parse_XML_in_Qt

    quint32 l_questionCount = 0;
    bool l_autoSave = false;
    bool l_finished = false;
    int l_fixedQuestion = 0;
    QList<CommandModel *> l_commands;
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartDocument) {
            continue;
        }

        if (token == QXmlStreamReader::StartElement) {
            if (xml.name() == "QuestionCount") {
                xml.readNext();
                l_questionCount = xml.text().toInt() ;
            }
            if (xml.name() == "AutoSave") {
                xml.readNext();
                l_autoSave = xml.text().toInt();
            }
            if (xml.name() == "FixedQuestion") {
                xml.readNext();
                l_fixedQuestion = xml.text().toInt();
            }
            if (xml.name() == "Finished") {
                xml.readNext();
                l_finished = xml.text().toInt();
            }
            if (xml.name() == "Command") {
                CommandModel *command = new CommandModel;
                while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "Command")) {
                    if (xml.tokenType() == QXmlStreamReader::StartElement) {
                        if (xml.name() == "Name") {
                            xml.readNext();
                            command->setCommandName(xml.text().toString());
                        } else if (xml.name() == "Id") {
                            xml.readNext();
                            command->m_commandId = xml.text().toString();
                        } else if (xml.name() == "Location") {
                            xml.readNext();
                            command->m_commandLocation = xml.text().toString();
                        } else if (xml.name() == "Table") {
                            xml.readNext();
                            command->m_tableNumber = xml.text().toString();
                        } else if (xml.name() == "Tag") {
                            xml.readNext();
                            command->m_tags << xml.text().toString();
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

    setQuestionCount(l_questionCount);
    m_autoSave = l_autoSave;
    set_finished(l_finished);
    fixQuestion(l_fixedQuestion);

    for (quint32 q = 0; q < l_questionCount; ++q) {
        m_questionRating[q] = 1;
        for (int i = 0; i < m_commands.size(); ++i) {
            if (m_commands[i]->m_answers[q] == CommandModel::ANSWER_RIGHT) {
                m_commands[i]->rightAnswersCount++;
            } else {
                m_questionRating[q]++;
            }
        }

        for (int i = 0; i < m_commands.size(); ++i) {
            if (m_commands[i]->m_answers[q] == CommandModel::ANSWER_RIGHT) {
                m_commands[i]->rating += m_questionRating[q];
            }
        }
    }

    endResetModel();
    if (fileName != m_fileName) {
        m_fileName = fileName;
        QFileInfo fileInfo(fileName);
        m_title = fileInfo.baseName();
        emit titleChanged();
    }
    m_modified = false;
    return true;
}

void GameModel::click(int col, int row) {
    if (col > 0) {
        invertCommandResult(row, col-1);
    }
}

void GameModel::empty(int col, int row) {
    if (col > 0) {
        setCommandResult(row, col-1,  CommandModel::ANSWER_WRONG);
    }
}

bool GameModel::readFromScanner(const QString &text, QModelIndexList *affectedIndexes) {
    if (m_fixedQuestion == 0) {
        return false;
    }
    quint32 hash, q;
    bool find_by_hash = fromBarcodeText(text, &hash, &q);
    q = m_fixedQuestion;
    bool result = false;
    bool matched = false;
    for (int i = 0; i < m_commands.size(); ++i) {
        if (find_by_hash) {
            matched = hash == m_commands[i]->commandNameHash();
        } else {
            matched = text == "#" + m_commands[i]->m_tableNumber;
        }
        if (matched) {
            setCommandResult(i, q-1, CommandModel::ANSWER_RIGHT);
            affectedIndexes->append(index(i, q));
            result = true;
        }
    }
    return result;
}

void GameModel::invertCommandResult(int commandNumber, quint32 question) {
    if (commandNumber >= m_commands.size() || question >= (quint32)m_commands[commandNumber]->m_answers.size()) {
        return;
    }
    CommandModel::CommandAnswer *a = &m_commands[commandNumber]->m_answers[question];
    switch (*a) {
    case CommandModel::ANSWER_RIGHT:
        *a = CommandModel::ANSWER_WRONG;
        m_commands[commandNumber]->rightAnswersCount--;
        m_commands[commandNumber]->rating -= m_questionRating[question]++;
        break;
    case CommandModel::ANSWER_WRONG:
    case CommandModel::ANSWER_UNKNOWN:
        *a = CommandModel::ANSWER_RIGHT;
        m_commands[commandNumber]->rightAnswersCount++;
        m_commands[commandNumber]->rating += --m_questionRating[question];
    }

    QModelIndex i = createIndex(commandNumber, question+1);
    dataChanged(i, i);
    i = createIndex(commandNumber, 0);
    dataChanged(i, i);

    for (int c = 0; c < m_commands.size(); ++c) {
        if (c != commandNumber && m_commands[c]->m_answers[question] == CommandModel::ANSWER_RIGHT) {
            if (*a == CommandModel::ANSWER_RIGHT) {
                m_commands[c]->rating--;
            } else {
                m_commands[c]->rating++;
            }
            i = createIndex(c, 0);
            dataChanged(i, i);
        }
    }
    m_modified = true;
    autoSave();
}

void GameModel::setCommandResult(int commandNumber, quint32 question, CommandModel::CommandAnswer value)
{
    if (commandNumber >= m_commands.size() || question >= (quint32)m_commands[commandNumber]->m_answers.size()) {
        return;
    }
    if (m_commands[commandNumber]->m_answers[question] == value) {
        return;
    }
    invertCommandResult(commandNumber, question);
}

void GameModel::removeCommandsAtRows(const QSet<int>& rows)
{
    foreach(int row, rows) {
        // first of all: need to upgrade rating
        //  and results of other commands
        for (quint32 q = 0; q < m_questionCount; ++q) {
            if (m_commands[row]->m_answers[q] != CommandModel::ANSWER_RIGHT) {
                m_questionRating[q]--;
                for (int c = 0; c < m_commands.size(); ++c) {
                    if (m_commands[c]->m_answers[q] == CommandModel::ANSWER_RIGHT) {
                        // rating going down
                        m_commands[c]->rating--;
                    }
                }
            }
        }
    }
    QList<int> sorted_rows = rows.toList();
    qSort(sorted_rows.begin(), sorted_rows.end(), std::greater<int>());
    beginResetModel();
    foreach(int row, sorted_rows) {
        m_commands.removeAt(row);
    }
    endResetModel();
    m_modified = true;
    autoSave();
}

void GameModel::fixQuestion(int column)
{
    int prev = m_fixedQuestion;
    if (column <= 0 || (quint32)column > m_questionCount) {
        m_fixedQuestion = 0;
    } else {
        m_fixedQuestion = column;
    }
    if (prev == m_fixedQuestion) {
        return;
    }
    emit fixedQuestionChanged(m_fixedQuestion);
    emit layoutChanged();
    m_modified = true;
    autoSave();
}

void GameModel::sort_by_criteria(SortCriteria criteria)
{
    switch(criteria) {
        case SORT_BY_TITLE:
            qSort(m_commands.begin(), m_commands.end(), CommandModel::sort_by_name);
            break;
        case SORT_BY_TABLE:
            qSort(m_commands.begin(), m_commands.end(), CommandModel::sort_by_table);
            break;
        case SORT_BY_RESULT:
            qSort(m_commands.begin(), m_commands.end(), CommandModel::sort_by_result);
            break;
        default:
            break;
    }

    layoutChanged();
}

void GameModel::set_finished(bool value) {
    if (value == m_finished) return;

    m_finished = value;
    emit finishedChanged();
    m_modified = true;
    autoSave();
}
