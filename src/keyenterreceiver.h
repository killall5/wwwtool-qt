#ifndef KEYENTERRECEIVER_H
#define KEYENTERRECEIVER_H

#include <QObject>

class KeyEnterReceiver : public QObject
{
    Q_OBJECT
public:
    explicit KeyEnterReceiver(QObject *parent = 0);

protected:
    bool eventFilter(QObject* obj, QEvent* event);

signals:
    void enterPressed();

public slots:
};

#endif // KEYENTERRECEIVER_H
