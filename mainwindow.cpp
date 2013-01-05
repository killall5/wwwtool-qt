#include "mainwindow.h"
#include <QTableWidget>
#include <QTableView>
#include <QPainter>
#include <QPrinter>
#include <QtDebug>
#include <QAction>
#include <QMenuBar>
#include <cmath>
#include "code128.h"

#include <QFileDialog>
#include "addcommanddialog.h"
#include "questioncountdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    init();
}

void MainWindow::init()
{
    setAttribute(Qt::WA_DeleteOnClose);

    m_model = new GameModel(this);

    QTableView *gameTable = new QTableView(this);
    gameTable->setModel(m_model);
    gameTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    setCentralWidget(gameTable);

    createActions();
    createMenus();
    resize(700, 500);
}

void MainWindow::createActions()
{
    newGameAct = new QAction(tr("Новая игра"), this);
    newGameAct->setShortcut(QKeySequence::New);
    connect(newGameAct, SIGNAL(triggered()), this, SLOT(newGame()));

    saveGameAct = new QAction(tr("Сохранить"), this);
    saveGameAct->setShortcut(QKeySequence::Save);
    connect(saveGameAct, SIGNAL(triggered()), this, SLOT(saveGame()));

    loadGameAct = new QAction(tr("Открыть..."), this);
    loadGameAct->setShortcut(QKeySequence::Open);
    connect(loadGameAct, SIGNAL(triggered()), this, SLOT(loadGame()));

    closeGameAct = new QAction(tr("Закрыть"), this);
    closeGameAct->setShortcut(QKeySequence::Close);
    connect(closeGameAct, SIGNAL(triggered()), this, SLOT(close()));

    printBlanksAct = new QAction(tr("Бланки ответов..."), this);
    printBlanksAct->setShortcut(QKeySequence::Print);
    connect(printBlanksAct, SIGNAL(triggered()), this, SLOT(printBlanks()));

    addCommandAct = new QAction(tr("Добавить команду..."), this);
    connect(addCommandAct, SIGNAL(triggered()), this, SLOT(addCommand()));

    deleteCommandAct = new QAction(tr("Удалить команду..."), this);
    connect(deleteCommandAct, SIGNAL(triggered()), this, SLOT(deleteCommand()));

    questionCountAct = new QAction(tr("Количество вопросов..."), this);
    questionCountAct->setShortcut(tr("Ctrl+E"));
    connect(questionCountAct, SIGNAL(triggered()), this, SLOT(questionCount()));
}

void MainWindow::createMenus()
{
    QMenu *gameMenu = menuBar()->addMenu("Игра");
    gameMenu->addAction(newGameAct);
    gameMenu->addAction(saveGameAct);
    gameMenu->addAction(loadGameAct);
    gameMenu->addAction(closeGameAct);
    gameMenu->addSeparator();
    gameMenu->addAction(printBlanksAct);

    QMenu *commandsMenu = menuBar()->addMenu("Команды");
    commandsMenu->addAction(addCommandAct);
    commandsMenu->addAction(deleteCommandAct);

    QMenu *questionsMenu = menuBar()->addMenu("Вопросы");
    questionsMenu->addAction(questionCountAct);

//    QMenu *help = menuBar()->addMenu(tr("Help"));


}

void MainWindow::newGame()
{
    MainWindow *w = new MainWindow();
    w->show();
}

void MainWindow::loadGame()
{

}

void MainWindow::saveGame()
{

}

void MainWindow::printBlanks()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export blanks"), QString(), tr("PDF (*.pdf)"));

    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    const int COLS=3;
    const int ROWS=4;

    const int commandsCount = 1;
    const int questionCount = 15;


    QPainter painter(&printer);

    qreal pageWidth = printer.width();
    qreal cellWidth = pageWidth/COLS;
    qreal pageHeight = printer.height();
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

    int pagesCount = ceil(questionCount*1.0/COLS/ROWS);
    QFont questionFont = painter.font();
    qreal ratio = cellHeight/2 / painter.fontMetrics().boundingRect(QString("%1").arg(questionCount)).size().height();
    questionFont.setPointSizeF(questionFont.pointSizeF()*ratio);

    painter.setBrush(QBrush(Qt::SolidPattern));

    QString commandName = "Пакет не читал, но редакторов осуждаю";
    for (int command=0; command < commandsCount; ++command) {

        QFont commandNameFont(painter.font());
        ratio = cellWidth*0.85 / painter.fontMetrics().boundingRect(commandName).size().width();
        commandNameFont.setPointSizeF(qMin(14.0, painter.font().pointSizeF()*ratio));

        for (int page=0; page < pagesCount; ++page) {
            int col, row, q;
            painter.setPen(defaultVPen);
            for (col=1; col < COLS; ++col) {
                painter.drawLine(col*cellWidth, 0, col*cellWidth, pageHeight);
            }
            painter.setPen(defaultHPen);
            for (row=1; row < ROWS; ++row) {
                painter.drawLine(0, row*cellHeight, pageWidth, row*cellHeight);
            }

            for (col=0; col < COLS; ++col) {
                for (row=0; row < ROWS; ++row) {
                    q=page*COLS*ROWS+col+row*COLS+1;

                    painter.setPen(defaultPen);
                    painter.setFont(commandNameFont);
                    QRect bounds = painter.fontMetrics().boundingRect(commandName);
                    painter.drawText(col*cellWidth + cellWidth/20, row*cellHeight + cellHeight/20, cellWidth*0.90, bounds.height(), Qt::AlignTop|Qt::AlignHCenter, commandName);

                    QRectF barcodeRect(col*cellWidth + cellWidth/4, (row+1)*cellHeight - cellHeight/20 - cellWidth/6, cellWidth/2, cellWidth/6);
                    if (q <= questionCount) {
                        Code128::drawBarcode("0-0-0", painter, barcodeRect);

                        painter.setPen(questionPen);
                        painter.setFont(questionFont);
                        painter.drawText(col*cellWidth, row*cellHeight + cellHeight/20 + bounds.height(), cellWidth, barcodeRect.top() - (row*cellHeight + cellHeight/20 + bounds.height()), Qt::AlignCenter, QString("%1").arg(q));
                    } else {
                        Code128::drawBarcode("0-0-0", painter, barcodeRect);
                    }
                }
            }

            if (page != pagesCount-1) {
                printer.newPage();
            }
        }

        if (command != commandsCount-1) {
            printer.newPage();
        }
    }
}

void MainWindow::addCommand()
{
    AddCommandDialog *addCommandDialog = new AddCommandDialog(m_model, this);
//    connect()
    addCommandDialog->exec();
}

void MainWindow::deleteCommand()
{

}

void MainWindow::questionCount()
{
    QuestionCountDialog dialog(m_model, this);
    dialog.exec();
}
