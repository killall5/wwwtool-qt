#ifndef ADDCOMMANDDIALOG_H
#define ADDCOMMANDDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QCheckBox>
#include <QDialogButtonBox>
#include "gamemodel.h"

class AddCommandDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddCommandDialog(GameModel *model, QWidget *parent = 0);
    ~AddCommandDialog();

private:
    QLineEdit *nameEdit;
    QLabel *nameLabel;

    QDialogButtonBox *buttonBox;

    GameModel * const m_model;

public slots:
    void addCommand();

};

#endif // ADDCOMMANDDIALOG_H
