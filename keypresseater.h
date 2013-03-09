#ifndef KEYPRESSEATER_H
#define KEYPRESSEATER_H

#include <QObject>
#include <QEvent>

class KeyPressEater : public QObject
{
    Q_OBJECT
public:
    explicit KeyPressEater(QObject *parent = 0);
    
protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    
public slots:
    
};

#endif // KEYPRESSEATER_H
