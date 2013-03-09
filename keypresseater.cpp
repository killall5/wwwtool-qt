#include "keypresseater.h"

#include <QKeyEvent>
#include <QDebug>

KeyPressEater::KeyPressEater(QObject *parent) :
    QObject(parent)
{
}

bool KeyPressEater::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() != QEvent::KeyPress) {
        return QObject::eventFilter(obj, event);
    }

    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    qDebug() << "Ate key press" << keyEvent->text() << keyEvent->isAutoRepeat();
    return true;
}
