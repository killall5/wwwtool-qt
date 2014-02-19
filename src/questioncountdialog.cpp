#include "questioncountdialog.h"

#include <QGridLayout>

QuestionCountDialog::QuestionCountDialog(GameModel *model, QWidget *parent) :
    QDialog(parent), m_model(model)
{
    questionLabel = new QLabel(tr("Вопросов"));
    questionEdit = new QLineEdit;
    questionValidator = new QIntValidator;
    questionValidator->setBottom(0);
    questionEdit->setValidator(questionValidator);

    QString questionCount = QString("%1").arg(model->questionCount());
    questionEdit->setText(questionCount);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addWidget(questionLabel, 0, 0);
    mainLayout->addWidget(questionEdit, 0, 1);
    mainLayout->addWidget(buttonBox, 1, 0, 1, 2);

    setLayout(mainLayout);
    setModal(true);
    setWindowModality(Qt::WindowModal);
    setWindowTitle(tr("Количество вопросов"));
}

QuestionCountDialog::~QuestionCountDialog()
{
    delete questionEdit;
    delete questionValidator;
    delete questionLabel;
    delete buttonBox;
}

void QuestionCountDialog::accept()
{
    m_model->setQuestionCount(questionEdit->text().toInt());
    close();
}
