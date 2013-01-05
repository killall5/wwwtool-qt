#include "addcommanddialog.h"
#include <QGridLayout>
#include <QException>

AddCommandDialog::AddCommandDialog(GameModel *model, QWidget *parent) :
    QDialog(parent), m_model(model)
{
    nameLabel = new QLabel(tr("Название"));
    nameEdit = new QLineEdit;

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(addCommand()));

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addWidget(nameLabel, 0, 0);
    mainLayout->addWidget(nameEdit, 0, 1);
    mainLayout->addWidget(buttonBox, 1, 0, 1, 2);

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

#include <QDebug>

void AddCommandDialog::addCommand()
{
    try {
        m_model->addCommand(nameEdit->text());
        nameEdit->clear();
    } catch (QException &e) {
        qDebug() << "catched" << e.what();
    }
}
