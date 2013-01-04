#include "mainmenu.h"
#include "mainwindow.h"
#include <QApplication>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QtDebug>
#include <cmath>
#include "code128.h"

MainMenu::MainMenu(QWidget *parent) :
    QMenuBar(parent)
{
    QAction *newGameAct = new QAction(tr("Новая игра"), this);
    newGameAct->setShortcut(QKeySequence::New);
    connect(newGameAct, SIGNAL(triggered()), this, SLOT(newGame()));

    QAction *saveGameAct = new QAction(tr("Сохранить"), this);
    saveGameAct->setShortcut(QKeySequence::Save);
    connect(saveGameAct, SIGNAL(triggered()), this, SLOT(saveGame()));

    QAction *printBlanksAct = new QAction(tr("Печать бланков..."), this);
    printBlanksAct->setShortcut(QKeySequence::Print);
    connect(printBlanksAct, SIGNAL(triggered()), this, SLOT(printBlanks()));

    QMenu *gameMenu = addMenu("Игра");
    gameMenu->addAction(newGameAct);
    gameMenu->addAction(saveGameAct);
    gameMenu->addSeparator();
    gameMenu->addAction(printBlanksAct);

//    QMenu *help = addMenu(tr("Help"));

}

void MainMenu::newGame()
{
    MainWindow *w = new MainWindow; // autorelease!
    w->show();
}

void MainMenu::saveGame()
{
  QMainWindow *w = reinterpret_cast<MainWindow*>(qApp->activeWindow());
  if (w) {
      w->setWindowTitle("hohohoh");
  }
}

void MainMenu::printBlanks()
{
    QFont barcodeFont = QFont("Code 128", 20, QFont::Normal);

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("/Users/mirror/Desktop/blank.pdf");

    const int COLS=3;
    const int ROWS=4;

    const int commandsCount = 2;
    const int questionCount = 10;


    QPainter painter;
    painter.begin(&printer);

    int pageWidth = printer.width();
    int cellWidth = pageWidth/COLS;
    int pageHeight = printer.height();
    int cellHeight = pageHeight/ROWS;

    QPen defaultPen(QColor(0, 0, 0));
    defaultPen.setWidth(2);

    QPen questionPen(QColor(0, 0, 0, 64));

    int pagesCount = ceil(questionCount*1.0/COLS/ROWS);
    QFont questionFont = painter.font();
    qreal ratio = cellWidth/2 / painter.fontMetrics().boundingRect(QString("%1").arg(questionCount)).size().width();
    questionFont.setPointSizeF(questionFont.pointSizeF()*ratio);


    QString commandName = "Длинное название";
    for (int command=0; command < commandsCount; ++command) {

        QFont commandNameFont(painter.font());
        ratio = cellWidth*0.85 / painter.fontMetrics().boundingRect(commandName).size().width();
        commandNameFont.setPointSizeF(qMin(14.0, painter.font().pointSizeF()*ratio));

        for (int page=0; page < pagesCount; ++page) {
            int col, row, q;
            painter.setPen(defaultPen);
            for (col=1; col < COLS; ++col) {
                painter.drawLine(col*cellWidth, 0, col*cellWidth, pageHeight);
            }
            for (row=1; row < ROWS; ++row) {
                painter.drawLine(0, row*cellHeight, pageWidth, row*cellHeight);
            }

            for (col=0; col < COLS; ++col) {
                for (row=0; row < ROWS; ++row) {
                    q=page*COLS*ROWS+col+row*COLS+1;

                    painter.setPen(defaultPen);
                    painter.setFont(commandNameFont);
                    painter.drawText(col*cellWidth + cellWidth/20, row*cellHeight + cellHeight/20, cellWidth*0.95, cellHeight*0.95, Qt::AlignTop, commandName);
                    QRect bounds = painter.fontMetrics().boundingRect(commandName);

                    painter.setFont(barcodeFont);
                    QString code128 = Code128::encodeToCode128(QString("98736820124345332442"));
                    qDebug() << code128;
                    painter.drawText(col*cellWidth + cellWidth/20, row*cellHeight + cellHeight/20 + bounds.size().height(), cellWidth*0.95, cellHeight*0.95, Qt::AlignTop, code128);

                    painter.setPen(questionPen);
                    painter.setFont(questionFont);
                    painter.drawText(col*cellWidth, row*cellHeight, cellWidth, cellHeight, Qt::AlignCenter, QString("%1").arg(q));
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

    painter.end();
}
