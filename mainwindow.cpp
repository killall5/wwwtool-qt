#include "mainwindow.h"
#include <QtDebug>
#include <QMenuBar>
#include <QFileDialog>
#include "addcommanddialog.h"
#include "questioncountdialog.h"
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    init();
}

void MainWindow::init()
{
    setAttribute(Qt::WA_DeleteOnClose);

    m_model = new GameModel(this);

    gameTable = new QTableView(this);
    //gameTable->setSelectionMode(QAbstractItemView::SingleSelection);
    gameTable->horizontalHeader()->setDefaultSectionSize(40);
    gameTable->setModel(m_model);

    setCentralWidget(gameTable);

    createActions();
    createMenus();
    resize(700, 500);

    ScannerManager::instance()->addListener(this);
    kpe = new KeyPressEater(this);
    connect(kpe, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(keyPressEvent(QKeyEvent*)));
    gameTable->installEventFilter(kpe);
}

MainWindow::~MainWindow()
{
    ScannerManager::instance()->removeListener(this);
    gameTable->removeEventFilter(kpe);
    delete kpe;
    delete gameTable;
    delete m_model;
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

    exportResultsAct = new QAction(tr("Экспорт результатов..."), this);
    connect(exportResultsAct, SIGNAL(triggered()), this, SLOT(exportResults()));
}

void MainWindow::createMenus()
{
    QMenu *gameMenu = menuBar()->addMenu("Игра");
    gameMenu->addAction(newGameAct);
    gameMenu->addAction(saveGameAct);
    gameMenu->addAction(loadGameAct);
    gameMenu->addAction(exportResultsAct);
    gameMenu->addAction(closeGameAct);
    gameMenu->addSeparator();
    gameMenu->addAction(printBlanksAct);

    QMenu *commandsMenu = menuBar()->addMenu("Команды");
    commandsMenu->addAction(addCommandAct);
    commandsMenu->addAction(deleteCommandAct);

    QMenu *questionsMenu = menuBar()->addMenu("Вопросы");
    questionsMenu->addAction(questionCountAct);

    QMenu *help = menuBar()->addMenu(tr("Help"));
    Q_UNUSED(help);
}

void MainWindow::newGame()
{
    MainWindow *w = new MainWindow();
    w->show();
}

void MainWindow::loadGame()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load game"), QString(), tr("WWWTool game (*.wg)"));

    if (fileName.isEmpty()) return;

    m_model->load(fileName);
    QFileInfo fileInfo(fileName);
    setWindowTitle(fileInfo.baseName());
}

void MainWindow::saveGame()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save game"), QString(), tr("WWWTool game (*.wg)"));

    if (fileName.isEmpty()) return;

    m_model->save(fileName);

    QFileInfo fileInfo(fileName);
    setWindowTitle(fileInfo.baseName());
}

void MainWindow::exportResults()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export results"), QString(), tr("HTML (*.html)"));

    if (fileName.isEmpty()) return;

    m_model->exportResults(fileName);
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

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QModelIndexList s = gameTable->selectionModel()->selection().indexes();
    if (event->key() == 32 && !s.empty()) {
        m_model->click(s.begin()->column(), s.begin()->row());
    }
}

void MainWindow::onTextScanned(const QString &text)
{
    m_model->readFromScanner(text);
}
