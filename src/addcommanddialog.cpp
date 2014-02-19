#include "addcommanddialog.h"
#include <QGridLayout>
#include <QException>

AddCommandDialog::AddCommandDialog(GameModel *model, QWidget *parent) :
    QDialog(parent), m_model(model)
{
    nameLabel = new QLabel(tr("Название"));
    nameEdit = new QLineEdit;

    errorLabel = new QLabel;
    errorLabel->setVisible(false);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(addCommand()));

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addWidget(nameLabel, 0, 0);
    mainLayout->addWidget(nameEdit, 0, 1);
    mainLayout->addWidget(errorLabel, 1, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 2, 0, 1, 2);

    setLayout(mainLayout);
    setModal(true);
    setWindowModality(Qt::WindowModal);
    setWindowTitle(tr("Добавить команду"));
}

AddCommandDialog::~AddCommandDialog()
{
    delete nameEdit;
    delete nameLabel;
    delete buttonBox;
}

void AddCommandDialog::addCommand()
{
    try {
        m_model->addCommand(nameEdit->text());
        errorLabel->setVisible(false);
        nameEdit->clear();
    } catch (QException &e) {
        errorLabel->setText(tr("Команда с таким названием уже существует"));
        errorLabel->setVisible(true);
    }
}
