#include "mainwindow.h"
#include <QtDebug>
#include <QMenuBar>
#include <QFileDialog>
#include "addcommanddialog.h"
#include "questioncountdialog.h"
#include "tablepainterdelegate.h"
#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    init();
}

void MainWindow::init()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(QIcon(":/img/wwwtool-old.ico"));

    m_model = new GameModel(this);
    connect(m_model, SIGNAL(titleChanged()), this, SLOT(titleChanged()));

    gameTable = new QTableView(this);
    originalPalette = gameTable->palette();
    gameTable->horizontalHeader()->setDefaultSectionSize(40);
    gameTable->setModel(m_model);
    gameTable->setItemDelegate(new TablePainterDelegate);

    gameTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    setCentralWidget(gameTable);

    createActions();
    createMenus();
    resize(700, 500);

    ScannerManager::instance()->addListener(this);
    kpe = new KeyPressEater(this);
    connect(kpe, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(keyPressEvent(QKeyEvent*)));
    gameTable->installEventFilter(kpe);

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerFinished()));
}

MainWindow::~MainWindow()
{
    ScannerManager::instance()->removeListener(this);
    gameTable->removeEventFilter(kpe);
    delete kpe;
    delete gameTable;
    delete m_model;
    delete timer;
}

void MainWindow::createActions()
{
    newGameAct = new QAction(tr("Новая игра"), this);
    newGameAct->setShortcut(QKeySequence::New);
    connect(newGameAct, SIGNAL(triggered()), this, SLOT(newGame()));

    saveGameAct = new QAction(tr("Сохранить..."), this);
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

    exportHTMLAct = new QAction(tr("HMTL..."), this);
    connect(exportHTMLAct, SIGNAL(triggered()), this, SLOT(exportHTML()));

    exportCSVAct = new QAction(tr("CSV..."), this);
    connect(exportCSVAct, SIGNAL(triggered()), this, SLOT(exportCSV()));
}

void MainWindow::createMenus()
{
    QMenu *gameMenu = menuBar()->addMenu(tr("Игра"));
    gameMenu->addAction(newGameAct);
    gameMenu->addAction(saveGameAct);
    gameMenu->addAction(loadGameAct);

    QMenu *exportMenu = gameMenu->addMenu(tr("Экспорт"));
    exportMenu->addAction(exportHTMLAct);
    exportMenu->addAction(exportCSVAct);

    gameMenu->addAction(closeGameAct);
    gameMenu->addSeparator();
    gameMenu->addAction(printBlanksAct);

    QMenu *commandsMenu = menuBar()->addMenu(tr("Команды"));
    commandsMenu->addAction(addCommandAct);
    commandsMenu->addAction(deleteCommandAct);

    QMenu *questionsMenu = menuBar()->addMenu(tr("Вопросы"));
    questionsMenu->addAction(questionCountAct);

//    QMenu *help = menuBar()->addMenu(tr("Help"));
//    Q_UNUSED(help);
}

void MainWindow::newGame()
{
    MainWindow *w = new MainWindow();
    QPoint position = pos() + QPoint(40, 40);
    QRect available = QApplication::desktop()->availableGeometry(this);
    QRect frame = frameGeometry();
     if (position.x() + frame.width() > available.right())
         position.rx() = available.left();
     if (position.y() + frame.height() > available.bottom() - 20)
         position.ry() = available.top();
    w->move(position);
    w->show();
}

void MainWindow::loadGame()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load game"), QString(), tr("WWWTool game (*.wg)"));

    if (fileName.isEmpty()) return;

    m_model->load(fileName);
}

void MainWindow::saveGame()
{
    if (m_model->title().isEmpty()) {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save game"), QString(), tr("WWWTool game (*.wg)"));
        if (fileName.isEmpty()) return;
        m_model->save(fileName);
    } else {
        m_model->save();
    }
}

void MainWindow::exportHTML()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export results"), QString(), tr("HTML (*.html)"));
    if (fileName.isEmpty()) return;
    m_model->exportHTML(fileName);
}

void MainWindow::exportCSV()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export results"), QString(), tr("CSV (*.csv)"));
    if (fileName.isEmpty()) return;
    m_model->exportCSV(fileName);
}


void MainWindow::printBlanks()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export blanks"), QString(), tr("PDF (*.pdf)"));

    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    m_model->printBlanks(&printer);
}

void MainWindow::addCommand()
{
    AddCommandDialog addCommandDialog(m_model, this);
    addCommandDialog.exec();
}

void MainWindow::deleteCommand()
{

}

void MainWindow::questionCount()
{
    QuestionCountDialog dialog(m_model, this);
    dialog.exec();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QModelIndexList s = gameTable->selectionModel()->selection().indexes();
    if (event->key() == 32 && !s.empty()) {
        m_model->click(s.begin()->column(), s.begin()->row());
    }
}

void MainWindow::onTextScanned(const QString &text)
{
    if (text.length() < 2) {
        return;
    }

    QPalette p = originalPalette;
    if (m_model->readFromScanner(text)) {
        p.setColor(QPalette::Base, QColor("green"));
    } else {
        p.setColor(QPalette::Base, QColor("red"));
    }
    gameTable->setPalette(p);
    timer->start(100);
}

void MainWindow::timerFinished()
{
    gameTable->setPalette(originalPalette);
}

void MainWindow::titleChanged()
{
    setWindowTitle(m_model->title());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::Yes;
    if (m_model->modified()) {
        resBtn = QMessageBox::question( this, windowTitle(),
                                        tr("Сохранить изменения?"),
                                        QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                        QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes) {
            saveGame();
        }
    }
    if (resBtn == QMessageBox::Cancel) {
        event->ignore();
    } else {
        event->accept();
    }
}
