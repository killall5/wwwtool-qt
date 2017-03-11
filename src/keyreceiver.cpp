#include "keyreceiver.h"
#include <QEvent>
#include <QKeyEvent>

KeyReceiver::KeyReceiver(QList<int> keys, QObject *parent)
    : QObject(parent), acceptedKeys(keys)
{

}

bool KeyReceiver::eventFilter(QObject *obj, QEvent *event) {
    if (event->type()==QEvent::KeyPress) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if (acceptedKeys.contains(key->key())) {
            emit acceptedKeyPressed();
        } else {
            return QObject::eventFilter(obj, event);
        }
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
    return false;
}
