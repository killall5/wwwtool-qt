#ifndef QUESTIONCOUNTDIALOG_H
#define QUESTIONCOUNTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QDialogButtonBox>

#include "gamemodel.h"

class QuestionCountDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QuestionCountDialog(GameModel *model, QWidget *parent = 0);
    ~QuestionCountDialog();
    
signals:
    
public slots:
    void accept();
private:
    QLabel *questionLabel;
    QLineEdit *questionEdit;
    QIntValidator *questionValidator;
    QDialogButtonBox *buttonBox;

    GameModel *const m_model;
};

#endif // QUESTIONCOUNTDIALOG_H
