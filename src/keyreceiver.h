#ifndef KEYENTERRECEIVER_H
#define KEYENTERRECEIVER_H

#include <QObject>
#include <QList>

class KeyReceiver : public QObject
{
    Q_OBJECT
public:
    explicit KeyReceiver(QList<int> keys, QObject *parent = 0);

protected:
    bool eventFilter(QObject* obj, QEvent* event);

private:
    QList<int> acceptedKeys;

signals:
    void acceptedKeyPressed();

public slots:
};

#endif // KEYENTERRECEIVER_H
