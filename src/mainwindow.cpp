#include <QtDebug>
#include <QMenuBar>
#include <QFileDialog>
#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QTableView>
#include <QTimer>
#include <QKeyEvent>

#include <string>

#include "mainwindow.h"
#include "addcommanddialog.h"
#include "questioncountdialog.h"
#include "tablepainterdelegate.h"
#include "keyreceiver.h"

using std::string;

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    m_model = new GameModel;

    createActions();
    createMenus();

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(QIcon(":/img/wwwtool-old.ico"));

    connect(m_model, SIGNAL(titleChanged()), this, SLOT(titleChanged()));
    connect(m_model, SIGNAL(questionCountChanged(int)), this, SLOT(questionCountChanged(int)));

    gameTable = new QTableView;
    originalPalette = gameTable->palette();
    gameTable->horizontalHeader()->setDefaultSectionSize(40);
    gameTable->setModel(m_model);
    gameTable->setItemDelegate(new TablePainterDelegate(m_model));

    gameTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    connect(gameTable->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged(QItemSelection,QItemSelection)));

    KeyReceiver *escape_receiver = new KeyReceiver({Qt::Key_Escape});
    connect(escape_receiver, SIGNAL(acceptedKeyPressed()), this, SLOT(escapePressed()));
    gameTable->installEventFilter(escape_receiver);

    KeyReceiver *space_receiver = new KeyReceiver({Qt::Key_Space});
    connect(space_receiver, SIGNAL(acceptedKeyPressed()), this, SLOT(spacePressed()));
    gameTable->installEventFilter(space_receiver);

    QHBoxLayout *inputHelperLayout = new QHBoxLayout;

    QGroupBox *inputHelperBox = new QGroupBox;

    QLabel *fixedQuestionLabel = new QLabel(tr("Вопрос:"));
    fixedQuestionBox = new QComboBox;
    fixedQuestionBox->setEditable(true);
    connect(fixedQuestionBox, SIGNAL(currentIndexChanged(int)), this, SLOT(fixQuestion(int)));
    connect(m_model, SIGNAL(fixedQuestionChanged(int)), fixedQuestionBox, SLOT(setCurrentIndex(int)));

    inputHelperLayout->addWidget(fixedQuestionLabel);
    inputHelperLayout->addWidget(fixedQuestionBox);

    helperLabel = new QLabel(tr("Фильтр команд:"));
    helperLineEdit = new QLineEdit;
    helperLabel->setBuddy(helperLineEdit);

    KeyReceiver *enter_receiver = new KeyReceiver({Qt::Key_Enter, Qt::Key_Return});
    connect(enter_receiver, SIGNAL(acceptedKeyPressed()), this, SLOT(helperLineEditChanged()));
    helperLineEdit->installEventFilter(enter_receiver);

    inputHelperLayout->addWidget(helperLabel);
    inputHelperLayout->addWidget(helperLineEdit);

    inputHelperBox->setLayout(inputHelperLayout);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
//    layout->setMenuBar(menuBar);
    layout->addWidget(gameTable);
    layout->addWidget(inputHelperBox);

    setLayout(layout);



    resize(700, 500);

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerFinished()));

    player = new QMediaPlayer;
}

MainWindow::~MainWindow()
{
    delete gameTable;
    delete m_model;
    delete timer;
    delete player;
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

    finishedGameAct = new QAction(tr("Игра завершена"), this);
    finishedGameAct->setCheckable(true);
    finishedGameAct->setChecked(m_model->finished());
    connect(finishedGameAct, SIGNAL(triggered()), this, SLOT(changeFinished()));

    printBlanksAct = new QAction(tr("Бланки ответов..."), this);
    printBlanksAct->setShortcut(QKeySequence::Print);
    connect(printBlanksAct, SIGNAL(triggered()), this, SLOT(printBlanks()));

    printTablesAct = new QAction(tr("Бланки рассадки..."), this);
    connect(printTablesAct, SIGNAL(triggered()), this, SLOT(printTables()));

    addCommandAct = new QAction(tr("Добавить команду..."), this);
    connect(addCommandAct, SIGNAL(triggered()), this, SLOT(addCommand()));

    deleteCommandAct = new QAction(tr("Удалить команду..."), this);
    deleteCommandAct->setEnabled(false);
    connect(deleteCommandAct, SIGNAL(triggered()), this, SLOT(deleteCommand()));

    sortByTitleAct = new QAction(tr("Сортировать по имени"), this);
    sortByTitleAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_0));
    connect(sortByTitleAct, SIGNAL(triggered()), this, SLOT(sortByTitle()));

    sortByTableAct = new QAction(tr("Сортировать по столику"), this);
    sortByTableAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
    connect(sortByTableAct, SIGNAL(triggered()), this, SLOT(sortByTable()));

    sortByResultAct = new QAction(tr("Сортировать по результату"), this);
    sortByResultAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
    connect(sortByResultAct, SIGNAL(triggered()), this, SLOT(sortByResult()));

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
    QMenuBar *menuBar = new QMenuBar(this);

    QMenu *gameMenu = menuBar->addMenu(tr("Игра"));
    gameMenu->addAction(newGameAct);
    gameMenu->addAction(saveGameAct);
    gameMenu->addAction(loadGameAct);

    QMenu *exportMenu = gameMenu->addMenu(tr("Экспорт"));
    exportMenu->addAction(exportHTMLAct);
    exportMenu->addAction(exportCSVAct);

    gameMenu->addAction(closeGameAct);
    gameMenu->addSeparator();
    gameMenu->addAction(finishedGameAct);
    gameMenu->addSeparator();
    gameMenu->addAction(printBlanksAct);
    gameMenu->addAction(printTablesAct);

    QMenu *commandsMenu = menuBar->addMenu(tr("Команды"));
    commandsMenu->addAction(addCommandAct);
    commandsMenu->addAction(deleteCommandAct);
    commandsMenu->addSeparator();
    commandsMenu->addAction(sortByTitleAct);
    commandsMenu->addAction(sortByTableAct);
    commandsMenu->addAction(sortByResultAct);

    QMenu *questionsMenu = menuBar->addMenu(tr("Вопросы"));
    questionsMenu->addAction(questionCountAct);

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

