#include "keypresseater.h"

#include <QDebug>
#include "scannermanager.h"

KeyPressEater::KeyPressEater(QObject *parent) :
    QObject(parent)
{
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerFinished()));
}

KeyPressEater::~KeyPressEater() {
    timer->stop();
    delete timer;
}

bool KeyPressEater::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() != QEvent::KeyPress) {
        return QObject::eventFilter(obj, event);
    }

    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    emit keyPressed(keyEvent);
    if (keyEvent->key() == Qt::Key_Return) {
        timer->stop();
        timerFinished();
    } else {
        scannedText += keyEvent->text();
        timer->start(50);
    }
    return true;
}

void KeyPressEater::timerFinished() {
    ScannerManager::instance()->textScanned(scannedText);
    scannedText.clear();
}
