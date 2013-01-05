#include "addcommanddialog.h"
#include <QGridLayout>

AddCommandDialog::AddCommandDialog(GameModel *model, QWidget *parent) :
    QDialog(parent), m_model(model)
{
    nameLabel = new QLabel(tr("Название"));
    nameEdit = new QLineEdit;

    tableLabel = new QLabel(tr("Стол"));
    tableEdit = new QLineEdit;
    tableValidator = new QIntValidator;
    tableValidator->setBottom(1);
    tableEdit->setValidator(tableValidator);

    useFreeTableCheckBox = new QCheckBox(tr("Использовать свободный стол"));
    useFreeTableCheckBox->setChecked(model->useFreeTable());
    connect(useFreeTableCheckBox, SIGNAL(toggled(bool)), model, SLOT(setUseFreeTable(bool)));
    connect(useFreeTableCheckBox, SIGNAL(clicked()), this, SLOT(fillFields()));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(addCommand()));

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addWidget(nameLabel, 0, 0);
    mainLayout->addWidget(nameEdit, 0, 1);
    mainLayout->addWidget(tableLabel, 1, 0);
    mainLayout->addWidget(tableEdit, 1, 1);
    mainLayout->addWidget(useFreeTableCheckBox, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);

    setLayout(mainLayout);
    setModal(true);
    setWindowModality(Qt::WindowModal);
    setWindowTitle(tr("Добавить команду"));

    fillFields();
}

AddCommandDialog::~AddCommandDialog()
{
    delete tableEdit;
    delete tableLabel;
    delete tableValidator;
    delete nameEdit;
    delete nameLabel;
}

#include <QDebug>

void AddCommandDialog::addCommand()
{
    try {
        m_model->addCommand(nameEdit->text(), tableEdit->text().toInt());
        nameEdit->clear();
        fillFields();
    } catch (...) {
        qDebug() << "catched";
    }
}

void AddCommandDialog::fillFields()
{
    QString table = QString("%1").arg(m_model->getFreeTable());
    tableEdit->setText(table);
}