void MainWindow::printTables()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export tables"), QString(), tr("PDF (*.pdf)"));

    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    m_model->printTables(&printer);
}



void MainWindow::addCommand()
{
    AddCommandDialog addCommandDialog(m_model, this);
    addCommandDialog.exec();
}

void MainWindow::deleteCommand()
{
    QModelIndexList selection = gameTable->selectionModel()->selection().indexes();
    if (!selection.isEmpty()) {
        QSet<int> selected_rows;
        foreach(QModelIndex i, selection) {
            selected_rows.insert(i.row());
        }
        if (QMessageBox::Yes == QMessageBox::question(this, windowTitle(), tr("Удалить выбранные команды?"))) {
            m_model->removeCommandsAtRows(selected_rows);
        }
    } else {
        deleteCommandAct->setEnabled(false);
    }
}

void MainWindow::questionCount()
{
    QuestionCountDialog dialog(m_model, this);
    dialog.exec();
}

void MainWindow::spacePressed() {
    qDebug() << "Space";
    for (auto i: gameTable->selectionModel()->selection().indexes()) {
        m_model->click(i.column(), i.row());
    }
}


void MainWindow::escapePressed() {
    qDebug() << "Escape";
    for (auto i: gameTable->selectionModel()->selection().indexes()) {
        m_model->empty(i.column(), i.row());
    }
}

void MainWindow::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
    Q_UNUSED(deselected);
    deleteCommandAct->setEnabled(!selected.isEmpty());
}

void MainWindow::timerFinished()
{
    gameTable->setPalette(originalPalette);
}

void MainWindow::titleChanged()
{
    setWindowTitle(m_model->title());
}

void MainWindow::finishedChanged()
{
    finishedGameAct->setChecked(m_model->finished());
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

void MainWindow::sortByTitle()
{
    m_model->sort_by_criteria(GameModel::SORT_BY_TITLE);
}

void MainWindow::sortByTable()
{
    m_model->sort_by_criteria(GameModel::SORT_BY_TABLE);
}

void MainWindow::sortByResult()
{
    m_model->sort_by_criteria(GameModel::SORT_BY_RESULT);
}

void MainWindow::fixQuestion(int number) {    
    m_model->fixQuestion(number);
    if (number > 0) {
        gameTable->scrollTo(m_model->index(0, number));
        helperLabel->setText(tr("Код или #стол команды:"));
    } else {
        helperLabel->setText(tr("Фильтр команд:"));
    }
    helperLineEdit->setFocus();
}

void MainWindow::changeFinished() {
    m_model->set_finished(!m_model->finished());
}

void MainWindow::questionCountChanged(int count) {
    int old_value = fixedQuestionBox->currentIndex();
    fixedQuestionBox->clear();
    fixedQuestionBox->addItem("-");
    for (int i=0; i<count; ++i) {
        fixedQuestionBox->addItem(QString("%1").arg(i+1));
    }
    fixedQuestionBox->setCurrentIndex(old_value);
}

void MainWindow::helperLineEditChanged() {
    helperLineEdit->selectAll();
    QString text = helperLineEdit->text();

    if (m_model->fixedQuestion() == 0) {
        filterCommands(text);
    } else {
        processScannedText(text);
    }
}

void MainWindow::filterCommands(const QString& value) {
    if (value.isEmpty()) {
        return;
    }
    gameTable->clearSelection();
    gameTable->setSelectionMode(QAbstractItemView::MultiSelection);
    int found_row = -1;
    for (int row=0; row < m_model->rowCount(); ++row) {
        QString title = m_model->headerData(row, Qt::Vertical, Qt::DisplayRole).toString().toLower();
        if (title.contains(value.toLower())) {
            gameTable->selectRow(row);
            if (found_row == -1) {
                found_row = row;
            }
        }
    }
    gameTable->setSelectionMode(QAbstractItemView::SingleSelection);
    if (found_row >= 0) {
        gameTable->scrollTo(m_model->index(found_row, 0));
        gameTable->setFocus();
    }
}

void MainWindow::processScannedText(const QString &value) {
    QPalette p = originalPalette;
    QModelIndexList affectedIndexes;
    if (m_model->readFromScanner(value, &affectedIndexes)) {
        gameTable->clearSelection();
        gameTable->setSelectionMode(QAbstractItemView::MultiSelection);
        for(auto index: affectedIndexes) {
            gameTable->scrollTo(index);
            gameTable->selectionModel()->select(index, QItemSelectionModel::Select);
        }
        gameTable->setSelectionMode(QAbstractItemView::SingleSelection);
        player->setMedia(QUrl("qrc:/positive.wav"));
        p.setColor(QPalette::Base, QColor("green"));
    } else {
        player->setMedia(QUrl("qrc:/negative.wav"));
        p.setColor(QPalette::Base, QColor("red"));
    }
    gameTable->setPalette(p);
    timer->stop();
    timer->start(200);
    player->setVolume(100);
    player->play();
}
